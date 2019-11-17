#include "lcd.h"

/**
 * This file defines the global hardware, as well as the state machine.
 *
 * SYS_IDLE: non-locking, sleeping
 * 		The system in its idle state. The MCU should be put into sleeping in this state.
 * 		This state is entered whenever an operation is done, or the system was sit idle for too long.
 * 		This state is left when triggered by an interrupt.
 * SYS_IDLE_PWDWAIT: non-locking, sleeping
 * 		The keypad has been pressed, and the MCU is waiting for password input.
 * 		This is a non-locking state. Entered by typing on the keypad at SYS_IDLE.
 * 		Enter sleeping mode when waiting for input. A keyboard interrupt will return to the original program location
 * 		And the input will be logged, then sleep again until confirm key is pressed.
 * 		Left when password confirmed, or idle for too long.
 * SYS_IDLE_PWDFAULT: non-locking, sleeping
 * 		Wrong password entered. Displays Error scr, and enter sleeping.
 * 		Entered when wrong password in entered in SYS_IDLE_PWDWAIT.
 * 		Left 1. When keypad is pressed, return to SYS_IDLE_PWDWAIT.
 * 		     2. When sit idle, return to SYS_IDLE.
 * SYS_MGMT_MAIN: non-locking, sleeping
 * 		This is the selection menu for the management interface.
 * 		Entered with the correct password from SYS_IDLE_PWDWAIT
 * 		Interrupt from the keypad indicates a menu item being selected.
 */
#ifndef _PROGUI
#define _PROGUI

enum SFSM {
	/* Idle state. Shows idle screen. */
	SYS_IDLE = 0U,

	/* Wait for password input. Keypad callback. */
	SYS_IDLE_PWDWAIT, SYS_IDLE_PWDWAIT_KEYREADY, SYS_IDLE_PWDWAIT_CALLBACK,

	/* Wrong password. Keypad callback. */
	SYS_IDLEAUTH_WRONGPWD, SYS_IDLEAUTH_WRONGPWD_CALLBACK,

	/* Management interface. Keypad callback. */
	SYS_MGMT, SYS_MGMT_CALLBACK,

	/* Retrieving procedure. */
	SYS_OPEN_ALL_WAIT, SYS_OPEN_ALL_DONE,

	/* Settings menu */
	SYS_SETTINGS, SYS_SETTINGS_CALLBACK,

	/* Reconfiguration */
	SYS_RECONF, SYS_RECONF_CALLBACK,
	SYS_RECONF_DETECT, SYS_RECONF_DONE,

	/* Reset password */
	SYS_RESET_PWD, SYS_RESET_PWD_CALLBACK,

	/* Wait for the user to select operation. */
	SYS_CHOOSE_OP, SYS_CHOOSE_OP_CALLBACK,

	/* Package number input. */
	SYS_DROP_PKG, SYS_DROP_PKG_CALLBACK,

	/* Wait for cabinet key press. */
	SYS_DELI_WAIT_CHOICE,

	/* Ask if delivery OK/ Failed. */
	SYS_DELI_OKFAIL, SYS_DELI_OKFAIL_CALLBACK,

	/* Another package? */
	SYS_DELI_IF_REPEAT, SYS_DELI_IF_REPEAT_CALLBACK,

	SYS_DEINIT
};

enum SFSM System_FSM;
LCD_TypeDef *global_lcd;

#endif
