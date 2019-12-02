#include "alarm.h"
#include <stdint.h>

#define ALARM_THRES 5

extern bool armed;
uint8_t counter;

void start_monitor() {
	armed = true;
	counter = 0;
}

void stop_monitor() {
	armed = false;
}

void timer_trigger() {
	bool anyOffline = false;
	for(uint8_t i = 0; i < 128; i++) {
		if(addrList[i])
			if(HAL_I2C_IsDeviceReady(&hi2c2, i, 2, 500) != HAL_OK) {
				anyOffline = true;
				break;
			}
	}

	if(anyOffline) {
		counter += 1;
	} else {
		counter = 0;
	}

	if(counter >= ALARM_THRES) {
		System_FSM = SYS_ALARM;
	}
}

void Alarm_Interrupt() {
	timer_trigger();
}
