#include "App_Task.h"

/* 0. �������� */
void startTask(void *args);
#define START_TASK_NAME "start_task"
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 10
TaskHandle_t startTaskHandle;

/* 1. ��Դ�������� */
void powerTask(void *args);
#define POWER_TASK_NAME "power_task"
#define POWER_TASK_STACK 128
#define POWER_TASK_PRIORITY 9
TaskHandle_t powerTaskHandle;
#define POWER_EXEC_CYCLE (10 * 1000)

/* 2. �ƿ����� */
void ledTask(void *args);
#define LED_TASK_NAME "led_task"
#define LED_TASK_STACK 128
#define LED_TASK_PRIORITY 2
TaskHandle_t ledTaskHandle;
#define LED_EXEC_CYCLE 50

/* 3. �ɿ����� */
void flightTask(void *args);
#define FLIGHT_TASK_NAME "flight_task"
#define FLIGHT_TASK_STACK 256
#define FLIGHT_TASK_PRIORITY 8
TaskHandle_t flightTaskHandle;
#define FLIGHT_EXEC_CYCLE 4

/* 4. ͨѶ���� */
void communicationTask(void *args);
#define COMMUNICATION_TASK_NAME "communication_task"
#define COMMUNICATION_TASK_STACK 256
#define COMMUNICATION_TASK_PRIORITY 8
TaskHandle_t communicationTaskHandle;
#define COMMUNICATION_EXEC_CYCLE 6

/**
 * @description: ����ʵʱϵͳ
 * @return {*}
 */
void App_Task_FreeRTOSStart(void)
{
    /* 1. ��ʼ��debugģ�� */
    debug_start();

    debug_printfln("�й�����˻���Ŀ--�ɿ�");

    /* 2. ��������ģ�� */
    App_Flight_Start();

    /* 3. ����ͨѶģ�� */
    App_Communication_Start();

    /* 1. ����һ����������: ��������ִ�е�ʱ��, ������ҵ����ص����� */
    BaseType_t r = xTaskCreate(startTask,        /* ���������� */
                               START_TASK_NAME,  /* ������ */
                               START_TASK_STACK, /* ջ��С: ��λ����(�����Լ�) �ֵĴ�С: 32λ�ܹ���4  START_TASK_STACK * 4 = �ֽ���  */
                               NULL,             /* ָ�������������ݲ��� */
                               START_TASK_PRIORITY,
                               &startTaskHandle);
    if(r == pdPASS)
    {
        debug_printfln("�������񴴽��ɹ�...");
    }
    else
    {
        debug_printfln("�������񴴽�ʧ��...");
    }

    /* 2. ���������� */
    vTaskStartScheduler();
}

/* ���������� */
void startTask(void *args)
{
    debug_printfln("��������ʼ����");

    /* 1.������Դ�������� */
    xTaskCreate(powerTask,
                POWER_TASK_NAME,
                POWER_TASK_STACK,
                NULL,
                POWER_TASK_PRIORITY,
                &powerTaskHandle);
    /* 2. �����ƿ����� */
    xTaskCreate(ledTask,
                LED_TASK_NAME,
                LED_TASK_STACK,
                NULL,
                LED_TASK_PRIORITY,
                &ledTaskHandle);

    /* 3. �����ɿ����� */
    xTaskCreate(flightTask,
                FLIGHT_TASK_NAME,
                FLIGHT_TASK_STACK,
                NULL,
                FLIGHT_TASK_PRIORITY,
                &flightTaskHandle);

    /* 4. ����ͨѶ���� */
    xTaskCreate(communicationTask,
                COMMUNICATION_TASK_NAME,
                COMMUNICATION_TASK_STACK,
                NULL,
                COMMUNICATION_TASK_PRIORITY,
                &communicationTaskHandle);

    /* ɾ���Լ� */
    vTaskDelete(NULL);
    debug_printfln("����ִ��");
}

/* ��Դ���� */
void powerTask(void *args)
{
    debug_printfln("��Դ����ʼ����");

    uint32_t preTime = xTaskGetTickCount();
    while(1)
    {

        if(ulTaskNotifyTake(pdTRUE, POWER_EXEC_CYCLE))
        {
            /*�� POWER_EXEC_CYCLE �ڵȵ��˹ػ����� */
            debug_printfln("�ػ�");
            Inf_IP5305T_Close();
        }
        else
        {
            debug_printfln("��Դ...");
            Inf_IP5305T_Open(); /* ��ֹ�Զ��ػ� */
        }
    }
}

