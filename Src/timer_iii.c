#include "timer_iii.h"
#include "keypad.h"

bool timerOn = false;
TimerMode currentMode;
uint8_t currentCount;
uint8_t countTarget;

enum SFSM TargetFSM;

extern TIM_HandleTypeDef* htim3;

void startTimer(TimerMode ins, uint8_t delay){
	if(timerOn) return;
	__TIM3_CLK_ENABLE();
	countTarget = delay;
	currentCount = 0;
	currentMode = ins;
	HAL_TIM_Base_Init(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	timerOn = true;
}

void startTimerWTFSM(TimerMode ins, uint8_t delay, enum SFSM Target_FSM) {
	if(timerOn) return;
	__TIM3_CLK_ENABLE();
	countTarget = delay;
	currentCount = 0;
	currentMode = ins;
	TargetFSM = Target_FSM;
	HAL_TIM_Base_Init(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	timerOn = true;
}

void TimerIII_Interrupt(){
	if(++currentCount >= countTarget){
		switch(currentMode){
		case RTN_IDLE:
			System_FSM = SYS_IDLE;
			keyCache = KEY_FAKE;
			break;
		case SHDN_BKLT:
			LCD_BkltOff();
			break;
		case CHNG_SFSM:
			System_FSM = TargetFSM;
			keyCache = KEY_FAKE;
			keyAvailable = false;
			break;
		}
		HAL_TIM_Base_Stop(&htim3);
		currentMode = DEINIT;
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
		timerOn = false;
	}
}

inline void LCD_BkltOff(void) {
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET);
}

inline void LCD_BkltOn(void) {
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET);
}

void cancelTimer(TimerMode ifcatch) {
	if(timerOn && currentMode == ifcatch) {
		currentMode = DEINIT;
		HAL_TIM_Base_Stop(&htim3);
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
		timerOn = false;
		currentCount = 0;
	}
}
