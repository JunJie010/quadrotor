#include "App_Flight.h"

/* 6����̬pid */
/* ���� */
PID_Struct pitchPID = {.kp = -7.0f, .ki = 0.0f, .kd = 0.0f};
PID_Struct gyroYPID = {.kp = 2.0f, .ki = 0.0f, .kd = 0.1f};

/* ��� */
PID_Struct rollPID  = {.kp = -7.0f, .ki = 0.0f, .kd = 0.0f};
PID_Struct gyroXPID = {.kp = -2.0f, .ki = 0.0f, .kd = -0.1f};

/* ƫ�� */
PID_Struct yawPID   = {.kp = -2.2f, .ki = 0.0f, .kd = 0.0f};
PID_Struct gyroZPID = {.kp = -1.5f, .ki = 0.0f, .kd = 0.0f};

/* 2������pid */
/* �߶� */
PID_Struct heightPID = {.kp = -1.4f, .ki = 0.0f, .kd = 0.0f};
PID_Struct zSpeedPID = {.kp = -1.3f, .ki = 0.0f, .kd = -0.08f};
/**
 * @description: ����ģ�������
 * @return {*}
 */
void App_Flight_Start(void)
{
    /* 1. ��ʼ��motor */
    debug_printfln("��ʼ����� ��ʼ");
    Inf_Motor_Init();
    debug_printfln("��ʼ����� ����");

    /* 2. ��ʼ��MPU6050 */
    debug_printfln("��ʼ��MPU6050 ��ʼ");
    Inf_MPU6050_Init();
    debug_printfln("��ʼ��MPU6050 ����");

    /* 3. ��ʼ��VL53L1X */
    debug_printfln("��ʼ�������� ��ʼ");
    Inf_VL53LX1_Init();
    debug_printfln("��ʼ�������� ����");
}

/**
 * @description: �������������˲�
 *  ���ٶ�: ʹ��һ�׵�ͨ�˲�
 *      ����С, ���Ӱ��С
 *  ���ٶ�: �������˲�
 *      ������, �����յ�����
 * @param {GyroAccel_Struct} *gyroAccel
 * @return {*}
 */
void App_Flight_GetGyroAccelWithFilter(GyroAccel_Struct *gyroAccel)
{
    /* 1. ��ȡԭʼ���� */
    taskENTER_CRITICAL();
    Inf_MPU6050_ReadGyroAccelCalibrated(gyroAccel);
    taskEXIT_CRITICAL();

    /* 2. �Խ��ٶ���һ�׵�ͨ��ͨ�˲� */
    static int16_t lastDatas[3] = {0};
    gyroAccel->gyro.gyroX       = Com_Filter_LowPass(gyroAccel->gyro.gyroX, lastDatas[0]);
    gyroAccel->gyro.gyroY       = Com_Filter_LowPass(gyroAccel->gyro.gyroY, lastDatas[1]);
    gyroAccel->gyro.gyroZ       = Com_Filter_LowPass(gyroAccel->gyro.gyroZ, lastDatas[2]);
    lastDatas[0]                = gyroAccel->gyro.gyroX;
    lastDatas[1]                = gyroAccel->gyro.gyroY;
    lastDatas[2]                = gyroAccel->gyro.gyroZ;

    /* 3. �Լ��ٶ����������˲� */
    // OutData[0] = gyroAccel->accel.accelX ;
    gyroAccel->accel.accelX = Common_Filter_KalmanFilter(&kfs[0], gyroAccel->accel.accelX);
    gyroAccel->accel.accelY = Common_Filter_KalmanFilter(&kfs[1], gyroAccel->accel.accelY);
    gyroAccel->accel.accelZ = Common_Filter_KalmanFilter(&kfs[2], gyroAccel->accel.accelZ);
    // OutData[1] = gyroAccel->accel.accelX ;
    // OutPut_Data();
}

/**
 * @description: ��ȡŷ����
 * @return {*}
 */
void App_Flight_GetEulerAngle(GyroAccel_Struct  *gyroAccel,
                              EulerAngle_Struct *eulerAngle,
                              float              dt)
{
    Common_IMU_GetEulerAngle(gyroAccel, eulerAngle, dt);
}

/**
 * @description: ������̬pid
 * @param {GyroAccel_Struct} *gyroAccel
 * @param {EulerAngle_Struct} *eulerAngle
 * @param {float} dt ��������
 * @return {*}
 */
