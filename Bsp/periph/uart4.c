// uart4.c
// 2012-12-4 19:14:01
// js200400954

#include <stdio.h>
#include "uart4.h"
#include "stm32f10x.h"

#define UART4_BUFFER_SIZE_TRANSMIT 1024
#define UART4_BUFFER_SIZE_RECEIVE 2048

#define UART4_IRQ_ENABLE(enable) \
    USART_ITConfig(UART4,USART_IT_TC,enable); \
    USART_ITConfig(UART4,USART_IT_RXNE,enable); \
    
static struct 
{
    volatile uint32_t head_transmit;
    volatile uint32_t tail_transmit;
    volatile uint32_t head_receive;
    volatile uint32_t tail_receive;
    volatile uint8_t buffer_transmit[UART4_BUFFER_SIZE_TRANSMIT];
    volatile uint8_t buffer_receive[UART4_BUFFER_SIZE_RECEIVE];
}uart4;

void uart4_init(void);
int32_t uart4_transmit(const void *data,int32_t len);
int32_t uart4_receiveValidBufferSize(void);
int32_t uart4_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);
//
static uint32_t uart4_transmitBufferSizeIdle(void);
static uint32_t uart4_transmitBufferSizeOccupy(void);
static void uart4_transmitAByte(void);
static uint8_t uart4_receivePushToBuffer(uint8_t data);

void uart4_init(void)
{
    // 设置变量
    uart4.head_transmit = 0;
    uart4.tail_transmit = 0;
    uart4.head_receive = 0;
    uart4.tail_receive = 0;
    //
    // 接通时钟。
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
    //
    // 重置串口。
    USART_DeInit(UART4);
    //
    // 设置端口。
    GPIO_InitTypeDef pin_param;
    GPIO_StructInit(&pin_param);
    pin_param.GPIO_Mode     = GPIO_Mode_AF_PP; 	
    pin_param.GPIO_Speed    = GPIO_Speed_50MHz;
    pin_param.GPIO_Pin = GPIO_Pin_10 ;// c10:发送引脚，c11:接收引脚。
    GPIO_Init(GPIOC, &pin_param);
        
        /* Configure USART Rx as input floating */
    pin_param.GPIO_Pin = GPIO_Pin_11;
    pin_param.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &pin_param);
    
    //
    // 配置串口。
    USART_InitTypeDef init_param;
    USART_StructInit(&init_param);
    init_param.USART_BaudRate = 115200;
    init_param.USART_WordLength = USART_WordLength_8b;
    init_param.USART_StopBits = USART_StopBits_1;
    init_param.USART_Parity = USART_Parity_No;
    init_param.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    init_param.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4,&init_param);
    //
    // 设置中断。
    USART_ITConfig(UART4,USART_IT_TC,ENABLE);
    USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
    //
    NVIC_InitTypeDef nvicParam;
    nvicParam.NVIC_IRQChannel = UART4_IRQn;
    nvicParam.NVIC_IRQChannelPreemptionPriority = 0;
    nvicParam.NVIC_IRQChannelSubPriority = 7;
    nvicParam.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicParam);
    //
    // 启动串口。
    USART_Cmd(UART4,ENABLE);
}

// 获取发送缓冲区空闲空间大小。
uint32_t uart4_transmitBufferSizeIdle(void)
{
	if(uart4.tail_transmit <= uart4.head_transmit)
		return UART4_BUFFER_SIZE_TRANSMIT - uart4.head_transmit + uart4.tail_transmit;
	return uart4.tail_transmit - uart4.head_transmit - 1; // -1:不可以填满。
}

// 获取发送缓冲区中的数据长度。
uint32_t uart4_transmitBufferSizeOccupy(void)
{
	if(uart4.tail_transmit <= uart4.head_transmit)
		return uart4.head_transmit - uart4.tail_transmit;
	return uart4.head_transmit + (UART4_BUFFER_SIZE_TRANSMIT - uart4.tail_transmit);
}

