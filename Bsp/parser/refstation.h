// parser/refstation.h
// 2014-7-2 15:15:12
// yys@system-intergration-department

#ifndef __FILE_APP_PARSER_REFSTATION_H__
#define __FILE_APP_PARSER_REFSTATION_H__

#include <stdint.h>

typedef struct
{
    double x;
    double y;
    double z;
    uint32_t status; // bit0 => {0:valid, 1:invalid}
    uint32_t health;
    //
    uint32_t valid; // status的bit0的反。
} refstation_t;

int32_t refstation_is(const char *line);
int32_t refstation_parser(refstation_t *refstation,const char *line);


#endif // __FILE_APP_PARSER_REFSTATION_H__
