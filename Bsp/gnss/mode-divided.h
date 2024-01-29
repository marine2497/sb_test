// gnss/mode-divided.h
// 2013-11-29 15:47:56
// yys@system-intergration-department

#ifndef __FILE_GNSS_MODE_DIVIDED_H__
#define __FILE_GNSS_MODE_DIVIDED_H__

#include <stdint.h>
#include "gnss.h"

void mode_divided_init(gnss_t * gnss);
int32_t mode_divided_line_master(const char *line);
int32_t mode_divided_line_rover(const char *line);

#endif // __FILE_GNSS_MODE_DIVIDED_H__