// 从发送缓冲区取出一个字节，发送出去。
// 不检查缓冲区是否空，不检查发送状态。
void uart4_transmitAByte(void)
{
    USART_SendData(UART4,uart4.buffer_transmit[uart4.tail_transmit]);
	uart4.tail_transmit ++;
	if(uart4.tail_transmit >= UART4_BUFFER_SIZE_TRANSMIT)
		uart4.tail_transmit = 0;
}

// 把一段数据放入发送缓冲区。
// 返回值：{UART4_SUCCEED,UART4_FAILED}
int32_t uart4_transmit(const void * data,int32_t len)
{
    UART4_IRQ_ENABLE(DISABLE);
    //
	if(uart4_transmitBufferSizeIdle() < len)
	{
        UART4_IRQ_ENABLE(ENABLE);
		return 0; // 空间不够。
	}
	//
	for(int i=0;i<len;i++)
	{
		uart4.buffer_transmit[uart4.head_transmit] = ((uint8_t *)data)[i];
		//
		uart4.head_transmit ++;
		if(uart4.head_transmit >= UART4_BUFFER_SIZE_TRANSMIT)
			uart4.head_transmit = 0;
		//
	}
	//
	// 如果发送没有正在进行，就启动发送。
	if(USART_GetFlagStatus(UART4,USART_FLAG_TXE) == SET)
        uart4_transmitAByte();
	//
    UART4_IRQ_ENABLE(ENABLE);
    //
	return len;
}

// 获取接收缓冲区中的数据长度。
int32_t uart4_receiveValidBufferSize(void)
{
	if(uart4.tail_receive <= uart4.head_receive)
		return uart4.head_receive - uart4.tail_receive;
	return uart4.head_receive + (UART4_BUFFER_SIZE_RECEIVE - uart4.tail_receive);
}

// 把一个字节放入接收缓冲区。
// 缓冲区满则失败。
// 返回值：{UART4_SUCCEED,UART4_FAILED}。
uint8_t uart4_receivePushToBuffer(uint8_t data)
{
    uint32_t newHead = uart4.head_receive + 1;
	if(newHead >= UART4_BUFFER_SIZE_RECEIVE)
		newHead = 0;
    //
    if(newHead == uart4.tail_receive)
        return UART4_FAILED;
    //
    uart4.buffer_receive[uart4.head_receive] = data;
    uart4.head_receive = newHead;
    //
    return UART4_SUCCEED;
}

// 从接收缓冲区取出数据，返回取出的长度。
// 取出的长度为outputBufferLength和uart4_receiveValidBufferSize()的最小者。
int32_t uart4_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength)
{
    UART4_IRQ_ENABLE(DISABLE);
    //
    // 计算outputBufferLength和uart4_receiveValidBufferSize()的最小值。
    uint32_t returnLength = uart4_receiveValidBufferSize();
    if(outputBufferLength < returnLength)
        returnLength = outputBufferLength;
    //
    // 复制数据，推进指针。
    for(uint32_t i=0;i<returnLength;i++)
    {
        ((uint8_t *)outputBuffer)[i] = uart4.buffer_receive[uart4.tail_receive];
        //
        uart4.tail_receive ++;
        if(uart4.tail_receive >= UART4_BUFFER_SIZE_RECEIVE)
            uart4.tail_receive = 0;
    }
    //
    UART4_IRQ_ENABLE(ENABLE);
    //
    return returnLength;
}

void UART4_IRQHandler(void)
{
    // 发送完成。
    if(USART_GetFlagStatus(UART4,USART_FLAG_TC) == SET)
    {
        USART_ClearFlag(UART4,USART_FLAG_TC);
        //
        if(uart4_transmitBufferSizeOccupy() > 0)
            uart4_transmitAByte();
    }
    //
    // 接收到数据。
    if(USART_GetFlagStatus(UART4,USART_FLAG_RXNE) == SET)
    {
        USART_ClearFlag(UART4,USART_FLAG_RXNE);
        //
        uint8_t data = (uint8_t)USART_ReceiveData(UART4);
        //
        uart4_receivePushToBuffer(data);
    }
}
