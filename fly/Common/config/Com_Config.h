#ifndef __COM_CONFIG_H
#define __COM_CONFIG_H

#include "stdint.h"
#include "stdio.h"

typedef enum
{
    Com_OK = 0,
    Com_TIMEOUT,
    Com_FAIL
} Com_Status;

typedef enum
{
    LEFT_TOP,
    LEFT_BOTTOM,
    RIGHT_TOP,
    RIGHT_BOOTTOM
} Com_Location;

typedef struct
{
    /* ��ʾ�Ƶ�λ�� */
    Com_Location location;

    /* ��ʾ�Ƶ�״̬:
        0: ����
        1: ����
        2+: ��˸����
     */
    uint8_t status;
} LedStruct;

/* ��ʾ����Ľṹ�� */
typedef struct
{
    Com_Location location;
    int16_t      speed;
} Motor_Struct;

typedef struct
{
    int16_t THR; /* ���� */
    int16_t PIT; /* ���� */
    int16_t ROL; /* ��� */
    int16_t YAW; /* ƫ�� */

    uint8_t isPowerDown; /* �Ƿ�ػ�: 1:�ػ� 0:���ػ� */
    uint8_t isFixHeight; /* �Ƿ�ת���ߵ�״̬ */
} JoyStick_Struct;

/* �������Ž���ʱ�ļ���״̬ */
typedef enum
{
    THR_FREE = 0,
    THR_MAX,
    THR_MAX_LEAVE,
    THR_MIN,
    THR_UNLOCK
} Com_RemoteStatus;

/* �洢���ٶ� */
typedef struct
{
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
} Gyro_Struct;

/* �洢���ٶ� */
typedef struct
{
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
} Accel_Struct;

/* ���ٶȺͼ��ٶ� */
typedef struct
{
    Gyro_Struct  gyro;
    Accel_Struct accel;
} GyroAccel_Struct;

/* ŷ���� */
typedef struct
{
    float pitch;
    float roll;
    float yaw;

} EulerAngle_Struct;

/* ��ʾ��Ԫ���Ľṹ�� */
typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion_Struct;

/* ��ʾpid �ṹ�� */
typedef struct
{
    float kp; /* ����ϵ�� */
    float ki; /* ����ϵ�� */
    float kd; /* ΢��ϵ�� */

    float dt; /* ����ʱ�� */

    float integral;  /*���� ����ֵ */
    float lastError; /* �ϴ���� */

    float desire; /* ����ֵ */
    float measure; /* ����ֵ */

    float result;  /* pid���ս�� */ 

} PID_Struct;

/* 2.4gͨѶ�����ݸ��ص��Զ���֡ͷ */
#define FRAME_0 (0x11)
#define FRAME_1 (0x22)
#define FRAME_2 (0x33)

#define LIMIT(x, min, max) (x) >= (max) ? (max) : ((x) <= (min) ? (min) : (x))

extern LedStruct ledLeftTop;
extern LedStruct ledLeftBootom;
extern LedStruct ledRightTop;
extern LedStruct ledRightBottom;

extern Com_Status isRemoteConnected;
extern Com_Status isRemoteUnlocked;
extern Com_Status isFixHeight;

extern Motor_Struct motorLeftTop;
extern Motor_Struct motorLeftBottom;
extern Motor_Struct motorRightTop;
extern Motor_Struct motorRightBottom;

extern JoyStick_Struct joyStick;

extern GyroAccel_Struct gyroAccel;

extern EulerAngle_Struct eulerAngle;

void Com_Config_PrintJoyStick(uint8_t *pre);

void Com_Config_PrintGyroAccel(uint8_t *pre, GyroAccel_Struct *gyroAccel);

void Com_Config_PrintEulerAngle(uint8_t *pre, EulerAngle_Struct *eulerAngle);

#endif