void App_Flight_PIDPosture(GyroAccel_Struct *gyroAccel, EulerAngle_Struct *eulerAngle, float dt)
{
    /* ���� */
    pitchPID.dt      = dt;
    pitchPID.desire  = (joyStick.PIT - 500) * 0.04; /* ���Ʒ��� */
    pitchPID.measure = eulerAngle->pitch;

    gyroYPID.dt      = dt;
    gyroYPID.measure = gyroAccel->gyro.gyroY * Gyro_G;

    Com_PID_CascadePID(&pitchPID, &gyroYPID);

    /* ��� */
    rollPID.dt      = dt;
    rollPID.desire  = (joyStick.ROL - 500) * 0.04;
    rollPID.measure = eulerAngle->roll;

    gyroXPID.dt      = dt;
    gyroXPID.measure = gyroAccel->gyro.gyroX * Gyro_G;

    Com_PID_CascadePID(&rollPID, &gyroXPID);

    /* ƫ�� */
    yawPID.dt      = dt;
    yawPID.desire  = (joyStick.YAW - 500) * 0.04;
    yawPID.measure = eulerAngle->yaw;

    gyroZPID.dt      = dt;
    gyroZPID.measure = gyroAccel->gyro.gyroZ * Gyro_G;

    Com_PID_CascadePID(&yawPID, &gyroZPID);
}

/**
 * @description: ����̬PID���õ������
 * @param {Com_Status} isRemoteUnlock
 * @return {*}
 */
void App_Flight_MotorWithPosturePID(Com_Status isRemoteUnlock)
{
    if(isRemoteUnlock != Com_OK) return;

    /*
        leftTop leftBottom rightTop rightBottom

        ���:  gyroXPID.result
            leftTop + leftBottom  vs   rightTop + rightBottom

        ����: gyroYPID.result
            leftTop + rightTop    vs    leftBottom + rightBottom

        ƫ��: gyroZPID.result
            leftTop+rightBottom   vs    leftBottom + rightTop

    */
    int16_t speed          = joyStick.THR * 0.7;
    motorLeftTop.speed     = speed + gyroXPID.result + gyroYPID.result + gyroZPID.result;
    motorLeftBottom.speed  = speed + gyroXPID.result - gyroYPID.result - gyroZPID.result;
    motorRightTop.speed    = speed - gyroXPID.result + gyroYPID.result - gyroZPID.result;
    motorRightBottom.speed = speed - gyroXPID.result - gyroYPID.result + gyroZPID.result;
}

/**
 * @description: ��ȡ�ɻ��ķ��и߶�
 * @return {*} �߶�: mm
 */
uint16_t App_Flight_GetHeight(void)
{
    static uint16_t lastHeight = 0;
    uint16_t        height     = Inf_VL53LX1_GetHeight();

    if(abs(height - lastHeight) > 500 || /* �����ͻ��,�򷵻��ϴε�ֵ */
       abs(joyStick.PIT - 500) > 100 ||  /* ��ˮƽ����, �����ϴε�ֵ */
       abs(joyStick.ROL - 500) > 100)
    {
        return lastHeight;
    }

    height     = Com_Filter_LowPass(height, lastHeight);
    lastHeight = height;

    return height;
}

/**
 * @description: �߶�pid����
 * @param {Com_Status} isRemoteUnlocked
 * @param {uint16_t} height
 * @return {*}
 */
