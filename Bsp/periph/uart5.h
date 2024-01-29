// uart3.h


#ifndef __FILE_UART5_H__
#define __FILE_UART5_H__

#include "stm32f10x.h"

enum
{
    UART5_SUCCEED = 0,
    UART5_FAILED  = 1
};

void uart5_init(void);
int32_t uart5_transmit(const void *data,int32_t len);
int32_t uart5_receiveValidBufferSize(void);
int32_t uart5_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);

#endif // __FILE_UART3_H__
