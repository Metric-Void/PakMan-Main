#include "main.h"
#include "stm32f4xx.h"
#include <stdlib.h>

#include "lcd_predefs.h"
#include "lcd.h"
#include "keypad.h"
#include <string.h>
#include "progui_fsm.h"
#include "states.h"
#include <stm32f4xx.h>
#include "config.h"
#include "i2c_util.h"
#include "locker.h"
#include "timer_iii.h"

Cabinet * cabinetOnOperation;

void state_sys_idle() {
	scr_idle();
	LCD_cursorOff(global_lcd);
	LCD_BkltOn();
	keyCache = KEY_FAKE;
	printf("IDLE Entering sleep mode.\n");

	startTimer(SHDN_BKLT, 5);
	/* Somehow this does not seem to work. */
//	HAL_SuspendTick();
//	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//	HAL_ResumeTick();

	while(System_FSM == SYS_IDLE) {}
	SystemClock_Config();
	printf("IDLE Woken up.\n");
}

/*
 * SYS_IDLE_PWDWAIT.
 * Request for password and verify it.
 */
void state_sys_auth() {
	scr_wait_password();
	LCD_cursorOn(global_lcd);
	char* keybd_cache = malloc(__CONF_PWDLEN * sizeof(char));
	memset(keybd_cache, 0, __CONF_PWDLEN * sizeof(char));
	uint8_t cachept = 0;

	uint8_t row = 1;
	uint8_t col = 0;

	while (keyCache != KEY_A && keyCache != KEY_C) {
		// Password is limited to __CONF_PWDLEN characters.
		if (keyAvailable && keyCache <= 9 && cachept < __CONF_PWDLEN) {
			// A digit is entered. Append it to the loaded cache.
			keybd_cache[cachept] = '0' + keyCache;
			cachept += 1;

			// Print an * onto the screen.
			LCD_setCursor(global_lcd, row, col++);
			LCD_putchar(global_lcd, '*');
		} else if (keyAvailable && keyCache == KEY_B) {
			// Delete a character
			if (cachept > 0) {
				keybd_cache[--cachept] = '\x00';

				// Move the cursor forward a bit.
				LCD_setCursor(global_lcd, row, --col);
				LCD_putchar(global_lcd, ' ');
				LCD_setCursor(global_lcd, row, col);
			}
		}

		// Indicates that the key has been parsed, so the next can get in.
		keyAvailable = false;

		// Enter sleep mode until an interrupt is generated.
		// Another key on the keypad can generate the interrupt.
		// Program would return to this position.

		/* Somehow this does not seem to work. */
		System_FSM = SYS_IDLE_PWDWAIT_KEYREADY;
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_IDLE_PWDWAIT_KEYREADY) {}
		SystemClock_Config();

		if (System_FSM != SYS_IDLE_PWDWAIT_CALLBACK || keyCache == KEY_C) {
			/* Panic: something else happened. Better leave this. */
			LCD_clearScreen(global_lcd);
			// free(keybd_cache);
			keyAvailable = false;
			LCD_cursorOff(global_lcd);
			return;
		} else {
			System_FSM = SYS_IDLE_PWDWAIT_KEYREADY;
			/* Everything OK. */
			/* Leave the key for the next loop. */
		}
	}

	keyAvailable = false;
	System_FSM = SYS_IDLE_PWDWAIT;

	// It should only be KEY_A now.
	// Check the password and put into states;
	LCD_cursorOff(global_lcd);
	if (passmatch(keybd_cache)) {
		System_FSM = SYS_MGMT;
	} else {
		System_FSM = SYS_IDLEAUTH_WRONGPWD;
	}

	free(keybd_cache);
}

/*
 * SYS_IDLEAUTH_WRONGPWD
 * Wrong password entered in SYS_IDLE_PWDWAIT.
 */
