#include "Int_Motor.h"

Motor_Handle_t motor_left_top = {.htim = &htim3, .channel = TIM_CHANNEL_1};
Motor_Handle_t motor_left_bottom = {.htim = &htim4, .channel = TIM_CHANNEL_4};
Motor_Handle_t motor_right_top = {.htim = &htim2, .channel = TIM_CHANNEL_2};
Motor_Handle_t motor_right_bottom = {.htim = &htim1, .channel = TIM_CHANNEL_3};

void Int_Motor_Init(void)
{
    HAL_TIM_PWM_Start(motor_left_top.htim, motor_left_top.channel);
    HAL_TIM_PWM_Start(motor_left_bottom.htim, motor_left_bottom.channel);
    HAL_TIM_PWM_Start(motor_right_top.htim, motor_right_top.channel);
    HAL_TIM_PWM_Start(motor_right_bottom.htim, motor_right_bottom.channel);
}

void Int_Motor_UpdateSpeed(Motor_Handle_t *motor_handle)
{
    __HAL_TIM_SET_COMPARE(motor_handle->htim, motor_handle->channel, motor_handle->speed);
}