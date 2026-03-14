#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <stdint.h>

typedef struct
{
    int16_t throttle;    // 油门
    int16_t yaw;         // 偏航
    int16_t roll;        // 横滚
    int16_t pitch;       // 俯仰
    uint8_t off;         // 关机信号位
    uint8_t hold_height; // 定高信号位
} RcData_t;

#endif /* __COMMON_TYPES_H__ */