#include "Int_Joystick.h"

uint16_t joystick_data_raw[4];

void Int_Joystick_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)joystick_data_raw, 4);
}

void Int_Joystick_ReadData(RcData_t *rc_data)
{
    rc_data->throttle = joystick_data_raw[0];
    rc_data->pitch = joystick_data_raw[1];
    rc_data->roll = joystick_data_raw[2];
    rc_data->yaw = joystick_data_raw[3];
}