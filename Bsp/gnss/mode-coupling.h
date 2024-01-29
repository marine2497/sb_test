// gnss/mode-coupling.h
// 2013-12-2 8:43:30
// yys@system-intergration-department

#ifndef __FILE_GNSS_MODE_COUPLING_H__
#define __FILE_GNSS_MODE_COUPLING_H__

#include <stdint.h>
#include "gnss.h"

void mode_coupling_init(gnss_t * gnss);
int32_t mode_coupling_line_master(const char *line);
int32_t mode_coupling_line_rover(const char *line);

#endif // __FILE_GNSS_MODE_COUPLING_H__
