#include "Inf_IP5305T.h"

static void Inf_IP5305T_ShortPress(void)
{
    /* �̰�:  ����-> �ӳ�100ms -> ���� */
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);

    vTaskDelay(100);

    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
}
/**
 * @description: оƬ��������
 * @return {*}
 */
void Inf_IP5305T_Open(void)
{
    Inf_IP5305T_ShortPress();
}

/**
 * @description: оƬ�Ĺػ�
 * @return {*}
 */
void Inf_IP5305T_Close(void)
{
    /* ���ζ̰� */
    Inf_IP5305T_ShortPress();
    vTaskDelay(500);
    Inf_IP5305T_ShortPress();
}

