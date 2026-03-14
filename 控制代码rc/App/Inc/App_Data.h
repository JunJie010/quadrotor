#ifndef __APP_DATA_H__
#define __APP_DATA_H__
#include <stdint.h>
#include "Int_Joystick.h"
#include "Common_Types.h"
#include "Int_Key.h"
#include <stdio.h>
#include "Int_SI24R1.h"

#define ATGUIGU_DRONE_ADDR0 0x67
#define ATGUIGU_DRONE_ADDR1 0x76
#define ATGUIGU_DRONE_ADDR2 0x22

void App_Data_ProcessJoystick(RcData_t *RCdata);

void App_Data_ProcessKey(RcData_t *RCdata);

void App_Data_Send(RcData_t *RCdata);

#endif /* __APP_DATA_H__ */