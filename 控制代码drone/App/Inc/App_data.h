#ifndef __APP_DATA_H__
#define __APP_DATA_H__

#include "Common_Types.h"
#include "Int_SI24R1.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Common_IMU.h"

#define ATGUIGU_DRONE_ADDR0 0x67
#define ATGUIGU_DRONE_ADDR1 0x76
#define ATGUIGU_DRONE_ADDR2 0x22

Data_Valid_e App_Data_Receive(RcData_t *rc_data);

THR_Status_e App_Data_Unlock(RcData_t *rc_data);

void App_Data_ProcessDroneState(RcData_t *rc_data, Drone_Status_e *drone_state, RC_Status_e rc_state);

#endif /* __APP_DATA_H__ */