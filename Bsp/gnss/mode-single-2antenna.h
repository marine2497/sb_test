// gnss/mode-single-2antenna.h
// 2014-9-11 10:26:56
// yys@system-intergration-department

#ifndef __FILE_GNSS_MODE_SINGLE_2ANTENNA_H__
#define __FILE_GNSS_MODE_SINGLE_2ANTENNA_H__

#include <stdint.h>
#include "gnss.h"

void mode_single_2antenna_init(gnss_t * gnss);
int32_t mode_single_2antenna_line_master(const char *line);
int32_t mode_single_2antenna_line_rover(const char *line);

#endif // __FILE_GNSS_MODE_SINGLE_2ANTENNA_H__
