#include <stdbool.h>
#include <stm32f4xx.h>
#include <stdint.h>
#include "i2c_util.h"
#include "progui_fsm.h"
#include "main.h"

extern bool addrList[128];

extern I2C_HandleTypeDef hi2c2;

const uint8_t ascii_ENQ = 0x05;

const uint8_t cmnd_reqWidth = 0xF0;
const uint8_t cmnd_reqHeight = 0xF1;
const uint8_t cmnd_reqId = 0xF2;
const uint8_t cmnd_reqOcc = 0xF3;
const uint8_t cmnd_openCabinet = 0xF4;
const uint8_t cmnd_isPressed = 0xF5;
const uint8_t cmnd_setOcc = 0xF6;
const uint8_t cmnd_clearOcc = 0xF7;

const uint8_t data_true = 0xFF;
const uint8_t data_false = 0x00;

void scanBus() {
	bool known_ids[128];
	memset(known_ids,0,sizeof(known_ids));

	for (uint16_t i = 0x09; i < 128; i += 1) {
		// Send 0x05 ENQ. Device should reply with 0x06 ACK.
		uint8_t receive_buffer;

#ifdef DEBUG
		char debug_buffer[30];
		sprintf(debug_buffer,"Scanning address %d\n",i);
		debug_print(debug_buffer);
#endif

		/* See if anything responds to this address. */
		if(HAL_I2C_IsDeviceReady(&hi2c2, i, 2, 500) == HAL_OK) {

#ifdef DEBUG
			sprintf(debug_buffer, "Address %d is ready.\n",i);
			debug_print(debug_buffer);
#endif
			/* Address has responded. Log its ID. */
			if (HAL_I2C_Master_Transmit(&hi2c2, i, &ascii_ENQ, 1, 500) == HAL_OK) {
				HAL_I2C_Master_Transmit(&hi2c2, i, &cmnd_reqId, 1, 500);
				HAL_Delay(1);
				HAL_I2C_Master_Receive(&hi2c2, i, &receive_buffer, 1, 500);
#ifdef DEBUG
				sprintf(debug_buffer, "Address %d reached.\n",i);
				debug_print(debug_buffer);
#endif
				if(known_ids[receive_buffer] == true){
					sprintf(debug_buffer, "Address %d has conflicting id %d.\n",i,receive_buffer);
					debug_print(debug_buffer);
					continue;
				}
				else {
					addrList[i] = true;
					known_ids[receive_buffer] = true;
				}
			}
		} else {
			sprintf(debug_buffer, "Address %d is not ready.\n",i);
			debug_print(debug_buffer);
		}
	}
}

void i2c_scanCabinets() {
	// Put something on the LCD screen.
	scr_scan();
	scanBus();
	uint8_t receive_buffer;
	cab_clearAll();
	for (uint8_t i = 1; i < 64; i += 1) {
		if (addrList[i]) {
			Cabinet newCabinet;
			// F2 - Ask for cabinet ID
			HAL_I2C_Master_Transmit(&hi2c2, i, &cmnd_reqId, 1, 500);
			HAL_Delay(3);
			HAL_I2C_Master_Receive(&hi2c2, i, &receive_buffer, 1, 500);
			newCabinet.cab_id = receive_buffer;
			// Here begins some custom protocol transfer.
			// F0 - Ask for cabinet width
			newCabinet.i2c_addr = i;
			HAL_I2C_Master_Transmit(&hi2c2, i, &cmnd_reqWidth, 1, 500);
			HAL_Delay(3);
			HAL_I2C_Master_Receive(&hi2c2, i, &receive_buffer, 1, 500);
			newCabinet.size_width = receive_buffer;
			// F1 - Ask for cabinet height
			HAL_I2C_Master_Transmit(&hi2c2, i, &cmnd_reqHeight, 1, 500);
			HAL_Delay(3);
			HAL_I2C_Master_Receive(&hi2c2, i, &receive_buffer, 1, 500);
			newCabinet.size_height = receive_buffer;
			newCabinet.occupied = false;

			directAddCabinet(newCabinet);
		}
	}
}

void i2c_openAll(){
	for(uint8_t i=0;i<128;i++){
		if(addrList[i]){
			HAL_I2C_Master_Transmit(&hi2c2, i, &cmnd_openCabinet, 1, 500);
		}
	}
}

bool is_occupied(Cabinet c) {
	uint8_t receive_buffer;
	if (HAL_I2C_Master_Transmit(&hi2c2, c.i2c_addr, &cmnd_reqOcc, 1, 500)
			!= HAL_OK) {
		return true;// To make sure system doesn't try to put things in here.
	}
	if (HAL_I2C_Master_Receive(&hi2c2, c.i2c_addr, &receive_buffer, 1, 500)
			!= HAL_OK) {
		return true;
	}
	if (receive_buffer == 0x00)
		return false;
	return true;
}

void i2c_setOccupied(Cabinet * c, bool status) {
	char uart_tx_buffer[128];
	if(status){
		HAL_I2C_Master_Transmit(&hi2c2, c -> i2c_addr, &cmnd_setOcc, 1, 500);
		sprintf(uart_tx_buffer,"Set occupied for addr %d",c -> i2c_addr);
		debug_print(uart_tx_buffer);
	} else {
		HAL_I2C_Master_Transmit(&hi2c2, c -> i2c_addr, &cmnd_clearOcc, 1, 500);
		sprintf(uart_tx_buffer,"Set not occupied for addr %d",c -> i2c_addr);
		debug_print(uart_tx_buffer);
	}
}

/* Warning. This will free the whole program! */
/*
 * fun i2c_waitForKey;
 * Continuously query the I2C bus until a key press on some cabinet is detected.
 * Returns the I2C address of the cabinet.
 */
int i2c_waitForKey() {
	uint8_t receive_buffer;
	while(System_FSM == SYS_DELI_WAIT_CHOICE) {
		for(uint16_t i=1; i < 128; i++){
			if(addrList[i]){
				HAL_Delay(100);
				HAL_I2C_Master_Transmit(&hi2c2, i, &cmnd_isPressed, 1, 500);
/*#ifdef DEBUG
				char dbgout[30];
				sprintf(dbgout, "Waiting for reply from target %d\n",i);
				debug_print(dbgout);
#endif */
				HAL_I2C_Master_Receive(&hi2c2, i, &receive_buffer, 1, 500);
#ifdef DEBUG
				char dbgout[30];
				sprintf(dbgout, "Target %d replied %d\n",i,receive_buffer);
				debug_print(dbgout);
#endif
				if(receive_buffer == 0xFF) return i;
			}
		}
	}
	return -1;
}
