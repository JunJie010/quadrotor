#include "Common_PID.h"

void Common_Single_PID(Single_PID_Handle_t *pid_handle)             //定义单环PID函数
{
    float current_error = pid_handle->target - pid_handle->measure;         //定义当前误差等于目标值减去测量值
    
    float p_part = pid_handle->kp * current_error;                                                //比例项

    pid_handle->sum += current_error * pid_handle->dt;       //积分和
    float i_part = pid_handle->ki * pid_handle->sum;                                              //积分项

    float d_part = pid_handle->kd * (current_error - pid_handle->last_error) / pid_handle->dt;    //微分项

    pid_handle->out = p_part + i_part + d_part;                                                   //输出等于三项之和

    pid_handle->last_error = current_error;          //更新上一次误差
}

void Common_Serial_PID(Single_PID_Handle_t *outter_pid, Single_PID_Handle_t *inner_pid)       //定义串级PID函数
{
    Common_Single_PID(outter_pid);             // 先算外环PID 产生输出值
    inner_pid->target = outter_pid->out;       // 将外环PID的输出值作为内环PID的目标值
    Common_Single_PID(inner_pid);              // 再算内环PID 产生最终输出值
}
