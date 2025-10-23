#ifndef __COM_CONFIG_H
#define __COM_CONFIG_H

#include "stdint.h"
#include "stdio.h"

typedef enum
{
    Com_OK = 0,
    Com_TIMEOUT,
    Com_FAIL
} Com_Status;

#define LIMIT(x, min, max) (x) >= (max) ? (max) : ((x) <= (min) ? (min) : (x))

typedef enum
{
    KEY_NONE = 0,
    KEY_LEFT,
    KEY_UP,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT_TOP,
    KEY_RIGHT_TOP,
    KEY_LEFT_TOP_LONG,
    KEY_RIGHT_TOP_LONG
} Com_Key;

typedef struct
{
    int16_t THR; /* 油门 */
    int16_t PIT; /* 俯仰 */
    int16_t ROL; /* 横滚 */
    int16_t YAW; /* 偏航 */

    uint8_t isPowerDown; /* 是否关机: 1:关机 0:不关机 */
    uint8_t isFixHeight; /* 是否翻转定高的状态 */
} JoyStick_Struct;

#define FRAME_0 (0x11)
#define FRAME_1 (0x22)
#define FRAME_2 (0x33)

extern JoyStick_Struct joyStick;
extern JoyStick_Struct joyStickBias;

void Com_Config_PrintJoyStick(uint8_t *pre);
void Com_Config_PrintJoyStickBias(uint8_t *pre);
#endif