void state_idleauth_wrongpwd() {
	scr_wrongpwd();

	while (true) {
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_IDLEAUTH_WRONGPWD) {}
		SystemClock_Config();

		if (System_FSM != SYS_IDLEAUTH_WRONGPWD_CALLBACK) {
			return;
		} else {
			System_FSM = SYS_IDLEAUTH_WRONGPWD;
			keyAvailable = false;
			switch (keyCache) {
			case KEY_A:
				keyCache = KEY_FAKE;
				// Super not elegant but who cares
				System_FSM = SYS_IDLE_PWDWAIT;
				return;
				break;
			case KEY_B:
				System_FSM = SYS_IDLE;
				return;
				break;
			default:
				break;
			}
		}
	}
}

/*
 * Management
 * [1] Open all cabinets
 * [2] Reconfigure hardware
 * [D] Exit
 */
void state_management() {
	scr_mgmt();

	while (true) {
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_MGMT) {}
		SystemClock_Config();

		if (System_FSM != SYS_MGMT_CALLBACK) {
			return;
		} else {
			System_FSM = SYS_MGMT;
			keyAvailable = false;
			switch (keyCache) {
			case KEY_1:
				System_FSM = SYS_OPEN_ALL_WAIT;
				return;
				break;
			case KEY_2:
				System_FSM = SYS_SETTINGS;
				return;
				break;
			case KEY_D:
				System_FSM = SYS_IDLE;
				keyCache = KEY_FAKE;
				return;
				break;
			default:
				break;
			}
		}
	}
}

void state_openall(){
	scr_openall();
	i2c_openAll();
	cab_setAllEmpty();
	System_FSM = SYS_OPEN_ALL_DONE;
}

void state_openall_done(){
	scr_openall_done();

	startTimer(RTN_IDLE, 5);
	while(System_FSM == SYS_OPEN_ALL_DONE && keyAvailable == false) {}
	System_FSM = SYS_IDLE;

	System_FSM = SYS_IDLE;
}

void state_settings() {
	scr_settings();
	while (true) {
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_SETTINGS) {}
		SystemClock_Config();

		if (System_FSM != SYS_SETTINGS_CALLBACK) {
			return;
		} else {
			System_FSM = SYS_SETTINGS;
			keyAvailable = false;
			switch (keyCache) {
			case KEY_1:
				keyAvailable = false;
				keyCache = KEY_FAKE;
				System_FSM = SYS_RESET_PWD;
				return;
				break;
			case KEY_2:
				System_FSM = SYS_RECONF;
				return;
				break;
			case KEY_D:
				System_FSM = SYS_IDLE;
				return;
				break;
			default:
				break;
			}
		}
	}
}

void state_hwreconf() {
	scr_reconf_hw();
	stop_monitor();
	while(true){
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_RECONF) {}
		SystemClock_Config();

		if(System_FSM == SYS_RECONF_CALLBACK) {
			System_FSM = SYS_RECONF;
			keyAvailable = false;
			if(keyCache == KEY_A){
				System_FSM = SYS_RECONF_DETECT;
				return;
			} else if (keyCache == KEY_D){
				System_FSM = SYS_SETTINGS;
				return;
			}
		} else {
			return;
		}
	}
}

void state_hwreconf_detect() {
	scr_reconf_hw_detect();
	cab_clearAll();
	i2c_scanCabinets();
	keyAvailable = false;
	System_FSM = SYS_RECONF_DONE;
	return;
}

void state_hwreconf_done() {
	scr_reconf_hw_done();
	start_monitor();

	startTimer(RTN_IDLE, 5);
	while(System_FSM == SYS_RECONF_DONE && keyAvailable == false) {}
	System_FSM = SYS_IDLE;

	return;
}

