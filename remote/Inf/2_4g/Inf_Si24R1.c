#include "Inf_Si24R1.h"

uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x0A, 0x01, 0x07, 0x0E, 0x01};   // ����һ����̬���͵�ַ
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x0A, 0x01, 0x07, 0x0E, 0x01};   // ����һ����̬���͵�ַ

uint8_t TX_BUFF[TX_PLOAD_WIDTH];
uint8_t RX_BUFF[RX_PLOAD_WIDTH];

/* ��־λ������յ����ݣ�����0�������ط��᲻�϶���++ */
uint16_t connect_flag = 1;   // Ĭ�ϸ�һ������0����

/**
 * @description: д�Ĵ���
 * @param {uint8_t} reg �Ĵ�����ַ
 * @param {uint8_t} data Ҫд����ֽ�
 * @return {*}
 */
uint8_t Inf_Si24R1_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t status = 0;
    /* 1��Ƭѡѡ�� */
    SI24R1_CS_L;
    /* 2��д�Ĵ�����ַ */
    status = Driver_SPI_SwapByte(reg);
    /* 3��д������ */
    Driver_SPI_SwapByte(data);
    /* 4��Ƭѡȡ�� */
    SI24R1_CS_H;

    return status;
}

/**
 * @description: ���Ĵ���
 * @param {uint8_t} reg Ҫ��ȡ�ļĴ���
 * @return {*} ��ȡ����ֵ
 */
uint8_t Inf_Si24R1_ReadReg(uint8_t reg)
{
    uint8_t res = 0;
    /* 1��Ƭѡѡ�� */
    SI24R1_CS_L;
    /* 2��д�Ĵ�����ַ(��ָ��) */
    Driver_SPI_SwapByte(reg);
    /* 3����ȡ���ݣ�дʲô����ν�� */
    res = Driver_SPI_SwapByte(0);
    /* 4��Ƭѡȡ�� */
    SI24R1_CS_H;

    return res;
}

/**
 * @description: ��ָ���Ĵ���д����ֽ�
 * @param {uint8_t} reg д��ļĴ�����ַ
 * @param {uint8_t*} pBuf   д��Ķ���ֽ�����ָ��
 * @param {uint8_t} len �����ֽ���
 * @return {*}
 */
uint8_t Inf_Si24R1_WriteBuf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    uint8_t status = 0;
    /* 1��Ƭѡѡ�� */
    SI24R1_CS_L;
    /* 2��д�Ĵ�����ַ */
    status = Driver_SPI_SwapByte(reg);
    /* 3��ѭ��д�����ֽ����� */
    for(uint8_t i = 0; i < len; i++)
    {
        Driver_SPI_SwapByte(*pBuf++);
    }
    /* 4��Ƭѡȡ�� */
    SI24R1_CS_H;

    return status;
}

/**
 * @description: ������ֽ�
 * @param {uint8_t} reg Ҫ��ȡ�ļĴ�����ַ
 * @param {uint8_t *} pBuf  �������յ����ݻ����ָ��
 * @param {uint8_t} len ���յ��ֽ���
 * @return {*}
 */
uint8_t Inf_Si24R1_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    /* 1��Ƭѡѡ�� */
    SI24R1_CS_L;
    /* 2��д�Ĵ�����ַ(��ָ��) */
    Driver_SPI_SwapByte(reg);
    /* 3��ѭ����ȡ����ֽ����ݣ�дʲô����ν�� */
    for(uint8_t i = 0; i < len; i++)
    {
        *pBuf++ = Driver_SPI_SwapByte(0);
    }

    /* 4��Ƭѡȡ�� */
    SI24R1_CS_H;

    return 0;
}

/**
 * @description: ����ģʽ��ʼ�����ο����̡��Ĵ���ӳ���
 * @return {*}
 */
void Inf_Si24R1_TXMode(void)
{
    /* 1���������ģʽ��CE=0 */
    SI24R1_EN_L;
    /* 2����ص����ã����͵�ַ�����չܵ�0��ַ��һ������ACKʹ�ܡ�ʹ�ܹܵ�0�����ʡ�ConfigΪ����ģʽ */
    Inf_Si24R1_WriteBuf(SPI_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);      // д�뷢�͵�ַ
    Inf_Si24R1_WriteBuf(SPI_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);   // Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + EN_AA, 0x01);        // ʹ�ܽ���ͨ��0�Զ�Ӧ��
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + EN_RXADDR, 0x01);    // ʹ�ܽ���ͨ��0
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + SETUP_RETR, 0x0a);   // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + RF_CH, CH);          // ѡ����Ƶͨ��0x40
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + RF_SETUP, 0x0f);     // ���ݴ�����2Mbps�����书��7dBm
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + CONFIG, 0x0e);       // CRCʹ�ܣ�16λCRCУ�飬�ϵ�
    /* 3��ʹ��CE */
    SI24R1_EN_H;
}

