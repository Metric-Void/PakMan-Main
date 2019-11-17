#ifndef __I2C_UTILS_CU_H__
#define __I2C_UTILS_CU_H__

#include "locker.h"
#include <stdbool.h>

bool addrList[128];

extern void scr_scan();

void scanBus();

void i2c_scanCabinets();

void i2c_openAll();

bool is_occupied(Cabinet c);

void i2c_setOccupied(Cabinet * c, bool status);

#endif
