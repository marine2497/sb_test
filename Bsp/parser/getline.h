// getline.h


// 从数据流中分段成行�?
#ifndef __FILE_GETLINE_H__
#define __FILE_GETLINE_H__

#include <stdint.h>

typedef void (*getline_cb)(char *line,int32_t length,void *param);

typedef struct
{
    char      * buffer;
    int32_t     buffer_length;
    int32_t     buffer_index;
    getline_cb  callback;
    void      * param;
    char        line_seperator;
}getline_t;

void getline_init(getline_t *getline,void *line_buffer,int32_t buffer_length,getline_cb cb,void *param,char seperator);
void getline_input(getline_t *getline,const char *stream,int32_t length);
void getline_input_modbus_rtu(getline_t *getline,const char *stream,int32_t length);
void getline_modbus_rtu_go(getline_t *getline);
#endif // __FILE_GETLINE_H__
