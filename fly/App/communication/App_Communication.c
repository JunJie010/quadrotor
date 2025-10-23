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
    Inf_Si24R1_RXMode();

    debug_printfln("ͨѶģ������� ����");
}

/**
 * @description: ����ҡ������
 * @return {*} Com_Status �Ƿ��յ�����: Com_OK�յ� ����û���յ�
 */
Com_Status App_Communication_ReceiveJoyStickData(void)
{
    /* 1. ��������, ���û���յ�, ֱ�ӷ��� fail */
    taskENTER_CRITICAL();
    uint8_t r = Inf_Si24R1_RxPacket(RX_BUFF);
    taskEXIT_CRITICAL();
    if(r == 1) return Com_FAIL;

    /* 2. �յ�����: �ж�֡ͷ�Ƿ����(�Ƿ����Լ���ң��������) */
    if(RX_BUFF[0] != FRAME_0 ||
       RX_BUFF[1] != FRAME_1 ||
       RX_BUFF[2] != FRAME_2) return Com_FAIL;

    /* 3. ���������� */
    joyStick.THR = (RX_BUFF[4] << 8) | RX_BUFF[5];
    joyStick.YAW = (RX_BUFF[6] << 8) | RX_BUFF[7];
    joyStick.PIT = (RX_BUFF[8] << 8) | RX_BUFF[9];
    joyStick.ROL = (RX_BUFF[10] << 8) | RX_BUFF[11];

    joyStick.isPowerDown = RX_BUFF[12];

    if(RX_BUFF[13]) /* �� RX_BUFF[13] ��1ʱ  �л�����ģʽ */
    {
        joyStick.isFixHeight = !joyStick.isFixHeight;

        isFixHeight = joyStick.isFixHeight == 1 ? Com_OK : Com_FAIL;
    }

    return Com_OK;
}

/**
 * @description: �������ң�������������
 *  ���ӳɹ�:
 *      ֻҪ�յ�һ������,�������ӳɹ�
 *  ʧ��:
 *      ������� 1.2s û���յ�����,��ʾʧ��
 * @param {Com_Status} isReceiveData �Ƿ��յ�ң��������
 * @return {*}  �Ƿ����ӳɹ�
 */
Com_Status App_Communication_CheckConnection(Com_Status isReceiveData)
{                             /* ��¼�����ղ������ݵĴ��� */
    static uint8_t cnt = 200; /* ��ʼ��Ϊ200���Է�ֹһ����ʱ��,û�����ӳɹ���ʾ���ӳɹ�����*/
    /* 1. ����յ�����,��ʾ���ӳɹ� */
    if(isReceiveData == Com_OK)
    {
        cnt = 0;
        return Com_OK;
    }

    /* 2. ��ʾû���յ�����: ��ʱ, ����200��(1.2s) */
    cnt++;
    if(cnt >= 200)
    {
        cnt = 200; /* ������� */
        return Com_FAIL;
    }

    return Com_OK;
}

/**
 * @description: ң�����Ľ���
 *  �������������,����1s,Ȼ�����������,����1s, �����ɹ�.
 *
 *   ����״̬
 *       �ڴ�״̬, ��������������(>=960), ����  ���ֵ���� ״̬
 *   ���ֵ����
 *       ������ų���ʱ�䳬��1s ,�뿪���ֵ״̬
 *       ������������ʱ�䲻��1s, �˻ص� ����״̬
 *   �뿪���ֵ
 *       ����������� <=20, ������Сֵ����
 *   ��Сֵ����
 *       �������ʱ�����1s, �������״̬
 *       �������ʱ�䲻��1s, ��������״̬
 *   ����
 *
 *
 *
 *
 * @param {Com_Status} isRemoteConnected �Ƿ����ӳɹ�
 * @return {*} �Ƿ�����ɹ� Com_OK �����ɹ�  ���� ����ʧ��
 */
Com_Status App_Communication_RemoteUnlock(Com_Status isRemoteConnected)
{
    /* ���������ֵ����Сֵ�����ĳ���ʱ�� */
    static uint8_t thrMaxDuration = 0;
    static uint8_t thrMinDuration = 0;
    /* �������ŵ�״̬ */
    static Com_RemoteStatus remoteStatus = THR_FREE;

    /* 1. ���ʧ��״̬, ֱ�ӽ���ʧ�� */
    if(isRemoteConnected != Com_OK)
    {
        remoteStatus         = THR_FREE;
        joyStick.isFixHeight = 0;
        isFixHeight          = Com_FAIL;
        return Com_FAIL;
    }

    switch(remoteStatus)
    {
        case THR_FREE:
        {
            /* ������״̬, ���ֵ����Сֵ��������ʱ���0���� */
            thrMaxDuration = 0;
            thrMinDuration = 0;
            if(joyStick.THR >= 960)
            {
                remoteStatus = THR_MAX;
            }
            break;
        }
        case THR_MAX:
        {
            if(joyStick.THR >= 960)
            {
                /* �жϳ���ʱ�� */
                thrMaxDuration++;
                if(thrMaxDuration >= 200)
                {
                    remoteStatus = THR_MAX_LEAVE;
                }
            }
            else
            {
                /* ֤�������ֵ����û�м��1.2s */
                remoteStatus = THR_FREE; /* �ص�����״̬ */
            }
            break;
        }
        case THR_MAX_LEAVE:
        {
            if(joyStick.THR <= 20)
            {
                remoteStatus = THR_MIN;
            }
            break;
        }
        case THR_MIN:
        {
            if(joyStick.THR <= 20)
            {
                thrMinDuration++;
                if(thrMinDuration >= 200)
                {
                    remoteStatus = THR_UNLOCK;
                }
            }
            else
            {
                remoteStatus = THR_FREE;
            }
            break;
        }
        case THR_UNLOCK:
        {
            /* �������״̬��,���ų�ʱ��(1min) <=20, �������� */
            static uint32_t lowDuration = 0;
            if(joyStick.THR <= 20)
            {
                lowDuration++;
                if(lowDuration >= 200 * 50)
                {
                    remoteStatus = THR_FREE;
                    lowDuration  = 0;
                }
            }
            else
            {
                lowDuration = 0;
            }

            /* �����ɹ�, ֱ�ӷ��سɹ� */
            return Com_OK;
        }

        default:
            break;
    }

    joyStick.isFixHeight = 0;
    isFixHeight          = Com_FAIL;
    /* Ĭ�Ϸ��ؽ���ʧ�� */
    return Com_FAIL;
}
