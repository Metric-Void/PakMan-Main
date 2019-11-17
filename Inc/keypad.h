#ifndef __keypad_h__
#define __keypad_h__

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include <lcd.h>
#include "config.h"

#define nullptr ((void*)0)

typedef enum __key_input {
	KEY_0 = 0U,
	KEY_1 = 1U,
	KEY_2 = 2U,
	KEY_3 = 3U,
	KEY_4 = 4U,
	KEY_5 = 5U,
	KEY_6 = 6U,
	KEY_7 = 7U,
	KEY_8 = 8U,
	KEY_9 = 9U,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_ASTERISK,
	KEY_POUND,
	KEY_FAKE
} KeyInput;

bool keyAvailable;
KeyInput keyCache;

KeyInput getKeys();

KeyInput getSingleKey();

PortPin_Map col[4], row[4];

void keypad_init(PortPin_Map up1, PortPin_Map up2, PortPin_Map up3, PortPin_Map up4,
				PortPin_Map dn1, PortPin_Map dn2, PortPin_Map dn3, PortPin_Map dn4);

/*
 * Read one key from the keypad.
 * Will only set keyCache[0].
 */
void readOneKey();

#endif
