#include "keypad.h"
#include <stdlib.h>
#include <string.h>

const KeyInput symmap[4][4] = { { KEY_1, KEY_4, KEY_7, KEY_ASTERISK },
								{ KEY_2, KEY_5, KEY_8, KEY_0 },
								{ KEY_3, KEY_6, KEY_9, KEY_POUND },
								{ KEY_A, KEY_B, KEY_C, KEY_D } };

struct __rowcol {
	uint8_t row;
	uint8_t col;
};

extern bool keyAvailable;
extern KeyInput keyCache;

KeyInput getKeys() {
	return keyCache;
}

KeyInput getSingleKey() {
	return keyCache;
}

extern PortPin_Map col[4], row[4];

void keypad_init(PortPin_Map up1, PortPin_Map up2, PortPin_Map up3,
		PortPin_Map up4, PortPin_Map dn1, PortPin_Map dn2, PortPin_Map dn3,
		PortPin_Map dn4) {
	row[0] = up1;
	row[1] = up2;
	row[2] = up3;
	row[3] = up4;

	col[0] = dn1;
	col[1] = dn2;
	col[2] = dn3;
	col[3] = dn4;

	HAL_GPIO_WritePin(col[0].GPIOx, col[0].GPIO_Pin_x, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(col[1].GPIOx, col[1].GPIO_Pin_x, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(col[2].GPIOx, col[2].GPIO_Pin_x, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(col[3].GPIOx, col[3].GPIO_Pin_x, GPIO_PIN_RESET);
	keyAvailable = false;
}

void readOneKey() {
	if (keyAvailable) {
		return;
	}

	PortPin_Map GPIOx_watch;

	/* Clear keycache. */
	keyCache = 0;

	struct __rowcol temp;
	keyAvailable = true;
	memset(&temp, 0, sizeof(temp));

	/* Magic number as a flag. */
	/* If its value doesn't change, then this is a fake keypress. */
	temp.row = 42;

	for (uint8_t i = 0; i < 4; i += 1) {
		if (HAL_GPIO_ReadPin(row[i].GPIOx, row[i].GPIO_Pin_x) == GPIO_PIN_RESET) {
			temp.row = i;
			GPIOx_watch = row[i];
			break;
		}
	}

	if(temp.row == 42) {
		keyAvailable = false;
		keyCache = KEY_FAKE;
		return;
	}

	for (uint8_t i = 0; i < 4; i += 1) {
		HAL_GPIO_WritePin(col[i].GPIOx, col[i].GPIO_Pin_x, GPIO_PIN_SET);
		if (HAL_GPIO_ReadPin(GPIOx_watch.GPIOx, GPIOx_watch.GPIO_Pin_x)
				== GPIO_PIN_SET) {
			temp.col = i;
		}
		HAL_GPIO_WritePin(col[i].GPIOx, col[i].GPIO_Pin_x, GPIO_PIN_RESET);
	}

	keyCache = symmap[temp.row][temp.col];
}
