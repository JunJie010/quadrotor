#include "Inf_MPU6050.h"

#define abs(x) ((x) >= 0 ? (x) : -(x))

static GyroAccel_Struct offsetGyroAccel;

void Inf_MPU6050_Calibrate(void);
/**
 * @description: ��ָ���ļĴ���д��һ���ֽ�
 * @param {uint8_t} reg
 * @param {uint8_t} byte
 * @return {*}
 */
void Inf_MPU6050_WriteReg(uint8_t reg, uint8_t byte)
{
    HAL_I2C_Mem_Write(&hi2c1,
                      MPU6050_ADDR_W,
                      reg,
                      I2C_MEMADD_SIZE_8BIT,
                      &byte,
                      1,
                      2000);
}

/**
 * @description: ��ָ���ļĴ���startRegд�����ֽ�
 * @param {uint8_t} startReg ��ʼ�Ĵ�����ַ
 * @param {uint8_t} *bytes д���ֽ�����
 * @param {uint8_t} len ���ٸ��ֽ�
 * @return {*}
 */
void Inf_MPU6050_WriteRegs(uint8_t startReg, uint8_t *bytes, uint8_t len)
{
    HAL_I2C_Mem_Write(&hi2c1,
                      MPU6050_ADDR_W,
                      startReg,
                      I2C_MEMADD_SIZE_8BIT,
                      bytes,
                      len,
                      2000);
}

/**
 * @description: ��ָ���ļĴ�����ȡһ���ֽ�
 * @param {uint8_t} reg
 * @return {*}
 */
uint8_t Inf_MPU6050_ReadReg(uint8_t reg)
{
    uint8_t rByte = 0;
    HAL_I2C_Mem_Read(&hi2c1,
                     MPU6050_ADDR_R,
                     reg,
                     I2C_MEMADD_SIZE_8BIT,
                     &rByte,
                     1,
                     2000);
    return rByte;
}

/**
 * @description: ��ָ���ļĴ�����ȡ����ֽ�
 * @param {uint8_t} startReg
 * @param {uint8_t} *bytes
 * @param {uint8_t} len
 * @return {*}
 */
void Inf_MPU6050_ReadRegs(uint8_t startReg, uint8_t *bytes, uint8_t len)
{
    HAL_I2C_Mem_Read(&hi2c1,
                     MPU6050_ADDR_R,
                     startReg,
                     I2C_MEMADD_SIZE_8BIT,
                     bytes,
                     len,
                     2000);
}

/**
 * @description: mpu6050��ʼ������
 * @return {*}
 */
void Inf_MPU6050_Init(void)
{
    /* 1. ��λMPU6050:   ��λ->����200ms->���� */
    Inf_MPU6050_WriteReg(MPU_PWR_MGMT1_REG, 1 << 7);
    HAL_Delay(200);
    Inf_MPU6050_WriteReg(MPU_PWR_MGMT1_REG, 0);
    /* 2. �������� */
    /* 2.1 ���ٶȵ�����  +-2000��/s */
    Inf_MPU6050_WriteReg(MPU_GYRO_CFG_REG, 3 << 3);
    /* 2.2 ���ٶȵ�����  +2g  */
    Inf_MPU6050_WriteReg(MPU_ACCEL_CFG_REG, 0);
    /* 3. �ر��жϡ��ر� ��2 IIC �ӿڡ���ֹ FIFO */
    Inf_MPU6050_WriteReg(MPU_INT_EN_REG, 0);
    Inf_MPU6050_WriteReg(MPU_USER_CTRL_REG, 0);
    Inf_MPU6050_WriteReg(MPU_FIFO_EN_REG, 0);

    /* 4. ���������ǲ����ʺ͵�ͨ�˲���
        ��ͨ�˲���:  �����������ʱ 1kHz

        ������: �������Ƕ��ٵ�ʱ��,������ȷ�ķ�Ӧģ���ź�
            ��Ũ����(�ο�˹�ض���): ������ >= 2 * �ź�����ߵ�Ƶ��

            ����Ƶ�� = ���������Ƶ�� / (1 + SMPLRT_DIV)
            500Hz = 1000Hz / (1 + x)
            x = 1
     */

    Inf_MPU6050_WriteReg(MPU_SAMPLE_RATE_REG, 1); /*  ������ */
    Inf_MPU6050_WriteReg(MPU_CFG_REG, 1 << 0);    /* ��ͨ�˲��� */

    /* 5. ����ϵͳʱ��Դ */
    Inf_MPU6050_WriteReg(MPU_PWR_MGMT1_REG, 1);

    /* 6. ʹ�ܽ��ٶȴ������ͼ��ٶȴ�����   ���빤��ģʽ (���ô���ģʽ)*/
    Inf_MPU6050_WriteReg(MPU_PWR_MGMT2_REG, 0);

    /* У׼6������ */
    Inf_MPU6050_Calibrate();
}

/**
 * @description: ��ȡ���ٶ�
 * @param {Gyro_Struct} *gyro
 * @return {*}
 */
