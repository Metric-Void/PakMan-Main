#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#ifndef _locker_h
#define _locker_h

#define nullptr ((void*)0)

/* DELIMETER is a multiplier, in the unit of centimeters. */

/* All cabinets are not expected to exceed the size of DELIMETER*MAXINDEX */
/* in the unit of centimeters*/
#define DELIMETER 50
#define MAXINDEX 4

typedef struct _cabinet {
	uint8_t i2c_addr;
	uint8_t cab_id;
	uint8_t size_width;
	uint8_t size_height;
	bool occupied;
} Cabinet;

typedef struct _cabinlist {
	Cabinet cab;
	struct _cabinlist* next;
} CabinetList;

CabinetList* cabinetList;

Cabinet* getCabinet(int pack_width, int pack_height);

Cabinet * getCabinetById(uint8_t id);

Cabinet * getCabinetByAddr(uint8_t addr);

bool addCabinet(int width, int height, int id, int addr);

bool directAddCabinet(Cabinet x);

void initCabinets(void);

void removeCabinetById(uint8_t cab_id);

void cab_clearAll(void);

void cab_clAllHelper(CabinetList* ptr);

void cab_setAllEmpty(void);

/* This is used for testing */
void findPrintCabinet(int pw, int ph);

#endif
