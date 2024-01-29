
// 通信模块的接口。

#ifndef __FILE_COM_COM_H__
#define __FILE_COM_COM_H__

#include <stdint.h>

void com_init(void);
void com_check_event(void);
void A5com_directlink(char stat);
#endif // __FILE_COM_COM_H__