void state_mgmt_new_password() {
	scr_mgmt_new_password();
	LCD_cursorOn(global_lcd);

	char keybd_cache[__CONF_PWDLEN];
	uint8_t cachept = 0;

	uint8_t row = 1;
	uint8_t col = 0;

	while (keyCache != KEY_A && keyCache != KEY_C) {
		// Password is limited to __CONF_PWDLEN characters.
		if (keyAvailable && keyCache <= 9 && cachept < __CONF_PWDLEN) {
			// A digit is entered. Append it to the loaded cache.
			keybd_cache[cachept] = '0' + keyCache;
			cachept += 1;

			// Print an * onto the screen.
			LCD_setCursor(global_lcd, row, col++);
			LCD_putchar(global_lcd, '*');
		} else if (keyAvailable && keyCache == KEY_B) {
			// Delete a character
			if (cachept > 0) {
				keybd_cache[--cachept] = '\x00';
				// Move the cursor forward a bit.
				LCD_setCursor(global_lcd, row, --col);
				LCD_putchar(global_lcd, ' ');
				LCD_setCursor(global_lcd, row, col);
			}
		}

		// Indicates that the key has been parsed, so the next can get in.
		keyAvailable = false;

		// Enter sleep mode until an interrupt is generated.
		// Another key on the keypad can generate the interrupt.
		// Program would return to this position.

		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_RESET_PWD) {}
		SystemClock_Config();

		if (System_FSM != SYS_RESET_PWD_CALLBACK || keyCache == KEY_C) {
			/* Panic: something else happened. Better leave this. */
			LCD_clearScreen(global_lcd);
			keyAvailable = false;
			// free(keybd_cache);
			return;
		} else {
			System_FSM = SYS_RESET_PWD;
			/* Everything OK. */
			/* Leave the key for the next loop. */
		}
	}

	// It should only be KEY_A now.
	// Write the new password.
	LCD_cursorOff(global_lcd);
	changepwd(keybd_cache);
	// free(keybd_cache);
	keyAvailable = false;
	keyCache = KEY_FAKE;
	// System_FSM = SYS_SETTINGS;
	// This is a really tiny state. Don't bother putting it into sfsm.
	state_mgmt_new_password_done();
}

void state_mgmt_new_password_done() {
	scr_mgmt_new_password_done();

	/* Being lazy... They share the same state characteristics. */
	System_FSM = SYS_RECONF_DONE;

	startTimerWTFSM(CHNG_SFSM, 2, SYS_SETTINGS);
	while(System_FSM == SYS_RECONF_DONE && keyAvailable == false) {}
	System_FSM = SYS_SETTINGS;

	return;
}

void state_choose_kbop() {
	scr_function_choose();
	while(true) {
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_CHOOSE_OP) {}
		SystemClock_Config();

		if(System_FSM == SYS_CHOOSE_OP_CALLBACK) {
			System_FSM = SYS_CHOOSE_OP;
			keyAvailable = false;
			if(keyCache == KEY_B){
				keyCache = KEY_FAKE;
				keyAvailable = false;
				System_FSM = SYS_IDLE_PWDWAIT;
				return;
			} else if (keyCache == KEY_A){
				keyCache = KEY_FAKE;
				System_FSM = SYS_DROP_PKG;
				return;
			} else if (keyCache == KEY_C){
				System_FSM = SYS_IDLE;
				return;
			}
		} else {
			return;
		}
	}
}

