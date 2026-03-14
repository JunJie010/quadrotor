#include "App_motor.h"

int16_t last_gyro_x_out = 0;
int16_t last_gyro_y_out = 0;
int16_t last_gyro_z_out = 0;

// int16_t last_accel_x_out = 0;
void App_Motor_GetAG_ByFilter(A_G_Struct_t *ag_struct)
{
    Int_MPU6050_ReadDataByCalibration(&ag_struct->accel, &ag_struct->gyro);

    // 套用滤波器

    ag_struct->gyro.x = Common_LPF(ag_struct->gyro.x, last_gyro_x_out, 0.15);
    ag_struct->gyro.y = Common_LPF(ag_struct->gyro.y, last_gyro_y_out, 0.15);
    ag_struct->gyro.z = Common_LPF(ag_struct->gyro.z, last_gyro_z_out, 0.15);
    // 更新上一次的输出
    last_gyro_x_out = ag_struct->gyro.x;
    last_gyro_y_out = ag_struct->gyro.y;
    last_gyro_z_out = ag_struct->gyro.z;

    // 针对加速度（加速度噪音更多） 我们使用卡尔曼滤波器进行滤波
    // printf("%d,", ag_struct->accel.x); // 打印加速度 x 的原始数据

    // int16_t temp_accel_x_filter = Common_LPF(ag_struct->accel.x, last_accel_x_out, 0.15);
    // last_accel_x_out = temp_accel_x_filter;
    // printf("%d,", temp_accel_x_filter); // 一阶低通滤波 加速度 x 的效果

    ag_struct->accel.x = Common_Filter_KalmanFilter(&kfs[0], ag_struct->accel.x);
    ag_struct->accel.y = Common_Filter_KalmanFilter(&kfs[1], ag_struct->accel.y);
    ag_struct->accel.z = Common_Filter_KalmanFilter(&kfs[2], ag_struct->accel.z);

    // printf("%d\n", ag_struct->accel.x);
} 

//数量级确定，参数范围1-5，dt时间间隔6ms                                                                                              
Single_PID_Handle_t pitch_outter_pid = {.kp = 6.0f, .ki = 0.0f, .kd = 0.0f, .dt = 0.006};
Single_PID_Handle_t pitch_inner_pid = {.kp = -3.0f, .ki = 0.0f, .kd = -0.2f, .dt = 0.006};         //俯仰角内环外环PID参数
                                         //6 0 0     -3 0 -0.2   
Single_PID_Handle_t roll_outter_pid = {.kp = 6.0f, .ki = 0.0f, .kd = 0.0f, .dt = 0.006};
Single_PID_Handle_t roll_inner_pid = {.kp = -3.0f, .ki = 0.0f, .kd = -0.2f, .dt = 0.006};            //横滚角内环外环PID参数
                                         //6 0 0     -3 0 -0.2
Single_PID_Handle_t yaw_outter_pid = {.kp = 3.0f, .ki = 0.0f, .kd = 0.0f, .dt = 0.006};
Single_PID_Handle_t yaw_inner_pid = {.kp = -5.0f, .ki = 0.0f, .kd = 0.0f, .dt = 0.006};              //偏航角内环外环PID参数
                                         //3 0 0   -5 0 0
Single_PID_Handle_t height_outter_pid = {.kp = -1.2f, .ki = 0.0f, .kd = -0.085f, .dt = 0.006};
Single_PID_Handle_t height_inner_pid = {.kp = -1.2f, .ki = 0.0f, .kd = -0.085f, .dt = 0.006};            //高度内环外环PID参数
                                         //-1.2  0  -0.085       -1.2  0  -0.085

void App_Motor_RunPID(RcData_t *rc_data, EulerAngle_Struct *euler, A_G_Struct_t *ag_struct)       // 定义运行PID控制算法函数
{
    pitch_outter_pid.target = (rc_data->pitch - 500) * 0.03;            //0-1000先变成(-500)-500，而后500*0.03是15，即(-15)-15
    roll_outter_pid.target = (rc_data->roll - 500) * 0.03;
    yaw_outter_pid.target = (rc_data->yaw - 500) * 0.03;                //外环的目标值等于遥控器输入值映射后的值

    pitch_outter_pid.measure = euler->pitch;        
    roll_outter_pid.measure = euler->roll;
    yaw_outter_pid.measure = euler->yaw;                                //外环的测量值等于当前欧拉角(姿态解算后的角度)

    pitch_inner_pid.measure = ag_struct->gyro.y * 4000.0 / 65535;       
    //printf("%f\n", pitch_inner_pid.measure);                   //打印内环的测量值，以作调试
    roll_inner_pid.measure = ag_struct->gyro.x * 4000.0 / 65535;
    yaw_inner_pid.measure = ag_struct->gyro.z * 4000.0 / 65535;          //内环的测量值等于当前陀螺仪角速度(姿态解算前原始数据)需要转换成正常的角速度              

    Common_Serial_PID(&pitch_outter_pid, &pitch_inner_pid);           //调用串级PID函数计算PID
    Common_Serial_PID(&roll_outter_pid, &roll_inner_pid);
    Common_Serial_PID(&yaw_outter_pid, &yaw_inner_pid);
}

uint16_t last_height = 0;       // 上一次高度
float speed2 = 0;           //加速度积分和

void App_Motor_RunHeightPID(uint16_t fix_height,uint16_t current_height,float static_az,float dt)       // 运行高度PID控制算法函数
{
    height_outter_pid.target = fix_height;             //外环的目标值等于定高值

    height_outter_pid.measure = current_height;        //外环的测量值等于当前高度

    //算内环的垂直方向速度为速度1
    float speed1 = (current_height - last_height)/dt; 
    last_height = current_height;      //更新上一次高度

    //加速度求积分为速度2
    speed2 += (Common_IMU_GetNormAccZ() - static_az)*dt;   
    
    height_inner_pid.measure=(0.02*speed1 + 0.98*speed2);        //内环的测量值，其中两个速度加入了权重
    speed2=(0.02*speed1 + 0.98*speed2);                         //更新速度2

    Common_Serial_PID(&height_outter_pid, &height_inner_pid);        //调用串级PID函数计算PID
}
