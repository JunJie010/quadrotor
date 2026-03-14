#include "App_data.h"

uint8_t rx_buffer[17];
Data_Valid_e App_Data_Receive(RcData_t *rc_data)
{
    uint8_t res = Int_SI24R1_RxPacket(rx_buffer);
    if (res == 0)
    {
        if ((rx_buffer[10] == ATGUIGU_DRONE_ADDR0) && (rx_buffer[11] == ATGUIGU_DRONE_ADDR1) && (rx_buffer[12] == ATGUIGU_DRONE_ADDR2))
        {
            uint32_t check_sum = 0;
            for (uint8_t i = 0; i < 13; i++)
            {
                check_sum += rx_buffer[i];
            }
            if (check_sum == ((rx_buffer[13] << 24) | (rx_buffer[14] << 16) | (rx_buffer[15] << 8) | rx_buffer[16]))
            {
                // 才认为这是合规数据
                rc_data->throttle = (rx_buffer[0] << 8) | rx_buffer[1];
                rc_data->pitch = (rx_buffer[2] << 8) | rx_buffer[3];
                rc_data->roll = (rx_buffer[4] << 8) | rx_buffer[5];
                rc_data->yaw = (rx_buffer[6] << 8) | rx_buffer[7];                
                rc_data->off = rx_buffer[8];
                rc_data->hold_height = rx_buffer[9];
                return eData_Valid;
            }
        }
    }
    return eData_Invalid;
}

THR_Status_e current_thr_state = eFREE;
uint32_t enter_eMAX_tick = 0;
uint32_t enter_eMIN_tick = 0;
THR_Status_e App_Data_Unlock(RcData_t *rc_data)
{
    switch (current_thr_state)
    {
    case eFREE:
        if (rc_data->throttle >= 900)
        {
            current_thr_state = eMAX;
            enter_eMAX_tick = xTaskGetTickCount();
        }

        break;
    case eMAX:
        if (rc_data->throttle < 900)
        {

            if (xTaskGetTickCount() - enter_eMAX_tick >= 1000)
            {
                current_thr_state = eLEAVE_MAX;
            }
            else
            {
                current_thr_state = eFREE;
            }
        }

        break;
    case eLEAVE_MAX:
        if (rc_data->throttle <= 100)
        {
            current_thr_state = eMIN;
            enter_eMIN_tick = xTaskGetTickCount();
        }

        break;
    case eMIN:
        if (rc_data->throttle > 100)
        {
            if (xTaskGetTickCount() - enter_eMIN_tick < 1000)
            {
                current_thr_state = eFREE;
            }
        }
        else
        {
            if (xTaskGetTickCount() - enter_eMIN_tick >= 1000)
            {
                current_thr_state = eUNLOCK;
            }
        }

        break;
    case eUNLOCK:
        break;
    }

    return current_thr_state;
}

    extern uint16_t height;           // 高度
    extern uint16_t fix_height;       //定高目标值

    extern float static_az;           //静态Z轴加速度

void App_Data_ProcessDroneState(RcData_t *rc_data, Drone_Status_e *drone_state, RC_Status_e rc_state)
{
    switch (*drone_state)
    {
    case eDrone_IDLE:
        if (App_Data_Unlock(rc_data) == eUNLOCK)
        {
            *drone_state = eDrone_NORMAL;
            static_az = Common_IMU_GetNormAccZ();     //获取静态Z轴加速度
        }
        break;

    case eDrone_NORMAL:
        if (rc_state == eRC_UNCONNECTED)
        {
            *drone_state = eDrone_FAULT;
        }
        else
        {
            if (rc_data->hold_height == 1)
            {
                *drone_state = eDrone_HOLD_HIGHT;
                //让fix_height = 当前的高度
                fix_height = height;
            }
        }
        break;
    case eDrone_HOLD_HIGHT:
        if (rc_state == eRC_UNCONNECTED)
        {
            *drone_state = eDrone_FAULT;
        }
        else
        {
            if (rc_data->hold_height == 0)
            {
                *drone_state = eDrone_NORMAL;
            }
        }
        break;
    case eDrone_FAULT:
        if (rc_state == eRC_CONNECTED)
        {
            *drone_state = eDrone_NORMAL;
        }
        break;
    }
}