// SYS_DROP_PKG
void state_pkg_drop() {
	scr_get_tracknum();
	LCD_cursorOn(global_lcd);

	char pakid[40];
	memset(pakid, 0, sizeof(pakid));

	uint8_t cachept = 0;

	uint8_t row = 1;
	uint8_t col = 0;

	while (keyCache != KEY_A && keyCache != KEY_C) {
		// Password is limited to __CONF_PWDLEN characters.
		if (keyAvailable && keyCache <= 9 && cachept < 40) {
			// A digit is entered. Append it to the loaded cache.
			pakid[cachept] = '0' + keyCache;
			cachept += 1;

			// Echo the input onto screen.
			LCD_setCursor(global_lcd, row, col);
			LCD_putchar(global_lcd, '0' + keyCache);

			// Move column pointer.
			col += 1;
			if(col>=20)col-=20;
		} else if (keyAvailable && keyCache == KEY_B) {
			// Delete a character
			if (cachept > 0) {
				pakid[--cachept] = '\x00';
				// Move the cursor forward a bit.
				col -= 1;
				if(col == -1) col += 20;
				LCD_setCursor(global_lcd, row, col);
				LCD_putchar(global_lcd, ' ');
				LCD_setCursor(global_lcd, row, col);
			}
		}

		// Indicates that the key has been parsed, so the next can get in.
		keyAvailable = false;

		// Enter sleep mode until an interrupt is generated.
		// Another key on the keypad can generate the interrupt.
		// Program would return to this position.

		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_DROP_PKG) {}
		SystemClock_Config();

		if (System_FSM != SYS_DROP_PKG_CALLBACK || keyCache == KEY_C) {
			/* Panic: something else happened. Better leave this. */
			LCD_clearScreen(global_lcd);
			keyAvailable = false;
			// free(keybd_cache);
			return;
		} else {
			System_FSM = SYS_DROP_PKG;
			/* Everything OK. */
			/* Leave the key for the next loop. */
		}
	}

	LCD_cursorOff(global_lcd);

	// Should be KEY_A now.
	// Do whatever we want with pakid.

	System_FSM = SYS_DELI_WAIT_CHOICE;
}

// SYS_DELI_WAIT_CHOICE
void state_deli_wait_choice() {
	scr_deli_wait_choice();

	// This is the real procedure for getting a key.
	// But since slave hasn't been implemented yet, so..
	startTimer(RTN_IDLE, 10);
	//
	// I2C Wait button prss.
	int cabaddr = i2c_waitForKey();
	cabinetOnOperation = getCabinetByAddr(cabaddr);

#ifdef DEBUG
	char debugprt[30];
	sprintf(debugprt,"Button on addr %d is pressed.",cabaddr);
	debug_print(debugprt);
#endif
	//
	cancelTimer(RTN_IDLE);
	// HAL_Delay(5000);
	System_FSM = SYS_DELI_OKFAIL;
}

// SYS_DELI_OKFAIL
void state_deli_query() {
	scr_deli_okfail();
	while(true){
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_DELI_OKFAIL) {}
		SystemClock_Config();

		if(System_FSM == SYS_DELI_OKFAIL_CALLBACK) {
			System_FSM = SYS_DELI_OKFAIL;
			keyAvailable = false;
			if(keyCache == KEY_A){
				/* Set the cabinet to occupied. */
				i2c_setOccupied(cabinetOnOperation, true);
				System_FSM = SYS_DELI_IF_REPEAT;
				return;
			} else if (keyCache == KEY_B){
				System_FSM = SYS_DELI_IF_REPEAT;
				return;
			}
		} else {
			return;
		}
	}
}

void state_deli_ifrepeat() {
	scr_deli_if_repeat();
	while(true){
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_DELI_IF_REPEAT) {}
		SystemClock_Config();

		if(System_FSM == SYS_DELI_IF_REPEAT_CALLBACK) {
			System_FSM = SYS_DELI_IF_REPEAT;
			keyAvailable = false;
			if(keyCache == KEY_A){
				keyCache = KEY_FAKE;
				keyAvailable = false;
				System_FSM = SYS_DROP_PKG;
				return;
			} else if (keyCache == KEY_B){
				System_FSM = SYS_IDLE;
				return;
			}
		} else {
			return;
		}
	}
}

void state_alarm() {
	scr_alarm();
	while(true) {
		keyAvailable = false;
		/* Somehow this does not seem to work. */
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		HAL_ResumeTick();

		while(System_FSM == SYS_ALARM) {}
		SystemClock_Config();

		if(System_FSM == SYS_ALARM_CALLBACK) {
			System_FSM = SYS_ALARM;
			keyAvailable = false;
			if(keyCache == KEY_A) {
				keyCache = KEY_FAKE;
				keyAvailable = false;
				System_FSM = SYS_IDLE_PWDWAIT;
			}
		} else {
			return;
		}
	}
}
