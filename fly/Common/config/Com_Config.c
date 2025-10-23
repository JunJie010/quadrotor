#include "Com_Config.h"

/* ����4��LED */
LedStruct ledLeftTop     = {LEFT_TOP, 0};
LedStruct ledLeftBootom  = {LEFT_BOTTOM, 0};
LedStruct ledRightTop    = {RIGHT_TOP, 0};
LedStruct ledRightBottom = {RIGHT_BOOTTOM, 0};

/* ����ɿذ��״̬ */
/* 1. ң�����Ƿ����ӳɹ� */
Com_Status isRemoteConnected = Com_FAIL;
/* 2. ң�����Ƿ���� */
Com_Status isRemoteUnlocked = Com_FAIL;
/* 3. �Ƿ�������ģʽ */
Com_Status isFixHeight = Com_FAIL;

/* 4����� */
Motor_Struct motorLeftTop     = {LEFT_TOP, 0};
Motor_Struct motorLeftBottom  = {LEFT_BOTTOM, 0};
Motor_Struct motorRightTop    = {RIGHT_TOP, 0};
Motor_Struct motorRightBottom = {RIGHT_BOOTTOM, 0};

/* ҡ�����ݽṹ�� */
JoyStick_Struct joyStick;

/* ���������ݽṹ�� */
GyroAccel_Struct gyroAccel;

/* ŷ���� */
EulerAngle_Struct eulerAngle;

void Com_Config_PrintJoyStick(uint8_t *pre)
{
    printf("%s thr = %d, pit = %d, rol = %d, yaw = %d\r\n",
           pre,
           joyStick.THR,
           joyStick.PIT,
           joyStick.ROL,
           joyStick.YAW);
}

void Com_Config_PrintGyroAccel(uint8_t *pre, GyroAccel_Struct *gyroAccel)
{
    printf("%s gx = %d, gy = %d, gz = %d, ax = %d, ay = %d, az= %d\r\n",
           pre,
           gyroAccel->gyro.gyroX,
           gyroAccel->gyro.gyroY,
           gyroAccel->gyro.gyroZ,
           gyroAccel->accel.accelX,
           gyroAccel->accel.accelY,
           gyroAccel->accel.accelZ);
}

void Com_Config_PrintEulerAngle(uint8_t *pre, EulerAngle_Struct *eulerAngle)
{
    printf("%s pitch = %.1f, roll = %.1f, yaw = %.1f\r\n",
           pre,
           eulerAngle->pitch,
           eulerAngle->roll,
           eulerAngle->yaw);
}
