#include "App_Data.h"

int16_t pitch_offset = 0;
int16_t roll_offset = 0;
int16_t yaw_offset = 0;
int16_t throttle_offset = 0;

int16_t pitch_adjust = 0;
int16_t roll_adjust = 0;

#define LIMIT(x, min, max) (x < min ? min : (x > max ? max : x))

void App_Data_ProcessJoystick(RcData_t *RCdata)
{
    // 1. 先通过Int层 读到摇杆的数据 但是这个是原始数据
    // 问题1  方向是反的
    // 问题2  0~4095 不直观 感觉变成0~1000更优雅
    taskENTER_CRITICAL();
    Int_Joystick_ReadData(RCdata);

    // 2. 解决反向问题
    RCdata->pitch = 4095 - RCdata->pitch;
    RCdata->roll = 4095 - RCdata->roll;
    //RCdata->yaw = 4095 - RCdata->yaw;
    RCdata->throttle = 4095 - RCdata->throttle;

    // 3. 值域 0~1000
    RCdata->pitch = RCdata->pitch * 1000 / 4095;
    RCdata->roll = RCdata->roll * 1000 / 4095;
    RCdata->yaw = RCdata->yaw * 1000 / 4095;
    RCdata->throttle = RCdata->throttle * 1000 / 4095;

    // 4. 减去校准偏移量
    RCdata->pitch -= pitch_offset;
    RCdata->roll -= roll_offset;
    RCdata->yaw -= yaw_offset;
    RCdata->throttle -= throttle_offset;

    // 5. 俯仰加上我们的微调数据
    RCdata->pitch += pitch_adjust;
    RCdata->roll += roll_adjust;

    // 6. 值域保护 (0~1000)
    RCdata->pitch = LIMIT(RCdata->pitch, 0, 1000);
    RCdata->roll = LIMIT(RCdata->roll, 0, 1000);
    RCdata->yaw = LIMIT(RCdata->yaw, 0, 1000);
    RCdata->throttle = LIMIT(RCdata->throttle, 0, 1000);
    taskEXIT_CRITICAL();

    // printf("pitch:%d,roll:%d,yaw:%d,throttle:%d,off:%d,hold_height:%d\n", RCdata->pitch, RCdata->roll, RCdata->yaw, RCdata->throttle,RCdata->off,RCdata->hold_height);
}

void App_Data_ProcessKey(RcData_t *RCdata)
{
    // 1读取到哪个按键被按下。
    KEY_TYPE_e key = Int_KeyScan();

    if (key == KEY_RIGHT_X_LONG)
    {
        pitch_offset = RCdata->pitch - 500;
        roll_offset = RCdata->roll - 500;
        yaw_offset = RCdata->yaw - 500;
        throttle_offset = RCdata->throttle - 0;
    }
    else if (key == KEY_UP)
    {
        pitch_adjust += 5;
    }
    else if (key == KEY_DOWN)
    {
        pitch_adjust -= 5;
    }
    else if (key == KEY_RIGHT)
    {
        roll_adjust += 5;
    }
    else if (key == KEY_LEFT)
    {
        roll_adjust -= 5;
    }
    else if (key == KEY_LEFT_X)
    {
        RCdata->off = 1;
    }
    else if (key == KEY_RIGHT_X)
    {
        RCdata->hold_height ^= 1;
    }
}

uint8_t tx_buffer[17];
void App_Data_Send(RcData_t *RCdata)
{
    // 月薪过万协议
    tx_buffer[0] = RCdata->throttle >> 8;
    tx_buffer[1] = RCdata->throttle & 0xff;
    tx_buffer[2] = RCdata->pitch >> 8;
    tx_buffer[3] = RCdata->pitch & 0xff;
    tx_buffer[4] = RCdata->roll >> 8;
    tx_buffer[5] = RCdata->roll & 0xff;
    tx_buffer[6] = RCdata->yaw >> 8;
    tx_buffer[7] = RCdata->yaw & 0xff;
    tx_buffer[8] = RCdata->off;
    tx_buffer[9] = RCdata->hold_height;
    tx_buffer[10] = ATGUIGU_DRONE_ADDR0;
    tx_buffer[11] = ATGUIGU_DRONE_ADDR1;
    tx_buffer[12] = ATGUIGU_DRONE_ADDR2;
    // 再发明一种校验逻辑
    uint32_t check_sum = 0;
    for (uint8_t i = 0; i < 13; i++)
    {
        check_sum += tx_buffer[i];
    }
    tx_buffer[13] = check_sum >> 24;
    tx_buffer[14] = check_sum >> 16;
    tx_buffer[15] = check_sum >> 8;
    tx_buffer[16] = check_sum & 0xff;
    uint8_t tx_res = Int_SI24R1_TxPacket(tx_buffer);
    if (tx_res == TX_DS)
    {
        RCdata->off = 0;
    }
}
