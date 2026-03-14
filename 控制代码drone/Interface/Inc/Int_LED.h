#ifndef __INT_LED_H__
#define __INT_LED_H__
#include "main.h"

typedef struct
{
    GPIO_TypeDef *gpiox;
    uint16_t gpio_pin;
} LED_Handle_t;

extern LED_Handle_t led_left_top;
extern LED_Handle_t led_left_bottom;
extern LED_Handle_t led_right_top;
extern LED_Handle_t led_right_bottom;

void Int_LED_On(LED_Handle_t *led_handle);
void Int_LED_Off(LED_Handle_t *led_handle);
void Int_LED_Toggle(LED_Handle_t *led_handle);

#endif /* __INT_LED_H__ */