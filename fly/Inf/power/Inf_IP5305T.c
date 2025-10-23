#include "Inf_IP5305T.h"

static void Inf_IP5305T_ShortPress(void)
{
    /* 短按:  拉低-> 延迟100ms -> 拉高 */
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);

    vTaskDelay(100);

    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
}
/**
 * @description: 芯片开机功能
 * @return {*}
 */
void Inf_IP5305T_Open(void)
{
    Inf_IP5305T_ShortPress();
}

/**
 * @description: 芯片的关机
 * @return {*}
 */
void Inf_IP5305T_Close(void)
{
    /* 两次短按 */
    Inf_IP5305T_ShortPress();
    vTaskDelay(500);
    Inf_IP5305T_ShortPress();
}

