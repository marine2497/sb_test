// gnss/mode-single-base.c
// 2014-2-11 09:46:30
// YYS@system-intergration-department

// 单块主板作基准站的模式。

#ifndef __FILE_GNSS_MODE_SINGLE_BASE_H__
#define __FILE_GNSS_MODE_SINGLE_BASE_H__

#include <stdint.h>
#include "gnss.h"

void mode_single_base_init(gnss_t * gnss);
int32_t mode_single_base_line_master(const char *line);
int32_t mode_single_base_line_rover(const char *line);

#endif // __FILE_GNSS_MODE_SINGLE_H__
