// gphpd.c
// 2013-11-29 16:36:37
// yys@system-intergration-department

#include "gphdt.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int32_t gphdt_is(const char *line)
{
    return strncmp(line,"$GPHDT,",7) == 0 ||strncmp(line,"$GNHDT,",7) == 0||strncmp(line,"$HEHDT,",7) == 0;
}
//$GPHDT,196.67,T*0A
int32_t gphdt_parser(gphdt_t *gphdt,const char *line)
{
     memset(gphdt,0,sizeof(*gphdt));
    //
    char ** section = 0;
    int32_t section_amount = parser_cut_section(line,&section);
    //
    if(section_amount != 4 || (strcmp(section[1-1],"$GPHDT") != 0 && strcmp(section[1-1],"$GNHDT") != 0&& strcmp(section[1-1],"$HEHDT") != 0))
        return -1;
 
    //
    gphdt->heading       = atof(section[2-1]);
    gphdt->valid        = section[3-1][0] == 'T' ? 1 : 0; 
   
    return 0;
}

int32_t gphdt_print(char *line,const gphdt_t *gphdt)
{
    char * str = line;
    //
    str += sprintf(str,
        "$GPHDT,%.3f,T",gphdt->heading);
    //    
    uint16_t crc = parser_get_checksum0183(line);
    str += sprintf(str, "*%X%X\r\n", crc>>4,crc&0xF);
    //
    return (str - line);
}
