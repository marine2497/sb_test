// math/datetime.h


#ifndef __FILE_MATH_DATETIME_H__
#define __FILE_MATH_DATETIME_H__

#include <stdint.h>

// 时间。
typedef struct
{
	uint32_t year;   // 年份，eg:2013。
	uint8_t  month;  // 月份，十二月为12。
	uint8_t  day;    // 月中的日。
	uint8_t  hour;   // 时，[0,23]
	uint8_t  minute; // 分，[0,59]
	float    second; // 秒，[0,60)
} datetime_t;


void    datetime_fromWeekSecond(datetime_t *datetime,uint32_t week,double second);
int32_t datetime_compare(const datetime_t *left,const datetime_t *right);
void    datetime_set(datetime_t *dt,uint32_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,float second);
void    datetime_set_to_19700101(datetime_t *dt);
void    datetime_utc2beijing(datetime_t *dt);
void    datetime_sprintf(const datetime_t *dt,char *str);
void    datetime_printf(const datetime_t *dt);

#endif // __FILE_MATH_DATETIME_H__
