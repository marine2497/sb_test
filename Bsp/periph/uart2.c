// uart2.c
// 2012-12-4 19:13:01
// js200300953

#include <stdio.h>
#include "uart2.h"
#include "stm32f10x.h"

#define UART2_BUFFER_SIZE_TRANSMIT 1024
#define UART2_BUFFER_SIZE_RECEIVE 2048

#define UART2_IRQ_ENABLE(enable) \
    USART_ITConfig(USART2,USART_IT_TC,enable); \
    USART_ITConfig(USART2,USART_IT_RXNE,enable); \

static struct 
{
    volatile uint32_t head_transmit;
    volatile uint32_t tail_transmit;
    volatile uint32_t head_receive;
    volatile uint32_t tail_receive;
    volatile uint8_t buffer_transmit[UART2_BUFFER_SIZE_TRANSMIT];
    volatile uint8_t buffer_receive[UART2_BUFFER_SIZE_RECEIVE];
}uart2;

void uart2_init(void);
int32_t uart2_transmit(const void *data,int32_t len);
int32_t uart2_receiveValidBufferSize(void);
int32_t uart2_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);
//
static uint32_t uart2_transmitBufferSizeIdle(void);
static uint32_t uart2_transmitBufferSizeOccupy(void);
static void uart2_transmitAByte(void);
static uint8_t uart2_receivePushToBuffer(uint8_t data);

void uart2_init(void)
{
    // 设置变量
    uart2.head_transmit = 0;
    uart2.tail_transmit = 0;
    uart2.head_receive = 0;
    uart2.tail_receive = 0;
    //
    // 接通时钟。
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    //
    // 重置串口。
    USART_DeInit(USART2);
    //
       
    GPIO_InitTypeDef pin_param;
    GPIO_StructInit(&pin_param);
    pin_param.GPIO_Mode     = GPIO_Mode_AF_PP; 	
    pin_param.GPIO_Speed    = GPIO_Speed_50MHz;
    pin_param.GPIO_Pin = GPIO_Pin_2 ;// A2:发送引脚，A3:接收引脚。	
    GPIO_Init(GPIOA, &pin_param);
        
        /* Configure USART Rx as input floating */
    pin_param.GPIO_Pin = GPIO_Pin_3;
    pin_param.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &pin_param);
    
    
    
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
    USART_Init(USART2,&init_param);
    //
    // 设置中断。
    USART_ITConfig(USART2,USART_IT_TC,ENABLE);
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    //
    NVIC_InitTypeDef nvicParam;
    nvicParam.NVIC_IRQChannel = USART2_IRQn;
    nvicParam.NVIC_IRQChannelPreemptionPriority = 0;
    nvicParam.NVIC_IRQChannelSubPriority = 5;
    nvicParam.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicParam);
    //
    // 启动串口。
    USART_Cmd(USART2,ENABLE);
}

// 获取发送缓冲区空闲空间大小。
uint32_t uart2_transmitBufferSizeIdle(void)
{
	if(uart2.tail_transmit <= uart2.head_transmit)
		return UART2_BUFFER_SIZE_TRANSMIT - uart2.head_transmit + uart2.tail_transmit;
	return uart2.tail_transmit - uart2.head_transmit - 1; // -1:不可以填满。
}

// 获取发送缓冲区中的数据长度。
uint32_t uart2_transmitBufferSizeOccupy(void)
{
	if(uart2.tail_transmit <= uart2.head_transmit)
		return uart2.head_transmit - uart2.tail_transmit;
	return uart2.head_transmit + (UART2_BUFFER_SIZE_TRANSMIT - uart2.tail_transmit);
}

// 从发送缓冲区取出一个字节，发送出去。
// 不检查缓冲区是否空，不检查发送状态。
void uart2_transmitAByte(void)
{
    USART_SendData(USART2,uart2.buffer_transmit[uart2.tail_transmit]);
	uart2.tail_transmit ++;
	if(uart2.tail_transmit >= UART2_BUFFER_SIZE_TRANSMIT)
		uart2.tail_transmit = 0;
}

// 把一段数据放入发送缓冲区。
// 返回值：{UART2_SUCCEED,UART2_FAILED}
int32_t uart2_transmit(const void * data,int32_t len)
{
    UART2_IRQ_ENABLE(DISABLE);
    //
	if(uart2_transmitBufferSizeIdle() < len)
	{
        UART2_IRQ_ENABLE(ENABLE);
		return 0; // 空间不够。
	}
	//
	for(int i=0;i<len;i++)
	{
		uart2.buffer_transmit[uart2.head_transmit] = ((uint8_t *)data)[i];
		//
		uart2.head_transmit ++;
		if(uart2.head_transmit >= UART2_BUFFER_SIZE_TRANSMIT)
			uart2.head_transmit = 0;
		//
	}
	//
	// 如果发送没有正在进行，就启动发送。
	if(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == SET)
        uart2_transmitAByte();
	//
    UART2_IRQ_ENABLE(ENABLE);
    //
	return len;
}

// 获取接收缓冲区中的数据长度。
int32_t uart2_receiveValidBufferSize(void)
{
	if(uart2.tail_receive <= uart2.head_receive)
		return uart2.head_receive - uart2.tail_receive;
	return uart2.head_receive + (UART2_BUFFER_SIZE_RECEIVE - uart2.tail_receive);
}

// 把一个字节放入接收缓冲区。
// 缓冲区满则失败。
// 返回值：{UART2_SUCCEED,UART2_FAILED}。
uint8_t uart2_receivePushToBuffer(uint8_t data)
{
    uint32_t newHead = uart2.head_receive + 1;
	if(newHead >= UART2_BUFFER_SIZE_RECEIVE)
		newHead = 0;
    //
    if(newHead == uart2.tail_receive)
        return UART2_FAILED;
    //
    uart2.buffer_receive[uart2.head_receive] = data;
    uart2.head_receive = newHead;
    //
    return UART2_SUCCEED;
}

// 从接收缓冲区取出数据，返回取出的长度。
// 取出的长度为outputBufferLength和uart2_receiveValidBufferSize()的最小者。
int32_t uart2_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength)
{
    UART2_IRQ_ENABLE(DISABLE);
    //
    // 计算outputBufferLength和uart2_receiveValidBufferSize()的最小值。
    uint32_t returnLength = uart2_receiveValidBufferSize();
    if(outputBufferLength < returnLength)
        returnLength = outputBufferLength;
    //
    // 复制数据，推进指针。
    for(uint32_t i=0;i<returnLength;i++)
    {
        ((uint8_t *)outputBuffer)[i] = uart2.buffer_receive[uart2.tail_receive];
        //
        uart2.tail_receive ++;
        if(uart2.tail_receive >= UART2_BUFFER_SIZE_RECEIVE)
            uart2.tail_receive = 0;
    }
    //
    UART2_IRQ_ENABLE(ENABLE);
    //
    return returnLength;
}

void USART2_IRQHandler(void)
{
    // 发送完成。
    if(USART_GetFlagStatus(USART2,USART_FLAG_TC) == SET)
    {
        USART_ClearFlag(USART2,USART_FLAG_TC);
        //
        if(uart2_transmitBufferSizeOccupy() > 0)
            uart2_transmitAByte();
    }
    //
    // 接收到数据。
    if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == SET)
    {
        USART_ClearFlag(USART2,USART_FLAG_RXNE);
        //
        uint8_t data = (uint8_t)USART_ReceiveData(USART2);
        //
        uart2_receivePushToBuffer(data);
    }
}