void Inf_MPU6050_ReadGyro(Gyro_Struct *gyro)
{
    uint8_t data[6] = {0};
    Inf_MPU6050_ReadRegs(MPU_GYRO_XOUTH_REG, data, 6);

    gyro->gyroX = (int16_t)((data[0] << 8) | data[1]);
    gyro->gyroY = (int16_t)((data[2] << 8) | data[3]);
    gyro->gyroZ = (int16_t)((data[4] << 8) | data[5]);
}

/**
 * @description: ��ȡ���ٶ�
 * @param {Accel_Struct} *accel
 * @return {*}
 */
void Inf_MPU6050_ReadAccel(Accel_Struct *accel)
{
    uint8_t data[6] = {0};
    Inf_MPU6050_ReadRegs(MPU_ACCEL_XOUTH_REG, data, 6);
    accel->accelX = (int16_t)((data[0] << 8) | data[1]);
    accel->accelY = (int16_t)((data[2] << 8) | data[3]);
    accel->accelZ = (int16_t)((data[4] << 8) | data[5]);
}

/**
 * @description: ��ȡ���ٶȺͼ��ٶ�
 * @param {GyroAccel_Struct} *gyroAccel
 * @return {*}
 */
void Inf_MPU6050_ReadGyroAccel(GyroAccel_Struct *gyroAccel)
{
    Inf_MPU6050_ReadGyro(&gyroAccel->gyro);
    Inf_MPU6050_ReadAccel(&gyroAccel->accel);
}

void Inf_MPU6050_ReadGyroAccelCalibrated(GyroAccel_Struct *gyroAccel)
{
    /* ��ȡԭʼ���� */
    Inf_MPU6050_ReadGyroAccel(gyroAccel);

    /* ��ȥУ׼ֵ */
    gyroAccel->gyro.gyroX -= offsetGyroAccel.gyro.gyroX;
    gyroAccel->gyro.gyroY -= offsetGyroAccel.gyro.gyroY;
    gyroAccel->gyro.gyroZ -= offsetGyroAccel.gyro.gyroZ;
    gyroAccel->accel.accelX -= offsetGyroAccel.accel.accelX;
    gyroAccel->accel.accelY -= offsetGyroAccel.accel.accelY;
    gyroAccel->accel.accelZ -= offsetGyroAccel.accel.accelZ;
}

/**
 * @description: ��ˮƽƽ�澲ֹ״̬��У׼6������
 *  1.ȷ�Ϸɻ����ھ�ֹ
 *      ��β���3������(���ٶ�),�������ϴζԱ�,����仯С��ĳ����ֵ,����Ϊ�ɻ����ھ�ֹ״̬
 *  2.��β�������ƫ��
 *
 *
 * @return {*}
 */
void Inf_MPU6050_Calibrate(void)
{
    /* 1.ȷ�Ͼ�ֹ  */
    uint8_t          cnt = 30;
    GyroAccel_Struct current;
    GyroAccel_Struct last;
    debug_printfln(" ������: �ȴ���ֹ");
    while(cnt)
    {
        Inf_MPU6050_ReadGyroAccel(&current);
        if(abs(current.gyro.gyroX - last.gyro.gyroX) <= 10 &&
           abs(current.gyro.gyroY - last.gyro.gyroY) <= 10 &&
           abs(current.gyro.gyroZ - last.gyro.gyroZ) <= 10)
        {
            cnt--;
        }

        last = current; /* �ϴεĽṹ��ִ����� */
        HAL_Delay(3);
    }
    debug_printfln(" ������: �Ѿ���ֹ");

    debug_printfln(" ������: ��ʼУ׼");
    int32_t  sumBuff[6] = {0};
    uint16_t sumCount   = 255;
    for(uint16_t i = 0; i < sumCount; i++)
    {
        Inf_MPU6050_ReadGyroAccel(&gyroAccel);
        sumBuff[0] += gyroAccel.gyro.gyroX - 0;
        sumBuff[1] += gyroAccel.gyro.gyroY - 0;
        sumBuff[2] += gyroAccel.gyro.gyroZ - 0;
        sumBuff[3] += gyroAccel.accel.accelX - 0;
        sumBuff[4] += gyroAccel.accel.accelY - 0;
        sumBuff[5] += gyroAccel.accel.accelZ - 16383;

        HAL_Delay(3);
    }

    offsetGyroAccel.gyro.gyroX   = sumBuff[0] / sumCount;
    offsetGyroAccel.gyro.gyroY   = sumBuff[1] / sumCount;
    offsetGyroAccel.gyro.gyroZ   = sumBuff[2] / sumCount;
    offsetGyroAccel.accel.accelX = sumBuff[3] / sumCount;
    offsetGyroAccel.accel.accelY = sumBuff[4] / sumCount;
    offsetGyroAccel.accel.accelZ = sumBuff[5] / sumCount;

    // Com_Config_PrintGyroAccel("cal", &offsetGyroAccel);
    debug_printfln(" ������: ����У׼");
}
