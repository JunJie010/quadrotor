#include "Int_LED.h"

LED_Handle_t led_left_top = {.gpiox = LED1_GPIO_Port, .gpio_pin = LED1_Pin};
LED_Handle_t led_left_bottom = {.gpiox = LED4_GPIO_Port, .gpio_pin = LED4_Pin};
LED_Handle_t led_right_top = {.gpiox = LED2_GPIO_Port, .gpio_pin = LED2_Pin};
LED_Handle_t led_right_bottom = {.gpiox = LED3_GPIO_Port, .gpio_pin = LED3_Pin};

void Int_LED_On(LED_Handle_t *led_handle)
{
    HAL_GPIO_WritePin(led_handle->gpiox, led_handle->gpio_pin, GPIO_PIN_RESET);
}

void Int_LED_Off(LED_Handle_t *led_handle)
{
    HAL_GPIO_WritePin(led_handle->gpiox, led_handle->gpio_pin, GPIO_PIN_SET);
}

// 实现LED的翻转
void Int_LED_Toggle(LED_Handle_t *led_handle)
{
    HAL_GPIO_TogglePin(led_handle->gpiox, led_handle->gpio_pin);
}
