
#include "stdio.h"
#include "parser.h"
#include "getline.h"

void getline_init(getline_t *getline,void *line_buffer,int32_t buffer_length,getline_cb cb,void *param,char seperator)
{
    getline->buffer = (char *)line_buffer;
    getline->buffer_length = buffer_length;
    getline->callback = cb;
    getline->param = param;
    getline->line_seperator = seperator;
    //
    getline->buffer_index = 0;
}

void getline_input(getline_t *getline,const char *stream,int32_t length)
{
    for(int i=0;i<length;i++)
    {
        if(getline->buffer_index < getline->buffer_length-1)
        {
           
            getline->buffer[getline->buffer_index++] = stream[i];
        }
        //
        if(stream[i] == getline->line_seperator)
        {
            getline->buffer[getline->buffer_index] = '\0';
            getline->callback(getline->buffer,getline->buffer_index,getline->param);
            //
            getline->buffer_index = 0;
        }
    }
}

void getline_input_modbus_rtu(getline_t *getline,const char *stream,int32_t length)
{
    for(int i=0;i<length;i++)
    {
        if(getline->buffer_index < getline->buffer_length-1)
        {
           
            getline->buffer[getline->buffer_index++] = stream[i];
        }
        
        if(parser_modbus_rtu(getline->buffer, getline->buffer_index)==1)
        {
            getline->buffer[getline->buffer_index] = '\0';
            getline->callback(getline->buffer,getline->buffer_index,getline->param);
            //
            getline->buffer_index = 0;
         }
    }
    //for(int i=0;i<getline->buffer_index;i++)
    //    printf("%x ",getline->buffer[i]);
    //printf("\r\n");
}

//#define TEST_GETLINE
#ifdef TEST_GETLINE

#include <stdio.h>
#include <string.h>

static const char *g_data = \
        "ok\r\n"
        "line2\r\n"
        "line3\n"
        "fff\r\n"
        "abcdefghijk\r\n"
        "\n"
        "endl\r\n";
getline_t g_getline[1];
char line_buffer[8];

void line_cb(char *line,int32_t length,void *param)
{
    printf("p=%d\n",param);
    printf(line);
    printf("\n");
}

int main(void)
{
    int32_t data_len = strlen(g_data);
    //
    getline_init(g_getline,line_buffer,8,line_cb,0,'\n');
    for(int32_t i=0;i<data_len;i++)
    {
        getline_input(g_getline,g_data+i,1);
    }
    //
    return 0;
}

#endif // TEST_GETLINE
