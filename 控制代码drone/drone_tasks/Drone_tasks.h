#ifndef __DRONE_TASKS_H__
#define __DRONE_TASKS_H__

#include "FreeRTOS.h"

#include "task.h"
#include "main.h"
#include "Common_Types.h"
#include "Int_LED.h"
#include "Int_Motor.h"
#include "Int_SI24R1.h"
#include <string.h>
#include "App_data.h"
#include "Int_MPU6050.h"
#include "Common_Filter.h"
#include "App_motor.h"
#include "Common_IMU.h"
#include "Common_PID.h"
#include "Int_VL53L1X.h"

void Start_all_tasks(void);

#endif /* __DRONE_TASKS_H__ */