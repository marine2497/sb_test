// uart5.c
// 2012-12-4 19:15:01
// js200500955

#include <stdio.h>
#include "uart5.h"
#include "stm32f10x.h"

#define UART5_BUFFER_SIZE_TRANSMIT 1024
#define UART5_BUFFER_SIZE_RECEIVE 2048

#define UART5_IRQ_ENABLE(enable) \
    USART_ITConfig(UART5,USART_IT_TC,enable); \
    USART_ITConfig(UART5,USART_IT_RXNE,enable); \
    
static struct 
{
    volatile uint32_t head_transmit;
    volatile uint32_t tail_transmit;
    volatile uint32_t head_receive;
    volatile uint32_t tail_receive;
    volatile uint8_t buffer_transmit[UART5_BUFFER_SIZE_TRANSMIT];
    volatile uint8_t buffer_receive[UART5_BUFFER_SIZE_RECEIVE];
}uart5;

void uart5_init(void);
int32_t uart5_transmit(const void *data,int32_t len);
int32_t uart5_receiveValidBufferSize(void);
int32_t uart5_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);
//
static uint32_t uart5_transmitBufferSizeIdle(void);
static uint32_t uart5_transmitBufferSizeOccupy(void);
static void uart5_transmitAByte(void);
static uint8_t uart5_receivePushToBuffer(uint8_t data);

void uart5_init(void)
{
    // ���ñ���
    uart5.head_transmit = 0;
    uart5.tail_transmit = 0;
    uart5.head_receive = 0;
    uart5.tail_receive = 0;
    //
    // ��ͨʱ�ӡ�
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
    //
    // ���ô��ڡ�
    USART_DeInit(UART5);
    //
    // ���ö˿ڡ�
    GPIO_InitTypeDef pin_param;
    GPIO_StructInit(&pin_param);
    pin_param.GPIO_Mode     = GPIO_Mode_AF_PP; 	
    pin_param.GPIO_Speed    = GPIO_Speed_50MHz;
    pin_param.GPIO_Pin = GPIO_Pin_12 ;// C12:�������ţ�D02:�������š�
    GPIO_Init(GPIOC, &pin_param);
        
        /* Configure USART Rx as input floating */
    pin_param.GPIO_Pin = GPIO_Pin_2;
    pin_param.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &pin_param);
    
    //
    // ���ô��ڡ�
    USART_InitTypeDef init_param;
    USART_StructInit(&init_param);
    init_param.USART_BaudRate = 9600;
    init_param.USART_WordLength = USART_WordLength_8b;
    init_param.USART_StopBits = USART_StopBits_1;
    init_param.USART_Parity = USART_Parity_No;
    init_param.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    init_param.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART5,&init_param);
    //
    // �����жϡ�
    USART_ITConfig(UART5,USART_IT_TC,ENABLE);
    USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
    //
    NVIC_InitTypeDef nvicParam;
    nvicParam.NVIC_IRQChannel = UART5_IRQn;
    nvicParam.NVIC_IRQChannelPreemptionPriority = 0;
    nvicParam.NVIC_IRQChannelSubPriority = 8;
    nvicParam.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicParam);
    //
    // �������ڡ�
    USART_Cmd(UART5,ENABLE);
}

// ��ȡ���ͻ��������пռ��С��
uint32_t uart5_transmitBufferSizeIdle(void)
{
	if(uart5.tail_transmit <= uart5.head_transmit)
		return UART5_BUFFER_SIZE_TRANSMIT - uart5.head_transmit + uart5.tail_transmit;
	return uart5.tail_transmit - uart5.head_transmit - 1; // -1:������������
}

// ��ȡ���ͻ������е����ݳ��ȡ�
uint32_t uart5_transmitBufferSizeOccupy(void)
{
	if(uart5.tail_transmit <= uart5.head_transmit)
		return uart5.head_transmit - uart5.tail_transmit;
	return uart5.head_transmit + (UART5_BUFFER_SIZE_TRANSMIT - uart5.tail_transmit);
}

