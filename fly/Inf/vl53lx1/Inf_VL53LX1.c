#include "Inf_VL53LX1.h"

#define DEV 0x52
/**
 * @description: ��ʼ��������оƬ
 * @return {*}
 */
void Inf_VL53LX1_Init(void)
{
    /* 1. ������оƬ */
    HAL_GPIO_WritePin(VL53LX1_SHUT_GPIO_Port, VL53LX1_SHUT_Pin, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(VL53LX1_SHUT_GPIO_Port, VL53LX1_SHUT_Pin, GPIO_PIN_SET);

    /* 2. ��ʼ����оƬ */
    VL53L1X_SensorInit(DEV);

    /* 3. ���þ���ģʽ: �����  1:short 2:long*/
    VL53L1X_SetDistanceMode(DEV, 2);

    /* 4. �������ٶ� */
    VL53L1X_SetTimingBudgetInMs(DEV, 20);

    /* 5. ������Ƶ�� msֵ������ڵ�����һ��*/
    VL53L1X_SetInterMeasurementInMs(DEV, 20);

    /* 6. ��ʼ���� */
    VL53L1X_StartRanging(DEV);

    uint16_t sensorID;
    VL53L1X_GetSensorId(DEV, &sensorID);
    printf("sensorID:0x%x\r\n", sensorID);
}

/**
 * @description: ���ز⵽�ĸ߶�
 * @return {*}
 */
uint16_t Inf_VL53LX1_GetHeight(void)
{
    static uint16_t height = 0;
    uint8_t         isDataReady;

    /* ������Ƿ���� */
    VL53L1X_CheckForDataReady(DEV, &isDataReady);
    if(isDataReady)
    {
        VL53L1X_ClearInterrupt(DEV);
        /* ��ȡ����� */
        VL53L1X_GetDistance(DEV, &height);
    }

    return height;
}

