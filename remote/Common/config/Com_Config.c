#include "Com_Config.h"

/* ҡ�����ݽṹ�� */
JoyStick_Struct joyStick;

/* �洢ҡ��ƫ�����Ľṹ�� */
JoyStick_Struct joyStickBias;

void Com_Config_PrintJoyStick(uint8_t *pre)
{
    printf("%s thr = %d, pit = %d, rol = %d, yaw = %d\r\n",
           pre,
           joyStick.THR,
           joyStick.PIT,
           joyStick.ROL,
           joyStick.YAW);
}

void Com_Config_PrintJoyStickBias(uint8_t *pre)
{
    printf("%s thr = %d, pit = %d, rol = %d, yaw = %d\r\n",
           pre,
           joyStickBias.THR,
           joyStickBias.PIT,
           joyStickBias.ROL,
           joyStickBias.YAW);
}