void App_Flight_PIDHeight(Com_Status isRemoteUnlocked, uint16_t height, float dt)
{
    /* ����״̬��:
        ״̬0: ����Ƿ񶨸�
        ״̬1: ��ǰ������ֵ�Ƕ���ʱ������ֵ  ��ǰ�ĸ߶�: �̶��ĸ߶�
        ״̬2: ����pid����
     */
    static uint8_t  status     = 0;
    static uint16_t thrHold    = 0;
    static uint16_t heightHold = 0;

    static float staticAcc = 0;  /* ��̬ʱz�ļ��ٶ� */
    if(isRemoteUnlocked == Com_OK && staticAcc == 0)
    {
        staticAcc = Common_IMU_GetNormAccZ();
    }

    switch(status)
    {
        case 0: /* ���߼�� */
        {
            /* pid���� */
            heightPID.result = 0;
            zSpeedPID.result = 0;
            if(isRemoteUnlocked == Com_OK && isFixHeight == Com_OK)
            {
                status = 1;
            }

            break;
        }

        case 1: /* pid����ǰ��׼�� */
        {
            thrHold    = joyStick.THR;
            heightHold = height;
            status     = 2;
            break;
        }
        case 2: /* pid���� */
        {
            /* ����ʱ: ���ű仯����100, ���߶��ߵı��Ϊ0. ������� */
            if(abs(joyStick.THR - thrHold) > 100 || isFixHeight == Com_FAIL)
            {
                status               = 0; /* �ص�״̬0 */
                joyStick.isFixHeight = 0; /* ��Ƕ��ߵı�����Ϊ0 */
                isFixHeight          = Com_FAIL;
            }
            else
            {

                /* ���ڸ߶ȱ䶯������20ms, ����������Ҫ5��������һ��pid */
                static uint8_t cnt = 0;
                cnt++;
                if(cnt < 5) return;
                cnt = 0;
                dt *= 5;
                /* ��z���ٶ�: �����˲� */
                float zSpeed = 0.9 * (zSpeedPID.measure + (Common_IMU_GetNormAccZ() - staticAcc) * dt) +
                               0.1 * (height - heightPID.measure) / dt;
                /*
                    ����pid
                        �⻷  �߶Ȼ�
                        �ڻ�  z������ٶȻ�
                 */
                heightPID.desire  = heightHold;
                heightPID.measure = height;
                heightPID.dt      = dt;

                zSpeedPID.measure = zSpeed;
                zSpeedPID.dt      = dt;
                Com_PID_CascadePID(&heightPID, &zSpeedPID);
            }
            break;
        }
        default:
            break;
    }
}

/**
 * @description: �Ѷ��ߵ�pid���õ�motor��
 * @param {Com_Status} isRemoteUnlocked
 * @return {*}
 */
void App_Flight_MotorWithHeightPID(Com_Status isRemoteUnlocked)
{
    int16_t zPid = LIMIT(zSpeedPID.result, -150, 150);
    motorLeftTop.speed += zPid;
    motorLeftBottom.speed += zPid;
    motorRightTop.speed += zPid;
    motorRightBottom.speed += zPid;
}

/**
 * @description: �÷ɻ�����(����)
 * @param {Com_Status} isRemoteUnlock
 * @return {*}
 */
void App_Flight_Work(Com_Status isRemoteUnlock)
{
    if(isRemoteUnlock != Com_OK || joyStick.THR <= 30)
    {
        motorLeftTop.speed =
            motorLeftBottom.speed =
                motorRightTop.speed =
                    motorRightBottom.speed = 0;
    }

    Inf_Motor_AllMotorsWork();
}

/*
ģ���ź��˲�
    ����  ���

�����ź��˲�
    �㷨
        1. ��ֵ�˲�(���������˲�)
        2. ��ֵ�˲�
        3. һ�׵�ͨ�˲�
             ��� = ϵ�� * �ϴε�ֵ + (1 - ϵ��) * ��ε�ֵ
             ���ٶ�

        4. �������˲�
            ����5����ʽ
                https://www.kalmanfilter.net/CN/default_cn.aspx

            ���ٶ�

        5. �����˲�
            ����:
                ���ٶȶ�ʱ�����    �ٶ�: ��ӦѸ��, ��������յ����Ӱ��
                λ�ƶ�ʱ���΢��    �ٶ�: ���������Ӱ��, ��Ӧ����ʱ

                0.3 * �ٶ�1 + 0.7 * �ٶ�2

        ...


��ε���pid:

    �ڻ� p
        ȷ������:
            ����: �����µ�
            ��ȷ: ����ƽ�����µ�
        ��С:
            �µ���ʱ��Ҫƽ��
    �⻷ p
        ȷ������:
            ��ȷ: �ص���ʼλ��
            ����: ��ת
        ��С:
            �۲�����Ŀ���. ���Ƿ�����, ����������
    �ڻ� d
        ������
        ȷ������:
            ����: �𵴼Ӿ�
            ��ȷ: �𵴼���
        ��С:
            ���ܿ��ٻص���ʼλ��,��û����
    �⻷ d(��ѡ)
        ����ڻ���d���Ʋ�����,�ٵ����⻷��d
    �ڻ� i (��ѡ)
    �⻷ i (��ѡ)



*/
