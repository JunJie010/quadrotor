#ifndef __DRONE_TASKS_H__
#define __DRONE_TASKS_H__

#include "FreeRTOS.h"

#include "task.h"
#include "main.h"
#include "Int_Key.h"
#include "Int_Joystick.h"
#include "App_Data.h"
#include "Int_SI24R1.h"
#include "App_Display.h"

void Start_all_tasks(void);

#endif /* __DRONE_TASKS_H__ */