// 2013-12-9 15:03:14
// yys@system-intergration-department

#ifndef __FILE_SECURITY_SECURITY_H__
#define __FILE_SECURITY_SECURITY_H__

#include <stdint.h>
#include "structure.h"

void security_init(void);
void security_check_event(void);
void security_setup_dog(float timeout_s);

#endif // __FILE_SECURITY_SECURITY_H__

