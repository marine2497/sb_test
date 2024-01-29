// parser/parser.h
// 2013-11-29 17:01:52
// yys@system-intergration-department

// 通用函数�?
#ifndef __FILE_PARSER_H__
#define __FILE_PARSER_H__

#include <stdint.h>

#define PARSER_PRINT_BUFFER_SIZE 1024
extern char g_parser_print_buffer[PARSER_PRINT_BUFFER_SIZE];

int32_t parser_check_line(const char * line,int32_t length);
uint8_t parser_get_checksum0183(const char *str);
uint32_t parser_get_crc32(const char *str);
int32_t parser_cut_section(const char * line, char ***sections_ptr);
uint32_t parser_str2utc(const char * utc_str);
unsigned short parser_get_crc16(char *puchMsg, unsigned short usDataLen);
int parser_modbus_rtu(char *puchMsg, unsigned short usDataLen);
#endif // __FILE_PARSER_H__
