
#include "Drone_tasks.h"
#include <math.h>

/* 全局变量区 Start */
RC_Status_e RC_Status = eRC_UNCONNECTED;
Drone_Status_e Drone_Status = eDrone_IDLE;

uint16_t height;           // 高度
uint16_t fix_height;       //定高目标值

float static_az = 0;      //定义静态Z轴加速度

RcData_t rc_data;
/* 全局变量区 End */

void Power_Task(void *param);
#define Power_TASK_NAME "Power_Task"
#define Power_TASK_STACK_SIZE 128
#define Power_TASK_PRIORITY 4
TaskHandle_t Power_Task_Handle;

// LED任务
void LED_Task(void *param);
#define LED_TASK_NAME "LED_Task"
#define LED_TASK_STACK_SIZE 128
#define LED_TASK_PRIORITY 1
TaskHandle_t LED_Task_Handle;

// 电机任务
void Motor_Task(void *param);
#define MOTOR_TASK_NAME "Motor_Task"
#define MOTOR_TASK_STACK_SIZE 128
#define MOTOR_TASK_PRIORITY 2
TaskHandle_t Motor_Task_Handle;

// 通讯任务
void Comm_Task(void *param);
#define COMM_TASK_NAME "Comm_Task"
#define COMM_TASK_STACK_SIZE 128
#define COMM_TASK_PRIORITY 3
TaskHandle_t Comm_Task_Handle;

volatile uint16_t a = 3;

volatile double b = 4.0;

// #define LIMIT(x, min, max) (x > max) ? max : ((x < min) ? min : x) // 有问题

#define LIMIT(x, min, max) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))        //定义限幅函数
void Start_all_tasks(void)
{

    // 电机初始化
    Int_Motor_Init();
    HAL_Delay(400);
    // SI24R1的初始化
    Int_SI24R1_Init(RX_MODE);
    // MPU6050的初始化
    Int_MPU6050_Init();
    //VL53L1X的初始化
    Int_VL53L1X_Init();

    // 电源任务
    xTaskCreate(Power_Task, Power_TASK_NAME, Power_TASK_STACK_SIZE, NULL, Power_TASK_PRIORITY, &Power_Task_Handle);
    // LED任务
    xTaskCreate(LED_Task, LED_TASK_NAME, LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, &LED_Task_Handle);
    // 电机任务
    xTaskCreate(Motor_Task, MOTOR_TASK_NAME, MOTOR_TASK_STACK_SIZE, NULL, MOTOR_TASK_PRIORITY, &Motor_Task_Handle);
    // 通讯任务
    xTaskCreate(Comm_Task, COMM_TASK_NAME, COMM_TASK_STACK_SIZE, NULL, COMM_TASK_PRIORITY, &Comm_Task_Handle);

    vTaskStartScheduler();
}

void Power_Task(void *param)
{
    printf("power task start");
    while (1)
    {
        // 再来一个10000ms的延时 意思是每隔10s钟 拉低一次
        uint32_t res = ulTaskNotifyTake(pdTRUE, 10000);
        if (res == 0)
        {
            // 拉低
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
            // delay的时间按照手册来说 得在30ms以上
            vTaskDelay(100);
            // 再断开1
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);
        }
        else if (res > 0)
        {
            // 拉低
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
            // delay的时间按照手册来说 得在30ms以上
            HAL_Delay(100);
            // 再断开1
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);
            HAL_Delay(500);
            // 拉低
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
            // delay的时间按照手册来说 得在30ms以上
            HAL_Delay(100);
            // 再断开1
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);
        }
    }
}

void LED_Task(void *param)
{
    printf("LED task start");
    uint32_t bottom_led_last_toggle_tick = 0;
    while (1)
    {
        // 根据遥控状态 改变前两个灯的状态
        switch (RC_Status)
        {
        case eRC_UNCONNECTED:
            // 关闭前两个灯
            Int_LED_Off(&led_left_top);
            Int_LED_Off(&led_right_top);
            break;
        case eRC_CONNECTED:
            // 常亮前两个灯
            Int_LED_On(&led_left_top);
            Int_LED_On(&led_right_top);
            break;
        }

        switch (Drone_Status)
        {
        case eDrone_HOLD_HIGHT:
            // 定高后两个灯常亮
            Int_LED_On(&led_left_bottom);
            Int_LED_On(&led_right_bottom);
            break;
        case eDrone_FAULT:
            // 故障就关闭后两个灯
            Int_LED_Off(&led_left_bottom);
            Int_LED_Off(&led_right_bottom);
            break;
        case eDrone_IDLE:
            // toggle一次
            if (xTaskGetTickCount() - bottom_led_last_toggle_tick >= 1000)
            {
                Int_LED_Toggle(&led_left_bottom);
                Int_LED_Toggle(&led_right_bottom);
                bottom_led_last_toggle_tick = xTaskGetTickCount();
            }

            break;
        case eDrone_NORMAL:
            if (xTaskGetTickCount() - bottom_led_last_toggle_tick >= 100)
            {
                Int_LED_Toggle(&led_left_bottom);
                Int_LED_Toggle(&led_right_bottom);
                bottom_led_last_toggle_tick = xTaskGetTickCount();
            }
            break;
        }

        vTaskDelay(50);
    }
}

