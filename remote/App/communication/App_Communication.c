#include "App_Communication.h"

/**
 * @description: ����ͨѶģ��
 * @return {*}
 */
void App_Communication_Start(void)
{
    debug_printfln("ͨѶģ������� ��ʼ");

    debug_printfln(" 2.4gģ���Լ� ��ʼ");
    while(Inf_Si24R1_Check() == 1)
    {
        HAL_Delay(500);
    }
    debug_printfln(" 2.4gģ���Լ� ����");

    debug_printfln(" 2.4����Ϊ����ģʽ");
    Inf_Si24R1_TXMode();

    debug_printfln("ͨѶģ������� ����");
}

/**
 * @description: ͨ��2.4g����ҡ������
 *  ǰ3���ֽ�: 0x01,0x02,0x03
 *  1���ֽ�:   ���������ݳ���  = 10
 *  10���ֽ�:  ����������
 *  4���ֽ� :  У���
 *
 * @return {*}
 */
void App_Communication_SendJoyStickData(void)
{
    uint8_t index = 0;
    /* ����֡ͷ */
    TX_BUFF[index++] = FRAME_0;
    TX_BUFF[index++] = FRAME_1;
    TX_BUFF[index++] = FRAME_2;

    /* ����ʵ�ʵ����ݳ���  �������ʵ�ʵ�����,���޸�*/
    TX_BUFF[index++] = 0;

    /* ҡ������ */
    TX_BUFF[index++] = joyStick.THR >> 8;
    TX_BUFF[index++] = joyStick.THR;

    TX_BUFF[index++] = joyStick.YAW >> 8;
    TX_BUFF[index++] = joyStick.YAW;

    TX_BUFF[index++] = joyStick.PIT >> 8;
    TX_BUFF[index++] = joyStick.PIT;

    TX_BUFF[index++] = joyStick.ROL >> 8;
    TX_BUFF[index++] = joyStick.ROL;

    TX_BUFF[index++]     = joyStick.isPowerDown;
    joyStick.isPowerDown = 0; /* ����ػ����� */

    TX_BUFF[index++]     = joyStick.isFixHeight; /* ���շ��յ�1֮��,�Զ��߽���ȡ������ */
    joyStick.isFixHeight = 0;                    /* ֻ��һ�� */

    TX_BUFF[3] = index - 4;

    /* ����У��� */
    int32_t sum = 0;
    for(uint8_t i = 0; i < index; i++)
    {
        sum += TX_BUFF[i];
    }
    TX_BUFF[index++] = sum >> 24;
    TX_BUFF[index++] = sum >> 16;
    TX_BUFF[index++] = sum >> 8;
    TX_BUFF[index++] = sum;

    taskENTER_CRITICAL();
    Inf_Si24R1_TxPacket(TX_BUFF);
    taskEXIT_CRITICAL();
}
