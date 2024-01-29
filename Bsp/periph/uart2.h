// uart2.h
// 2012-12-4 19:13:19
// js200300953

#ifndef __FILE_UART2_H__
#define __FILE_UART2_H__

#include "stm32f10x.h"

enum
{
    UART2_SUCCEED = 0,
    UART2_FAILED  = 1
};

void uart2_init(void);
int32_t uart2_transmit(const void *data,int32_t len);
int32_t uart2_receiveValidBufferSize(void);
int32_t uart2_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);

#endif // __FILE_UART2_H__
