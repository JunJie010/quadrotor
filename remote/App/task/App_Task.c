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

/* 2. ͨѶ���� */
void communicationTask(void *args);
#define COMMUNICATION_TASK_NAME "communication_task"
#define COMMUNICATION_TASK_STACK 256
#define COMMUNICATION_TASK_PRIORITY 8
TaskHandle_t communicationTaskHandle;
#define COMMUNICATION_EXEC_CYCLE 6

/* 3. ������������ */
void keyTask(void *args);
#define KEY_TASK_NAME "key_task"
#define KEY_TASK_STACK 256
#define KEY_TASK_PRIORITY 7
TaskHandle_t keyTaskHandle;
#define KEY_EXEC_CYCLE 50

/* 4. ҡ�˴������� */
void joyStickTask(void *args);
#define JOY_STICK_TASK_NAME "joy_stick_task"
#define JOY_STICK_TASK_STACK 256
#define JOY_STICK_TASK_PRIORITY 7
TaskHandle_t joyStickTaskHandle;
#define JOY_STICK_EXEC_CYCLE 4

/* 5. ��ʾ���� */
void displayTask(void *args);
#define DISPLAY_TASK_NAME "display_task"
#define DISPLAY_TASK_STACK 256
#define DISPLAY_TASK_PRIORITY 7
TaskHandle_t displayTaskHandle;
#define DISPLAY_EXEC_CYCLE 6

/**
 * @description: ����ʵʱϵͳ
 * @return {*}
 */
void App_Task_FreeRTOSStart(void)
{
    /* 1. ��ʼ��debugģ�� */
    debug_start();

    debug_printfln("�й�����˻���Ŀ--ң��");

    /* 2. ����ͨѶģ�� */
    App_Communication_Start();

    /* 3. ����������ģ�� */
    App_DataProcess_Start();

    /* 4. ������ʾģ�� */
    App_Display_Start();

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

    /* 2. ����ͨѶ���� */
    xTaskCreate(communicationTask,
                COMMUNICATION_TASK_NAME,
                COMMUNICATION_TASK_STACK,
                NULL,
                COMMUNICATION_TASK_PRIORITY,
                &communicationTaskHandle);

    /* 3. ������������ */
    xTaskCreate(keyTask,
                KEY_TASK_NAME,
                KEY_TASK_STACK,
                NULL,
                KEY_TASK_PRIORITY,
                &keyTaskHandle);

    /* 4. ҡ�˴������� */
    xTaskCreate(joyStickTask,
                JOY_STICK_TASK_NAME,
                JOY_STICK_TASK_STACK,
                NULL,
                JOY_STICK_TASK_PRIORITY,
                &joyStickTaskHandle);

    /* 5. ��ʾ���� */
    xTaskCreate(displayTask,
                DISPLAY_TASK_NAME,
                DISPLAY_TASK_STACK,
                NULL,
                DISPLAY_TASK_PRIORITY,
                &displayTaskHandle);

    /* ɾ���Լ� */
    vTaskDelete(NULL);
    debug_printfln("����ִ��");
}

/*1 ��Դ���� */
void powerTask(void *args)
{
    debug_printfln("��Դ����ʼ����");

    uint32_t preTime = xTaskGetTickCount();
    while(1)
    {

        if(ulTaskNotifyTake(pdTRUE, POWER_EXEC_CYCLE))
        {
            /*�� POWER_EXEC_CYCLE �ڵȵ��˹ػ����� */
            //debug_printfln("�ػ�");
            Inf_IP5305T_Close();
        }
        else
        {
            //debug_printfln("��Դ...");
            Inf_IP5305T_Open(); /* ��ֹ�Զ��ػ� */
        }
    }
}

/* 2. ͨѶ���� */
void communicationTask(void *args)
{
    vTaskDelay(1000);
    debug_printfln("ͨѶ����ʼ����");
    uint32_t preTime = xTaskGetTickCount();

    while(1)
    {
        App_Communication_SendJoyStickData();
        vTaskDelayUntil(&preTime, COMMUNICATION_EXEC_CYCLE);
    }
}

/* 3. ����ɨ�� */
void keyTask(void *args)
{
    debug_printfln("����ɨ������ʼ����");
    uint32_t preTime = xTaskGetTickCount();
    while(1)
    {
        App_DataProcess_KeyDataProcess();

        vTaskDelayUntil(&preTime, KEY_EXEC_CYCLE);
    }
}

/* 4. ҡ�˴��� */
void joyStickTask(void *args)
{
    vTaskDelay(500);
    debug_printfln("ҡ�˴�������ʼ����");
    uint32_t preTime = xTaskGetTickCount();
    while(1)
    {
        App_DataProcess_JoyStickDataProcess();
        vTaskDelayUntil(&preTime, JOY_STICK_EXEC_CYCLE);
    }
}

/* 5. ��ʾ���� */
void displayTask(void *args)
{
    debug_printfln("��ʾ����ʼ����");
    uint32_t preTime = xTaskGetTickCount();
    while(1)
    {
        App_Display_Show();
        vTaskDelayUntil(&preTime, DISPLAY_EXEC_CYCLE);
    }
}