// �ӷ��ͻ�����ȡ��һ���ֽڣ����ͳ�ȥ��
// ����黺�����Ƿ�գ�����鷢��״̬��
void uart5_transmitAByte(void)
{
    USART_SendData(UART5,uart5.buffer_transmit[uart5.tail_transmit]);
	uart5.tail_transmit ++;
	if(uart5.tail_transmit >= UART5_BUFFER_SIZE_TRANSMIT)
		uart5.tail_transmit = 0;
}

// ��һ�����ݷ��뷢�ͻ�������
// ����ֵ��{UART5_SUCCEED,UART5_FAILED}
int32_t uart5_transmit(const void * data,int32_t len)
{
    UART5_IRQ_ENABLE(DISABLE);
    //
	if(uart5_transmitBufferSizeIdle() < len)
	{
        UART5_IRQ_ENABLE(ENABLE);
		return 0; // �ռ䲻����
	}
	//
	for(int i=0;i<len;i++)
	{
		uart5.buffer_transmit[uart5.head_transmit] = ((uint8_t *)data)[i];
		//
		uart5.head_transmit ++;
		if(uart5.head_transmit >= UART5_BUFFER_SIZE_TRANSMIT)
			uart5.head_transmit = 0;
		//
	}
	//
	// �������û�����ڽ��У����������͡�
	if(USART_GetFlagStatus(UART5,USART_FLAG_TXE) == SET)
        uart5_transmitAByte();
	//
    UART5_IRQ_ENABLE(ENABLE);
    //
	return len;
}

// ��ȡ���ջ������е����ݳ��ȡ�
int32_t uart5_receiveValidBufferSize(void)
{
	if(uart5.tail_receive <= uart5.head_receive)
		return uart5.head_receive - uart5.tail_receive;
	return uart5.head_receive + (UART5_BUFFER_SIZE_RECEIVE - uart5.tail_receive);
}

// ��һ���ֽڷ�����ջ�������
// ����������ʧ�ܡ�
// ����ֵ��{UART5_SUCCEED,UART5_FAILED}��
uint8_t uart5_receivePushToBuffer(uint8_t data)
{
    uint32_t newHead = uart5.head_receive + 1;
	if(newHead >= UART5_BUFFER_SIZE_RECEIVE)
		newHead = 0;
    //
    if(newHead == uart5.tail_receive)
        return UART5_FAILED;
    //
    uart5.buffer_receive[uart5.head_receive] = data;
    uart5.head_receive = newHead;
    //
    return UART5_SUCCEED;
}

// �ӽ��ջ�����ȡ�����ݣ�����ȡ���ĳ��ȡ�
// ȡ���ĳ���ΪoutputBufferLength��uart5_receiveValidBufferSize()����С�ߡ�
int32_t uart5_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength)
{
    UART5_IRQ_ENABLE(DISABLE);
    //
    // ����outputBufferLength��uart5_receiveValidBufferSize()����Сֵ��
    uint32_t returnLength = uart5_receiveValidBufferSize();
    if(outputBufferLength < returnLength)
        returnLength = outputBufferLength;
    //
    // �������ݣ��ƽ�ָ�롣
    for(uint32_t i=0;i<returnLength;i++)
    {
        ((uint8_t *)outputBuffer)[i] = uart5.buffer_receive[uart5.tail_receive];
        //
        uart5.tail_receive ++;
        if(uart5.tail_receive >= UART5_BUFFER_SIZE_RECEIVE)
            uart5.tail_receive = 0;
    }
    //
    UART5_IRQ_ENABLE(ENABLE);
    //
    return returnLength;
}

void UART5_IRQHandler(void)
{
    // ������ɡ�
    if(USART_GetFlagStatus(UART5,USART_FLAG_TC) == SET)
    {
        USART_ClearFlag(UART5,USART_FLAG_TC);
        //
        if(uart5_transmitBufferSizeOccupy() > 0)
            uart5_transmitAByte();
    }
    //
    // ���յ����ݡ�
    if(USART_GetFlagStatus(UART5,USART_FLAG_RXNE) == SET)
    {
        USART_ClearFlag(UART5,USART_FLAG_RXNE);
        //
        uint8_t data = (uint8_t)USART_ReceiveData(UART5);
        //
        uart5_receivePushToBuffer(data);
    }
}
