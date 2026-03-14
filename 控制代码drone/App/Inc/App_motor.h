#ifndef __APP_MOTOR_H__
#define __APP_MOTOR_H__
#include "Common_Types.h"
#include "Int_MPU6050.h"
#include "Common_Filter.h"
#include "Common_PID.h"
#include "Common_IMU.h"

extern Single_PID_Handle_t pitch_outter_pid;
extern Single_PID_Handle_t pitch_inner_pid;

extern Single_PID_Handle_t roll_outter_pid;
extern Single_PID_Handle_t roll_inner_pid;

extern Single_PID_Handle_t yaw_outter_pid;
extern Single_PID_Handle_t yaw_inner_pid; 

extern Single_PID_Handle_t height_outter_pid;
extern Single_PID_Handle_t height_inner_pid;            //定义外部全局变量方便调用

void App_Motor_GetAG_ByFilter(A_G_Struct_t *ag_struct);

void App_Motor_RunPID(RcData_t *rc_data, EulerAngle_Struct *euler, A_G_Struct_t *ag_struct);

void App_Motor_RunHeightPID(uint16_t fix_height,uint16_t current_height,float static_az,float dt);

#endif /* __APP_MOTOR_H__ */