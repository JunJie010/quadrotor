# 基于STM32的四轴飞行器
>本项目基于**尚硅谷**开源学习
>
>原项目地址：[尚硅谷四轴无人机](https://www.bilibili.com/video/BV1rx4y1a7a1/?spm_id_from=333.337.search-card.all.click&vd_source=95764cfd8bb1371dc92f356cd7f2fb75)
>
>在此感谢原项目作者的杰出工作

## 项目预览
![项目展示](制作过程纪念/成品展示.jpg)
[跳转项目演示视频](https://www.bilibili.com/video/BV1RCw4zRELT/?vd_source=95764cfd8bb1371dc92f356cd7f2fb75)

## 项目功能



## 项目职责
* 编写IP5305T**电源管理**模块代码，实现电源开启后待机时长超过1min
* 编写4*LED**灯控任务**模块代码，组合显示飞行器遥控状态和飞机状态
* 编写8*KEY**按键任务**模块代码，按键分为长按和短按逻辑
* 编写**摇杆任务**模块代码，实现按键完成校准，中点对齐，微调等
* 编写SI24R1**通讯任务**模块代码，DIY无人机通讯协议，实现飞控和遥控之间的数据传输
* 编写**状态机任务**模块代码，其中油门解锁状态机用于设计摇杆解锁方式，飞行状态机设计飞机四种状态
* 编写OLED**显示任务**模块代码，在屏幕上显示摇杆数据及飞机状态
* 编写**飞控任务**模块代码，具体包括MPU6050读取原始数据，对角速度采用一阶低通滤波，加速度采用卡尔曼滤波，后四元数姿态解算得到欧拉角；
* 编写VL53L1X**定高任务**模块代码，实现对飞行器高度测量，最高4m
* 编写**串级PID控制系统**模块代码，飞控外环角度内环角速度，定高外环位置内环速度，PID参数整定，使得无人机能够抵抗外部干扰
***
## 关于开发环境
本项目控制核心采用STM32F103C8T6，移植FreeRTOS实时操作系统；

本次项目采用HAL库开发，CubeMX生成驱动文件，Keil+Vscode编译程序；

优点在于HAL库图形化配置便捷，可根据需求进行更改，使用VScode作为编辑器，可以用到AI插件，帮助代码补全，并且将文件放在工作区，方便编辑；而Keil则可以用来快速编译调试，可以用到Debug功能，设置断点方便调试代码；串口调试工具以及波形调试工具用到的是VOFA；
### 开发环境
![开发环境](制作过程/开发环境.png)
***
## 关于项目整体架构
项目整体分为飞控项目和遥控项目
![整体框架](制作过程/整体框架.png)
### 软件
![软件架构](制作过程/软件架构.png)
其中Middle为中间层，为项目移植FreeRTOS实时操作系统，进行任务调度

MDK-ARM为编译器，此次在这里我们选择了KEIL5作为编译器

INTERFACE为接口层，此处主要存放各个硬件设备的底层驱动代码
![硬件驱动](制作过程/硬件驱动.png)
DRONE_TASK为任务调度层，在这里对各个任务的优先级进行调度及编写代码

DRIVES为驱动层，放置了STM32F103C8T6的固件库

CORE为核心层，放置了HAL库生成的驱动文件
![HAL库](制作过程/HAL库生成.png)
![驱动](制作过程/驱动.png)
COMMON为中间层，放置了公共的结构体方便调用，同时串级PID代码，一阶低通滤波，卡尔曼滤波，四元数姿态解算的代码都放置在这里

APP为业务层，主要存放数据处理和电机驱动的PWM配置
***
## 关于电源任务
IP5305T 是一款集成升压转换器、锂电池充电管理、电池电量指示的多功能电源管理SOC，为移动电源提供完整的电源解决方案。
![电源电路](制作过程/电源电路.png)
```
void Power_Task(void *param)       //定义电源任务函数
{
    printf("power task start");
    while (1)
    {
        // 再来一个10000ms的延时 意思是每隔10s钟 拉低一次
        vTaskDelay(10000);              //这个代码原本在断开后，此处前置不然相当于两次短按等于关机

        // 拉低，回到电源模式
        HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
        // delay的时间按照手册来说 得在30ms以上
        vTaskDelay(100);
        // 再断开1，让它回到高阻态模式
        HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);
    }                        //开机后待机能够撑过1分钟都不关机
}
```
***
## 关于灯控任务
需求：根据4个LED灯来显示遥控状态和飞控状态，其中前两个灯表示遥控状态，后俩个灯表示飞控状态
![灯控需求](制作过程/灯控需求.jpg)
![LED](制作过程/LED电路.png)
LED驱动采用句柄化封装，显得代码更加简洁
![句柄封装](制作过程/句柄封装.png)
在公共层定义枚举类型，定义飞机遥控和飞控状态
```
typedef enum          //定义遥控状态，为枚举类型
{
    eRC_UNCONNECTED,    //未连接
    eRC_CONNECTED       //已连接
} RC_Status_e;

typedef enum          //定义飞机状态，为枚举类型
{
    eDrone_IDLE,           //定义空闲状态
    eDrone_NORMAL,         //定义正常工作状态
    eDrone_HOLD_HIGHT,     //定义定高状态
    eDrone_FAULT           //定义故障状态
} Drone_Status_e;
```
再通过switch函数实现灯光效果,其中慢闪烁和快闪烁可以用当前时间减去上一次的时间来判断，不会占用太多CPU内存，代码更好
```
case eDrone_IDLE:            //空闲状态  后两灯每隔1s闪烁(慢闪)
            // toggle一次后延时，可以实现效果，但是延长了刷新率
            if (xTaskGetTickCount() - bottom_led_last_toggle_tick >= 1000)    //用当前的时间减去上一次的时间
            {                                           
                Int_LED_Toggle(&led_left_bottom);
                Int_LED_Toggle(&led_right_bottom);
                bottom_led_last_toggle_tick = xTaskGetTickCount();      //翻转后更新以下当前的时间
            }               
            break;
        case eDrone_NORMAL:          //正常状态  后两灯每隔100ms闪烁(快闪)
            if (xTaskGetTickCount() - bottom_led_last_toggle_tick >= 100)
            {
                Int_LED_Toggle(&led_left_bottom);
                Int_LED_Toggle(&led_right_bottom);
                bottom_led_last_toggle_tick = xTaskGetTickCount();
            }
```
***


>再次感谢原项目作者的杰出工作
>
>同时也欢迎大家一起来学习
>
***
后面会继续更新，请等待...
