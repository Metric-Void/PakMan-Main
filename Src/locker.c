#include "locker.h"
#include <stdio.h>
#include <string.h>
#include "i2c_util.h"

/**
 * Get the cabinet that's most suitable for this package.
 * May return nullptr if all cabinets are too large.
 */
Cabinet* getCabinet(int pack_width, int pack_height) {
	/* Temporary storage for finding cabinets.*/
	Cabinet* currBest = nullptr;
	uint16_t bestOffset = UINT16_MAX;

	CabinetList * curr = cabinetList;
	while(curr != nullptr) {
		if (curr->cab.size_width >= pack_width && curr->cab.size_height >= pack_height) {
			uint16_t t = curr->cab.size_height * curr->cab.size_width - pack_height * pack_width;
			if (t < bestOffset) {
				currBest = &(curr->cab);
				bestOffset = t;
			}
		}
	}
	return currBest;
}

Cabinet * getCabinetById(uint8_t id) {
	CabinetList * ptr = cabinetList;
	while(ptr != nullptr) {
		if(ptr -> cab.cab_id == id) {
			return &(ptr -> cab);
		}
		ptr = ptr -> next;
	}
	return nullptr;
}

Cabinet * getCabinetByAddr(uint8_t addr) {
	CabinetList * ptr = cabinetList;
	while(ptr != nullptr) {
		if(ptr -> cab.i2c_addr == addr) {
			return &(ptr -> cab);
		}
		ptr = ptr -> next;
	}
	return nullptr;
}

bool addCabinet(int width, int height, int id, int addr) {
#ifdef DEBUG
	printf("A cabinet is added with width: %d, height: %d, id: %d\n",
			width, height, id);
#endif
	Cabinet newCabinet;
	newCabinet.cab_id = id;
	newCabinet.i2c_addr = addr;
	newCabinet.size_width = width;
	newCabinet.size_height = height;

	if(cabinetList == nullptr) {
		cabinetList -> cab = newCabinet;
	} else {
		CabinetList * newNode = (CabinetList*) malloc(sizeof(CabinetList));
		newNode -> cab = newCabinet;
		newNode -> next = nullptr;

		CabinetList * ptr = cabinetList;
		while(ptr -> next !=nullptr) ptr = ptr -> next;
		ptr -> next = newNode;
	}
}

bool directAddCabinet(Cabinet x) {
#ifdef DEBUG
	printf("A cabinet is added with width: %d, height: %d, id: %d\n",
			x.size_width, x.size_height, x.cab_id);
#endif

	if(cabinetList == nullptr) {
		cabinetList = malloc(sizeof(CabinetList));
		cabinetList -> cab = x;
		cabinetList -> next = nullptr;
	} else {
		CabinetList * newNode = (CabinetList*) malloc(sizeof(CabinetList));
		newNode -> cab = x;
		newNode -> next = nullptr;

		CabinetList * ptr = cabinetList;
		while(ptr -> next !=nullptr) ptr = ptr -> next;
		ptr -> next = newNode;
	}
	return true;
}

void initCabinets(void) {
	cabinetList = nullptr;
}

void removeCabinetById(uint8_t cab_id) {
	CabinetList * ptr = cabinetList;
	CabinetList * lastPtr = nullptr;
	while(ptr != nullptr) {
		if(ptr -> cab.cab_id == cab_id) {
			if(lastPtr == nullptr) {
				cabinetList = cabinetList -> next;
			} else {
				lastPtr -> next = ptr -> next;
				free(ptr);
			}
			return;
		} else {
			lastPtr = ptr;
			ptr = ptr -> next;
		}
	}
}

/* This is used for testing */
void findPrintCabinet(int pw, int ph) {
	printf("[ADT] Cabinet for package width=%d, height=%d: ", pw, ph);
	Cabinet* find = getCabinet(pw, ph);
	if (find == nullptr) {
		printf("Not available.\n");
	} else {
		printf("%d\n", find->cab_id);
	}
}

void cab_setAllEmpty(void) {
	CabinetList * ptr = cabinetList;
	while(ptr != nullptr) {
		i2c_setOccupied(&(ptr -> cab), false);
		ptr = ptr -> next;
	}
}

/*
 * Remove all cabinets. Free the memory properly.
 */
void cab_clearAll(void) {
	cab_clAllHelper(cabinetList);
	cabinetList = nullptr;
}

/*
 * Helper method for removing all cabinets.
 */
void cab_clAllHelper(CabinetList* ptr) {
	if(ptr == nullptr){
		return;
	} else {
		cab_clAllHelper(ptr->next);
		free(ptr);
		return;
	}
}
