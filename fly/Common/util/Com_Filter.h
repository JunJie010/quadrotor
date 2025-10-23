#ifndef __COM_FILTER_H
#define __COM_FILTER_H
#include "Com_Debug.h"
#include "stdint.h"

/* �������˲����ṹ�� */
typedef struct
{
    float LastP;   // ��һʱ�̵�״̬�����Э���
    float Now_P;   // ��ǰʱ�̵�״̬�����Э���
    float out;     // �˲��������ֵ�������Ƶ�״̬
    float Kg;      // ���������棬���ڵ���Ԥ��ֵ�Ͳ���ֵ֮���Ȩ��
    float Q;       // ���������ķ����ӳϵͳģ�͵Ĳ�ȷ����
    float R;       // ���������ķ����ӳ�������̵Ĳ�ȷ����
} KalmanFilter_Struct;

extern KalmanFilter_Struct kfs[3];


int16_t Com_Filter_LowPass(int16_t newData, int16_t lastData);
double  Common_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input);
#endif

