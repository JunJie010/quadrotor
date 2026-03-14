
#include "RC_tasks.h"
#include <stdio.h> // 添加stdio.h头文件以使用printf

// 全局变量区域 Start
RcData_t rc_data = {0};

// 全局变量区域 End
//电源任务
void Power_Task(void *param);
#define Power_TASK_NAME "Power_Task"
#define Power_TASK_STACK_SIZE 128
#define Power_TASK_PRIORITY 4
TaskHandle_t Power_Task_Handle;

// key任务
void Key_Task(void *param);
#define Key_TASK_NAME "Key_Task"
#define Key_TASK_STACK_SIZE 128
#define Key_TASK_PRIORITY 2
TaskHandle_t Key_Task_Handle;

// 摇杆任务
void Joystick_Task(void *param);
#define Joystick_TASK_NAME "Joystick_Task"
#define Joystick_TASK_STACK_SIZE 128
#define Joystick_TASK_PRIORITY 2
TaskHandle_t Joystick_Task_Handle;

// 通讯任务
void Comm_Task(void *param);
#define Comm_TASK_NAME "Comm_Task"
#define Comm_TASK_STACK_SIZE 128
#define Comm_TASK_PRIORITY 3
TaskHandle_t Comm_Task_Handle;

// 显示任务
void Display_Task(void *param);
#define Display_TASK_NAME "Display_Task"
#define Display_TASK_STACK_SIZE 128
#define Display_TASK_PRIORITY 2
TaskHandle_t Display_Task_Handle;

void Start_all_tasks(void)
{
    // 摇杆初始化
    Int_Joystick_Init();
    //OLED初始化
    App_Display_Start();
    // 将2.4G收发芯片配置成发送模式
    Int_SI24R1_Init(TX_MODE);
    // 电源任务
    xTaskCreate(Power_Task, Power_TASK_NAME, Power_TASK_STACK_SIZE, NULL, Power_TASK_PRIORITY, &Power_Task_Handle);
    // key 任务
    xTaskCreate(Key_Task, Key_TASK_NAME, Key_TASK_STACK_SIZE, NULL, Key_TASK_PRIORITY, &Key_Task_Handle);
    // Joystick 任务
    xTaskCreate(Joystick_Task, Joystick_TASK_NAME, Joystick_TASK_STACK_SIZE, NULL, Joystick_TASK_PRIORITY, &Joystick_Task_Handle);
    // 通讯任务
    xTaskCreate(Comm_Task, Comm_TASK_NAME, Comm_TASK_STACK_SIZE, NULL, Comm_TASK_PRIORITY, &Comm_Task_Handle);
    //显示任务
    xTaskCreate(Display_Task, Display_TASK_NAME, Display_TASK_STACK_SIZE, NULL, Display_TASK_PRIORITY, &Display_Task_Handle);

    vTaskStartScheduler();
}

void Power_Task(void *param)
{
    printf("power task start");
    while (1)
    {
        // 再来一个10000ms的延时 意思是每隔10s钟 拉低一次
        vTaskDelay(10000);
        // 拉低
        HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
        // delay的时间按照手册来说 得在30ms以上
        vTaskDelay(100);
        // 再断开1
        HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);
    }
}

// 添加字符串映射数组，将枚举值与字符串描述对应
const char *key_names[] = {
    "KEY_LEFT_X",
    "KEY_RIGHT_X",
    "KEY_UP",
    "KEY_DOWN",
    "KEY_LEFT",
    "KEY_RIGHT",
    "KEY_RIGHT_X_LONG",
    "KEY_LEFT_X_LONG",
    "KEY_NONE"};
void Key_Task(void *param)
{

    while (1)
    {
        App_Data_ProcessKey(&rc_data);

        vTaskDelay(10);
    }
}

void Joystick_Task(void *param)
{
    while (1)
    {
        App_Data_ProcessJoystick(&rc_data);
        vTaskDelay(10);
    }
}

void Comm_Task(void *param)
{
    uint32_t pre_tick = xTaskGetTickCount();
    while (1)
    {
        App_Data_Send(&rc_data);
        vTaskDelayUntil(&pre_tick, 6);
    }
}

void Display_Task(void *param)
{
    uint32_t pre_tick = xTaskGetTickCount();
    while (1)
    {
        App_Display_Show(&rc_data);
        vTaskDelayUntil(&pre_tick, 20);
    }
}

