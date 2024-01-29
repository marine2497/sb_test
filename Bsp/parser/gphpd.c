// gphpd.c
// 2013-11-29 16:36:37
// yys@system-intergration-department

#include "gphpd.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t gphpd_is(const char *line)
{
    return strncmp(line,"$GPHPD,",7) == 0;
}

int32_t gphpd_parser(gphpd_t *gphpd,const char *line)
{
    return -1;
}

int32_t gphpd_print(char *line,const gphpd_t *gphpd)
{
    char * str = line;
    //
    str += sprintf(str,
        "$GPHPD,%d,%.3f,%0.2f,%0.2f,%0.2f,%0.7f,%0.7f,%0.3f,%0.3f,"
        "%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%d,%d,%d",
        gphpd->gpsweek, gphpd->gpstime, gphpd->heading, gphpd->pitch, gphpd->roll,
        gphpd->latitude ,gphpd->longitude, gphpd->altitude, gphpd->x,gphpd->y, gphpd->z, 
        gphpd->ve, gphpd->vn, gphpd->vu, gphpd->ae, gphpd->an, gphpd->au, gphpd->baseline, 
        gphpd->nsv1, gphpd->nsv2, gphpd->valid);
    //    
    uint16_t crc = parser_get_checksum0183(line);
    str += sprintf(str, "*%X%X\r\n", crc>>4,crc&0xF);
    //
    return (str - line);
}
