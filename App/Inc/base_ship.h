
#ifndef __FILE_BASE_SHIP_H__
#define __FILE_BASE_SHIP_H__
#include <stdint.h>



void recv_telecontroller_information(void);
void base_mode_init(void);
void base_go_code(void);
uint8_t base_read_remote_controllor(void);

int32_t vscbs_parser(const char *line);//岸上接收
int32_t vscbs_is(const char *line);

int32_t vsapid_is(const char *line);
int32_t vsapid_parser(const char *line);//岸上接收
#endif // __FILE_GNSS_MODE_DIVIDED_H__
