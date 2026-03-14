#ifndef _INT_VL53L1X_H_
#define _INT_VL53L1X_H_ 
#include "main.h"
#include "VL53L1X_api.h"

void Int_VL53L1X_Init(void);

uint16_t Int_VL53L1X_GetDistance(uint16_t *Distance);




#endif