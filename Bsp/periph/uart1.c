// uart1.c
// 2012-12-4 19:13:01
// js200300953

#include <stdio.h>
#include "uart1.h"
#include "stm32f10x.h"

#define UART1_BUFFER_SIZE_TRANSMIT 512
#define UART1_BUFFER_SIZE_RECEIVE 512

#define UART1_IRQ_ENABLE(enable) \
    USART_ITConfig(USART1,USART_IT_TC,enable); \
    USART_ITConfig(USART1,USART_IT_RXNE,enable); \

static struct 
{
    volatile uint32_t head_transmit;
    volatile uint32_t tail_transmit;
    volatile uint32_t head_receive;
    volatile uint32_t tail_receive;
    volatile uint8_t buffer_transmit[UART1_BUFFER_SIZE_TRANSMIT];
    volatile uint8_t buffer_receive[UART1_BUFFER_SIZE_RECEIVE];
}uart1;

void uart1_init(void);
int32_t uart1_transmit(const void *data,int32_t len);
int32_t uart1_receiveValidBufferSize(void);
int32_t uart1_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);
//
static uint32_t uart1_transmitBufferSizeIdle(void);
static uint32_t uart1_transmitBufferSizeOccupy(void);
static void uart1_transmitAByte(void);
static uint8_t uart1_receivePushToBuffer(uint8_t data);

void uart1_init(void)
{
    // 设置变量
    uart1.head_transmit = 0;
    uart1.tail_transmit = 0;
    uart1.head_receive = 0;
    uart1.tail_receive = 0;
    //
    // 接通时钟�?    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    //
    // 重置串口�?    
    USART_DeInit(USART1);
    //
    // 设置端口�? 	
    GPIO_InitTypeDef pin_param;
    GPIO_StructInit(&pin_param);
    pin_param.GPIO_Mode     = GPIO_Mode_AF_PP; 	
    pin_param.GPIO_Speed    = GPIO_Speed_10MHz;
    pin_param.GPIO_Pin = GPIO_Pin_9 ;// A9:发送引脚，A10:接收引脚�?  	
    GPIO_Init(GPIOA, &pin_param);
        
        /* Configure USART Rx as input floating */
    pin_param.GPIO_Pin = GPIO_Pin_10;
    pin_param.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &pin_param);
        
    //
    // 映射引脚�?    
   // GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
   // GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
    // // ���ô��ڡ�
    USART_InitTypeDef init_param;
    USART_StructInit(&init_param);
    init_param.USART_BaudRate = 19200;//115200;
    init_param.USART_WordLength = USART_WordLength_8b;
    init_param.USART_StopBits = USART_StopBits_1;
    init_param.USART_Parity = USART_Parity_No;
    init_param.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    init_param.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1,&init_param);
    //
    // �����жϡ� 
    UART1_IRQ_ENABLE(ENABLE);
    //
    NVIC_InitTypeDef nvicParam;
    nvicParam.NVIC_IRQChannel = USART1_IRQn;
    nvicParam.NVIC_IRQChannelPreemptionPriority = 0;
    nvicParam.NVIC_IRQChannelSubPriority = 4;;
    nvicParam.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicParam);
    //
    // �������ڡ�
    USART_Cmd(USART1,ENABLE);
}

// ��ȡ���ͻ��������пռ��С��
uint32_t uart1_transmitBufferSizeIdle(void)
{
	if(uart1.tail_transmit <= uart1.head_transmit)
		return UART1_BUFFER_SIZE_TRANSMIT - uart1.head_transmit + uart1.tail_transmit;
        return uart1.tail_transmit - uart1.head_transmit - 1; // -1:������������
}

// ��ȡ���ͻ������е����ݳ��ȡ�
uint32_t uart1_transmitBufferSizeOccupy(void)
{
	if(uart1.tail_transmit <= uart1.head_transmit)
		return uart1.head_transmit - uart1.tail_transmit;
	return uart1.head_transmit + (UART1_BUFFER_SIZE_TRANSMIT - uart1.tail_transmit);
}

