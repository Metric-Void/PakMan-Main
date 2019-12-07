#include "alarm.h"
#include <stdint.h>
#include "states.h"

#define ALARM_THRES 5

/* Not required. Just for noting myself. */
extern bool armed;
extern bool alarm_triggered;

uint8_t counter;

void start_monitor() {
	armed = true;
	alarm_triggered = false;
	counter = 0;
	__TIM4_CLK_ENABLE();
	HAL_TIM_Base_Init(&htim4);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

void stop_monitor() {
	armed = false;
	alarm_triggered = false;
	__TIM4_CLK_DISABLE();
	HAL_TIM_Base_Stop(&htim4);
	HAL_NVIC_DisableIRQ(TIM4_IRQn);
}

void timer_trigger() {
	if(!armed) {
		stop_monitor();
		return;
	}
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
		debug_print("Cabinet missing.\n");
	} else {
		debug_print("No cabinet missing.\n");
		counter = 0;
	}

	if(counter >= ALARM_THRES) {
		System_FSM = SYS_ALARM;
		alarm_triggered = true;
		state_alarm();
	}
}

void Alarm_Interrupt() {
	timer_trigger();
}
