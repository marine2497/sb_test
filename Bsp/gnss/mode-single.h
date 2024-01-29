// gnss/mode-single.h
// 2013-12-25 18:08:30
// yys@system-intergration-department

#ifndef __FILE_GNSS_MODE_SINGLE_H__
#define __FILE_GNSS_MODE_SINGLE_H__

#include <stdint.h>
#include "gnss.h"

void mode_single_init(gnss_t * gnss);
int32_t mode_single_line_master(const char *line);
int32_t mode_single_line_rover(const char *line);

#endif // __FILE_GNSS_MODE_SINGLE_H__