// �ӷ��ͻ�����ȡ��һ���ֽڣ����ͳ�ȥ��
// ����黺�����Ƿ�գ�����鷢��״̬��
void uart1_transmitAByte(void)
{
    USART_SendData(USART1,uart1.buffer_transmit[uart1.tail_transmit]);
	uart1.tail_transmit ++;
	if(uart1.tail_transmit >= UART1_BUFFER_SIZE_TRANSMIT)
		uart1.tail_transmit = 0;
}

// 把一段数据放入发送缓冲区�?
// 返回值：{UART1_SUCCEED,UART1_FAILED}
int32_t uart1_transmit(const void * data,int32_t len)
{
    UART1_IRQ_ENABLE(DISABLE);
    //
	if(uart1_transmitBufferSizeIdle() < len)
	{
        UART1_IRQ_ENABLE(ENABLE);
		return 0; // 空间不够�?	
        }
	//
	for(int i=0;i<len;i++)
	{
		uart1.buffer_transmit[uart1.head_transmit] = ((uint8_t *)data)[i];
		//
		uart1.head_transmit ++;
		if(uart1.head_transmit >= UART1_BUFFER_SIZE_TRANSMIT)
			uart1.head_transmit = 0;
		//
	}
	//
	// 如果发送没有正在进行，就启动发送�?	
        if(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == SET)
        uart1_transmitAByte();
    //
    UART1_IRQ_ENABLE(ENABLE);
	//
  return len;
}

// 获取接收缓冲区中的数据长度�?
int32_t uart1_receiveValidBufferSize(void)
{
	if(uart1.tail_receive <= uart1.head_receive)
		return uart1.head_receive - uart1.tail_receive;
	return uart1.head_receive + (UART1_BUFFER_SIZE_RECEIVE - uart1.tail_receive);
}

// 把一个字节放入接收缓冲区�?
// 缓冲区满则失败�?
// 返回值：{UART1_SUCCEED,UART1_FAILED}�?
uint8_t uart1_receivePushToBuffer(uint8_t data)
{
    uint32_t newHead = uart1.head_receive + 1;
	if(newHead >= UART1_BUFFER_SIZE_RECEIVE)
		newHead = 0;
    //
    if(newHead == uart1.tail_receive)
        return UART1_FAILED;
    //
    uart1.buffer_receive[uart1.head_receive] = data;
    uart1.head_receive = newHead;
    //
    return UART1_SUCCEED;
}

// 从接收缓冲区取出数据，返回取出的长度�?
// 取出的长度为outputBufferLength和uart1_receiveValidBufferSize()的最小者�?
int32_t uart1_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength)
{
    UART1_IRQ_ENABLE(DISABLE);
    //
    // 计算outputBufferLength和uart1_receiveValidBufferSize()的最小值�?    
    uint32_t returnLength = uart1_receiveValidBufferSize();
    if(outputBufferLength < returnLength)
        returnLength = outputBufferLength;
    //
    // 复制数据，推进指针�?    
    for(uint32_t i=0;i<returnLength;i++)
    {
        ((uint8_t *)outputBuffer)[i] = uart1.buffer_receive[uart1.tail_receive];
        //
        uart1.tail_receive ++;
        if(uart1.tail_receive >= UART1_BUFFER_SIZE_RECEIVE)
            uart1.tail_receive = 0;
    }
    //
    UART1_IRQ_ENABLE(ENABLE);
    //
    return returnLength;
}

void USART1_IRQHandler(void)
{
    // 发送完成�?    
  if(USART_GetFlagStatus(USART1,USART_FLAG_TC) == SET)
    {
        USART_ClearFlag(USART1,USART_FLAG_TC);
        //
        if(uart1_transmitBufferSizeOccupy() > 0)
            uart1_transmitAByte();
    }
    //
    // 接收到数据�?    
  if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
    {
        USART_ClearFlag(USART1,USART_FLAG_RXNE);
        //
        uint8_t data = (uint8_t)USART_ReceiveData(USART1);
        //
        uart1_receivePushToBuffer(data);
    }
}

//int fputc(int ch, FILE *f)
//{
//    uint8_t tmp = ch;
//    uart1_transmit(&tmp,1);
//    //
//    return 0;
//}