/**
 * @description: ����ģʽ
 * @return {*}
 */
void Inf_Si24R1_RXMode(void)
{
    SI24R1_EN_L;
    /*
        �뷢�͵�����
            1������Ҫ���÷��͵�ַ
            2����Ҫ���ý���ͨ��0�ĸ��س���
            3��config���õģ�bit0=1 Ϊ����ģʽ
     */
    Inf_Si24R1_WriteBuf(SPI_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);   // �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + EN_AA, 0x01);                            // ʹ�ܽ���ͨ��0�Զ�Ӧ��
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + EN_RXADDR, 0x01);                        // ʹ�ܽ���ͨ��0
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + RF_CH, CH);                              // ѡ����Ƶͨ��0x40
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);               // ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + RF_SETUP, 0x0f);                         // ���ݴ�����2Mbps�����书��7dBm
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + CONFIG, 0x0f);                           // CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
    SI24R1_EN_H;                                                           // ����CE���������豸
}

/**
 * @description: ����һ�����ݰ�
 * @param {uint8_t} *txBuf
 * @return {*}
 */
uint8_t Inf_Si24R1_TxPacket(uint8_t *txBuf)
{
    uint8_t state = 0;
    /* 1��ʹ��дTx FIFOָ������ݷ��� */
    SI24R1_EN_L;
    Inf_Si24R1_WriteBuf(WR_TX_PLOAD, txBuf, TX_PLOAD_WIDTH);
    SI24R1_EN_H;   // ȷ�����뷢��ģʽ

    /* 2���жϷ�����ɣ���ﵽ����ط���������ѭ����ȡ״̬�Ĵ��������ж�bit4��bit5 */
    while(!(state & (TX_OK | MAX_TX)))
    {
        state = Inf_Si24R1_ReadReg(SPI_READ_REG + STATUS);
    }

    /* 3�� ��� ���ջ�����ط����� �жϱ�־λ */
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + STATUS, state);

    /* 4���ﵽ����ط���������Ҫ�������Tx FIFO�������޷��������� */
    if(state & MAX_TX)
    {
        if(state & 0x01)   // bit0: ���TX FIFO���ˣ�=1��û��=0
        {
            Inf_Si24R1_WriteReg(FLUSH_TX, 0xff);
        }
    }

    /* 5������Ƿ��ͳɹ� */
    if(state & TX_OK)
    {
        return 0;
    }

    return 1;   // ����ԭ��δ�ɹ�
}

/**
 * @description: ����һ�����ݰ�
 * @param {uint8_t} *txBuf  ���ȡ������
 * @return {*}  0���ɹ��������ݣ�1��δ��������
 */
uint8_t Inf_Si24R1_RxPacket(uint8_t *rxBuf)
{
    /* 1����״̬�Ĵ������ж��Ƿ��յ������� */
    uint8_t state = 0;
    state         = Inf_Si24R1_ReadReg(SPI_READ_REG + STATUS);

    /* 2�� ��� �����ж� ��־ */
    Inf_Si24R1_WriteReg(SPI_WRITE_REG + STATUS, state);

    /* 3������յ������ˣ��Ϳ�ʼ�� RX FIFO */
    if(state & RX_OK)
    {
        /* 2.1 �� RX FIFO��ȡ���ݵ� buff �� */
        Inf_Si24R1_ReadBuf(RD_RX_PLOAD, rxBuf, RX_PLOAD_WIDTH);
        /* 2.2 ��� RX FIFO */
        Inf_Si24R1_WriteReg(FLUSH_RX, 0xff);
        /* 2.3 ���յ����ݺ󣬶Ա�־λ��0 */
        connect_flag = 0;

        /* =============���ԣ���ӡ���յ�����================= */
        // for (uint8_t i = 0; i < RX_PLOAD_WIDTH; i++)
        // {
        //     printf("receive[%d]=%02x\r\n", i, txBuf[i]);
        // }
        /* ================================================ */

        return 0;   // �ɹ���������
    }

    return 1;   // δ���յ�����
}

/**
 * @description: �Լ�
 * @return {*}
 */
uint8_t Inf_Si24R1_Check(void)
{
    uint8_t buff_w[5] = {0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
    uint8_t buff_r[5] = {0};
    uint8_t count     = 0;

    /* 1�����Ĵ���д�뼸���ֽ� */
    Inf_Si24R1_WriteBuf(SPI_WRITE_REG + TX_ADDR, buff_w, 5);

    /* 2���ӼĴ������������ֽ� */
    Inf_Si24R1_ReadBuf(SPI_READ_REG + TX_ADDR, buff_r, 5);

    /* 3���ж��Ƿ���ͬ */
    for(uint8_t i = 0; i < 5; i++)
    {
        if(buff_r[i] == buff_w[i])
        {
            count++;
        }
    }

    if(count == 5)
    {
        return 0;   // У��ɹ�
    }
    else
    {
        return 1;   // У��ʧ��
    }
}
