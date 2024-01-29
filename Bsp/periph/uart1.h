// uart1.h


#ifndef __FILE_UART1_H__
#define __FILE_UART1_H__

#include <stdint.h>

enum
{
    UART1_SUCCEED = 0,
    UART1_FAILED  = 1
};

void uart1_init(void);
int32_t uart1_transmit(const void *data,int32_t len);
int32_t uart1_receiveValidBufferSize(void);
int32_t uart1_readReceiveBuffer(void *outputBuffer,int32_t outputBufferLength);

#endif // __FILE_UART1_H__
