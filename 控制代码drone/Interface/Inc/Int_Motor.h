#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__

#include "tim.h"

typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    uint16_t speed;
} Motor_Handle_t;

extern Motor_Handle_t motor_left_top;
extern Motor_Handle_t motor_left_bottom;
extern Motor_Handle_t motor_right_top;
extern Motor_Handle_t motor_right_bottom;

void Int_Motor_Init(void);

void Int_Motor_UpdateSpeed(Motor_Handle_t *motor_handle);
#endif /* __INT_MOTOR_H__ */