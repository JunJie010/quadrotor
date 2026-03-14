#ifndef __INT_KEY_H__
#define __INT_KEY_H__

#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

typedef enum
{
    KEY_LEFT_X,
    KEY_RIGHT_X,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_RIGHT_X_LONG,
    KEY_LEFT_X_LONG,
    KEY_NONE
} KEY_TYPE_e;

KEY_TYPE_e Int_KeyScan(void);

#endif /* __INT_KEY_H__ */