#include "Int_Key.h"

KEY_TYPE_e Int_KeyScan(void)
{
    // ============================KEY_UP=================================
    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_SET)
                {
                    return KEY_UP;
                }
            }
        }
    }

    // ============================KEY_DOWN=================================
    if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_SET)
                {
                    return KEY_DOWN;
                }
            }
        }
    }

    // ============================KEY_LEFT=================================
    if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_SET)
                {
                    return KEY_LEFT;
                }
            }
        }
    }

    // ============================KEY_RIGHT=================================
    if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_SET)
                {
                    return KEY_RIGHT;
                }
            }
        }
    }

    // ============================KEY_LEFT_X=================================
    if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
    {
        uint32_t start_tick = xTaskGetTickCount();
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_SET)
                {
                    uint32_t end_tick = xTaskGetTickCount();
                    if (end_tick - start_tick >= 1000)
                    {
                        return KEY_LEFT_X_LONG;
                    }
                    else
                    {
                        return KEY_LEFT_X;
                    }
                }
            }
        }
    }

    // ============================KEY_RIGHT_X=================================
    if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
    {
        uint32_t start_tick = xTaskGetTickCount();
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_SET)
                {
                    uint32_t end_tick = xTaskGetTickCount();
                    if (end_tick - start_tick >= 1000)
                    {
                        return KEY_RIGHT_X_LONG;
                    }
                    else
                    {
                        return KEY_RIGHT_X;
                    }
                }
            }
        }
    }
    return KEY_NONE;
}
