#ifndef __INT_JOYSTICK_H__
#define __INT_JOYSTICK_H__

#include "adc.h"
#include <stdio.h>
#include "Common_Types.h"

void Int_Joystick_Init(void);

void Int_Joystick_ReadData(RcData_t *rc_data);
#endif /* __INT_JOYSTICK_H__ */