A_G_Struct_t ag_struct;
EulerAngle_Struct eulerAngle;
void Motor_Task(void *param)
{
    uint32_t pre_tick = xTaskGetTickCount();
    while (1)
    {
        App_Motor_GetAG_ByFilter(&ag_struct);          // 获取滤波后的加速度和角速度数据     
        Common_IMU_GetEulerAngle(&ag_struct, &eulerAngle, 0.006f);      // 四元数姿态解算 得到欧拉角

        if (Drone_Status == eDrone_IDLE)
        {
            motor_left_bottom.speed = 0;
            motor_right_bottom.speed = 0;
            motor_left_top.speed = 0;
            motor_right_top.speed = 0;
        }
        else if (Drone_Status == eDrone_NORMAL)
        {
            // 实现一个算法 根据遥控器数据 控制飞机的姿态
            App_Motor_RunPID(&rc_data, &eulerAngle, &ag_struct);        // 运行串级PID
            //将PID输出作用于电机上，注意看串级PID方框图，电机的输入是内环PID的输出，LIMIT是对PID进行输出限幅，同时对油门也进行限幅，因为油门如果封顶1000，pid将失去作用
            motor_left_bottom.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) - pitch_inner_pid.out - roll_inner_pid.out + yaw_inner_pid.out), 0, 1000);
            motor_right_bottom.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) - pitch_inner_pid.out + roll_inner_pid.out - yaw_inner_pid.out), 0, 1000);
            motor_left_top.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) + pitch_inner_pid.out - roll_inner_pid.out - yaw_inner_pid.out), 0, 1000);
            motor_right_top.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) + pitch_inner_pid.out + roll_inner_pid.out + yaw_inner_pid.out), 0, 1000);
            //俯仰角前两个电机为正，后两个电机为负(前后分别)
            //横滚角右两个电机为正，左两个电机为负(左右分别)
            //偏航角右前和左后电机为正，左前和右后电机为负(斜角分别)

        }
        else if (Drone_Status == eDrone_HOLD_HIGHT)
        {
            Int_VL53L1X_GetDistance(&height);
            printf("%d,\n", height);         //打印高度，以作为测试

            App_Motor_RunPID(&rc_data, &eulerAngle, &ag_struct);          // 运行串级PID
            App_Motor_RunHeightPID(fix_height,height,static_az,0.006);     // 加入高度PID

            // 再实现一个算法 将飞高度控制住                     对高度PID输出进行限幅
            motor_left_bottom.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) - pitch_inner_pid.out - roll_inner_pid.out + yaw_inner_pid.out + LIMIT(height_inner_pid.out,-150,150)), 0, 1000);
            motor_right_bottom.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) - pitch_inner_pid.out + roll_inner_pid.out - yaw_inner_pid.out + LIMIT(height_inner_pid.out,-150,150)), 0, 1000);
            motor_left_top.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) + pitch_inner_pid.out - roll_inner_pid.out - yaw_inner_pid.out + LIMIT(height_inner_pid.out,-150,150)), 0, 1000);
            motor_right_top.speed = LIMIT((LIMIT(rc_data.throttle, 0, 800) + pitch_inner_pid.out + roll_inner_pid.out + yaw_inner_pid.out + LIMIT(height_inner_pid.out,-150,150)), 0, 1000);
        }
        else if (Drone_Status == eDrone_FAULT)
        {
            // 故障处理
            motor_left_bottom.speed = 0;
            motor_right_bottom.speed = 0;
            motor_left_top.speed = 0;
            motor_right_top.speed = 0;
        }
        if (rc_data.throttle < 50)              // 如果油门小于50 就停止，方便测试
        {
            motor_left_bottom.speed = 0;
            motor_right_bottom.speed = 0;
            motor_left_top.speed = 0;
            motor_right_top.speed = 0;
        }

        Int_Motor_UpdateSpeed(&motor_left_bottom);
        Int_Motor_UpdateSpeed(&motor_right_bottom);
        Int_Motor_UpdateSpeed(&motor_left_top);
        Int_Motor_UpdateSpeed(&motor_right_top);

        vTaskDelayUntil(&pre_tick, 6);
    }
}

void Comm_Task(void *param)
{
    uint32_t pre_tick = xTaskGetTickCount();

    uint8_t invalid_counter = 0;
    while (1)
    {
        Data_Valid_e res = App_Data_Receive(&rc_data);
        // 1. 处理遥控的连接状态
        if (res == eData_Valid)
        {
            invalid_counter = 0;
            RC_Status = eRC_CONNECTED;
        }
        else if (res == eData_Invalid)
        {
            invalid_counter++;
            if (invalid_counter == 50)
            {
                RC_Status = eRC_UNCONNECTED;
            }
        }
        // 2. 判断off==1 如果是1 就给电源任务做一个give 让它去执行关机动作
        if (rc_data.off == 1)
        {
            xTaskNotifyGive(Power_Task_Handle);
        }

        App_Data_ProcessDroneState(&rc_data, &Drone_Status, RC_Status);

        vTaskDelayUntil(&pre_tick, 6);
    }
}