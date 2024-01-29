// uart3.h


#ifndef __FILE_UART4_H__
#define __FILE_UART4_H__

#include "stm32f10x.h"

enum
{
    UART4_SUCCEED = 0,
    UART4_FAILED  = 1
};

void uart4_init(void);
int32_t uart4_transmit(const void *data,int32_t len);
int32_t uart4_receiveValidBufferSize(void);
int32_t uart4_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);

#endif // __FILE_UART3_H__
