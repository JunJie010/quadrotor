#include "Com_PID.h"

/**
 * @description: ����pid
 * @param {PID_Struct} *pid
 * @return {*}
 */
void Com_PID_ComputePID(PID_Struct *pid)
{
    /* 0. ������� */
    float error = pid->measure - pid->desire;
    /* 1. ������ */
    float pV = pid->kp * error;

    /* 2. ������ */
    pid->integral += pid->ki * error * pid->dt;

    /* 3. ΢���� */
    float dV       = pid->kd * (error - pid->lastError) / pid->dt;
    pid->lastError = error;

    /* 4. ������ */
    pid->result = pV + pid->integral + dV;
}

/**
 * @description: ����pid
 * @param {PID_Struct} *out �⻷
 * @param {PID_Struct} *in �ڻ�
 * @return {*}
 */
void Com_PID_CascadePID(PID_Struct *out, PID_Struct *in)
{
    /* 1. �����⻷ */
    Com_PID_ComputePID(out);
    /* 2. ���⻷�������Ϊ�ڻ�������ֵ */
    in->desire = out->result;
    /* 3. �����ڻ� */
    Com_PID_ComputePID(in);
}

