#ifndef __TIMER_3_H__
#define __TIMER_3_H__

#include <stm32f407xx.h>
#include <stdbool.h>
#include <stm32f4xx_it.h>
#include "progui_fsm.h"

extern enum SFSM;

typedef enum __timer_op {
	DEINIT=0U, RTN_IDLE, SHDN_BKLT, CHNG_SFSM
} TimerMode;

void startTimer(TimerMode ins, uint8_t delay);

void startTimerWTFSM(TimerMode ins, uint8_t delay, enum SFSM Target_FSM);

void LCD_BkltOn(void);

void LCD_BkltOff(void);

void cancelTimer(TimerMode ifcatch);

#endif
