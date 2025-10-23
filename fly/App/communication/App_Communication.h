#ifndef __APP_COMMUNICATION_H
#define __APP_COMMUNICATION_H
#include "Com_Debug.h"
#include "Inf_Si24R1.h"
#include "Com_Config.h"
#include "FreeRTOS.h"
#include "task.h"
void App_Communication_Start(void);
Com_Status App_Communication_ReceiveJoyStickData(void);
Com_Status App_Communication_CheckConnection(Com_Status isReceiveData);
Com_Status App_Communication_RemoteUnlock(Com_Status isRemoteConnected);
#endif
