#ifndef __INF_MOTOR_H
#define __INF_MOTOR_H
#include "Com_Debug.h"
#include "tim.h"

#include "Com_Config.h"

void Inf_Motor_Init(void);

void Inf_Motor_SetSpeed(Motor_Struct *motor);

void Inf_Motor_AllMotorsWork(void);
#endif

