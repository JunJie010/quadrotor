#ifndef __COMMON_PID_H__
#define __COMMON_PID_H__
#include <stdint.h>

typedef struct               // 单环PID结构体
{
    float kp;
    float ki;
    float kd;                //PID参数
    float target;            //目标值
    float measure;           //测量值
    float sum;               //积分和
    float last_error;        //上一次误差
    float dt;                //时间间隔
    float out;               //输出
} Single_PID_Handle_t;          

void Common_Single_PID(Single_PID_Handle_t *pid_handle);

void Common_Serial_PID(Single_PID_Handle_t *outter_pid, Single_PID_Handle_t *inner_pid);

#endif /* __COMMON_PID_H__ */