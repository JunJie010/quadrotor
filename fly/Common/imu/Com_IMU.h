#ifndef __COM_IMU_H
#define __COM_IMU_H
#include "Com_Debug.h"
#include "Com_Config.h"


/* ����ŷ�����õ���3������ */
extern float RtA;   // ����->��
// �����ǳ�ʼ������+-2000��/���� 1/(65536 / 4000) = 0.03051756*2
// float Gyro_G = 0.03051756f * 2;
extern float Gyro_G;   // ��/s
// ��ÿ��,ת������ÿ���� 2*0.03051756 * 0.0174533f = 0.0005326*2
// float Gyro_Gr = 0.0005326f * 2;
extern float Gyro_Gr;   // ����/s
void Common_IMU_GetEulerAngle(GyroAccel_Struct  *gyroAccel,
                              EulerAngle_Struct *eulerAngle,
                              float              dt);

float Common_IMU_GetNormAccZ(void);

#endif