/* 2. �ƿ����� */
void ledTask(void *args)
{
    debug_printfln("�ƿ�����ʼ����...");

    // isRemoteConnected = Com_OK;
    // isRemoteUnlocked = Com_OK;
    // isFixHeight = Com_OK;

    uint32_t preTime = xTaskGetTickCount();
    uint32_t cnt     = 0;

    static uint8_t isFirstUnlock = 1; /* �Ƿ�ʱ�״ν������ */
    while(1)
    {
        /* ����->����->���� */
        if(isFixHeight == Com_OK)
        {
            ledLeftTop.status =
                ledLeftBootom.status =
                    ledRightTop.status =
                        ledRightBottom.status = 2;
        }
        else if(isRemoteUnlocked == Com_OK)
        {
            if(isFirstUnlock == 1)
            {
                ledLeftTop.status =
                    ledLeftBootom.status =
                        ledRightTop.status =
                            ledRightBottom.status = 1;

                isFirstUnlock = 0;
            }
            else
            {
                ledLeftTop.status =
                    ledLeftBootom.status =
                        ledRightTop.status =
                            ledRightBottom.status = 10;
            }
        }
        else if(isRemoteConnected == Com_OK)
        {
            isFirstUnlock = 1;

            ledLeftTop.status =
                ledRightTop.status = 1;
            ledLeftBootom.status =
                ledRightBottom.status = 15;
        }
        else
        {
            ledLeftTop.status =
                ledRightTop.status = 15;
            ledLeftBootom.status =
                ledRightBottom.status = 1;
        }

        cnt++;

        if(ledLeftTop.status <= 1 || cnt % ledLeftTop.status == 0)
        {
            Inf_LED_SetStatus(&ledLeftTop);
        }

        if(ledLeftBootom.status <= 1 || cnt % ledLeftBootom.status == 0)
        {
            Inf_LED_SetStatus(&ledLeftBootom);
        }

        if(ledRightTop.status <= 1 || cnt % ledRightTop.status == 0)
        {
            Inf_LED_SetStatus(&ledRightTop);
        }

        if(ledRightBottom.status <= 1 || cnt % ledRightBottom.status == 0)
        {
            Inf_LED_SetStatus(&ledRightBottom);
        }
        vTaskDelayUntil(&preTime, LED_EXEC_CYCLE);
    }
}

/* 3. �ɿ����� */
void flightTask(void *args)
{
    debug_printfln("�ɿ�����ʼ����");
    uint32_t preTime = xTaskGetTickCount();

    float dt = FLIGHT_EXEC_CYCLE / 1000.0; /* �������� */
    while(1)
    {

        /* 1. ��6���������˲� */
        App_Flight_GetGyroAccelWithFilter(&gyroAccel);

        /* 2. ��ȡŷ���� */
        App_Flight_GetEulerAngle(&gyroAccel,
                                 &eulerAngle,
                                 dt);

        /* 3. ����6��pid(3������pid) */
        App_Flight_PIDPosture(&gyroAccel, &eulerAngle, dt);

        /* 4. ����̬pid���õ������ */
        App_Flight_MotorWithPosturePID(isRemoteUnlocked);

        /* 5. ��ȡ�߶� */
        uint16_t height = App_Flight_GetHeight();

        /* 7. ���㶨��pid */
        App_Flight_PIDHeight(isRemoteUnlocked, height, dt);

        /* 8.�Ѹ߶�pid���õ������ */
        App_Flight_MotorWithHeightPID(isRemoteUnlocked);
        
        /* ���Ƶ��ת�� */
        App_Flight_Work(isRemoteUnlocked);
        vTaskDelayUntil(&preTime, FLIGHT_EXEC_CYCLE);
    }
}

/* 4. ͨѶ���� */
void communicationTask(void *args)
{
    // vTaskDelay(1000);
    debug_printfln("ͨѶ����ʼ����");
    uint32_t preTime = xTaskGetTickCount();
    while(1)
    {
        /* 1. ��ȡ2.4g���� */
        Com_Status isReceiveData = App_Communication_ReceiveJoyStickData();

        /* 2. �ж�ң������������� */
        isRemoteConnected = App_Communication_CheckConnection(isReceiveData);
        // debug_printfln("%d", isRemoteConnected);

        /* 3. ң�������� */
        isRemoteUnlocked = App_Communication_RemoteUnlock(isRemoteConnected);

        /* Զ�̹ػ� */
        if(isReceiveData == Com_OK)
        {
            if(joyStick.isPowerDown)
            {
                xTaskNotifyGive(powerTaskHandle);
            }
        }
        vTaskDelayUntil(&preTime, COMMUNICATION_EXEC_CYCLE);
    }
}
