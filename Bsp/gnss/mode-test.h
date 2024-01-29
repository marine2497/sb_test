// gnss/mode-divided.h


#ifndef __FILE_GNSS_MODE_DIVIDED_H__
#define __FILE_GNSS_MODE_DIVIDED_H__

#include <stdint.h>
#include "gnss.h"

void mode_test_init(gnss_t * gnss);
int32_t mode_test_line_master(const char *line);
int32_t mode_test_line_rover(const char *line);

#endif // __FILE_GNSS_MODE_DIVIDED_H__
