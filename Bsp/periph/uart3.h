// uart3.h


#ifndef __FILE_UART3_H__
#define __FILE_UART3_H__

#include "stm32f10x.h"

enum
{
    UART3_SUCCEED = 0,
    UART3_FAILED  = 1
};

void uart3_init(void);
int32_t uart3_transmit(const void *data,int32_t len);
int32_t uart3_receiveValidBufferSize(void);
int32_t uart3_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);

#endif // __FILE_UART3_H__
