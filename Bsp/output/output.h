// output/output.h
// 2013-11-29 19:15:04
// yys@system-intergration-department

#ifndef __FILE_OUTPUT_OUTPUT_H__
#define __FILE_OUTPUT_OUTPUT_H__

#include <stdint.h>
#include "gnss.h"

#define OUTPUT_TYPE_NO_OUTPUT     0 // 没有输出的状态值。
#define OUTPUT_BIT_SOUTH          0 // 输出南方格式的位位置。
#define OUTPUT_BIT_XING_WANG      1 // 输出星网格式的位位置。
#define OUTPUT_BIT_MASTER_GPGGA   2 // 输出星网格式的位位置。
#define	OUTPUT_BIT_COMPASS_GPHDT  3 // 输出电子罗经的GHPDT格式的位位置
#define	OUTPUT_BIT_MASTER_GPGSV   4 // 输出后主板的gpgsv星图
#define	OUTPUT_BIT_D5EA_0x0400    5 // 沈阳低精度主板单天线方式
#define OUTPUT_BIT_TEST           6 // 测试用的报文

#define OUTPUT_BIT_VALUE_SOUTH          (1 << OUTPUT_BIT_SOUTH         )
#define OUTPUT_BIT_VALUE_XING_WANG      (1 << OUTPUT_BIT_XING_WANG     )
#define OUTPUT_BIT_VALUE_MASTER_GPGGA   (1 << OUTPUT_BIT_MASTER_GPGGA  )
#define	OUTPUT_BIT_VALUE_COMPASS_GPHDT  (1 << OUTPUT_BIT_COMPASS_GPHDT )
#define	OUTPUT_BIT_VALUE_MASTER_GPGSV   (1 << OUTPUT_BIT_MASTER_GPGSV  )
#define	OUTPUT_BIT_VALUE_D5EA_0x0400    (1 << OUTPUT_BIT_D5EA_0x0400   )
#define	OUTPUT_BIT_VALUE_TEST           (1 << OUTPUT_BIT_TEST          )

#define output_is_bit_set(type,bit) (((type) >> (bit)) & 0x01)
#define output_is_bits_set(type,bits) ((type) & (bits))
//#define output_after_set_bit(type,bit)    (type |  (((uint64_t)1) >> (bit)) )
//#define output_after_reset_bit(type,bit)  (type & ~(((uint64_t)1) >> (bit)) )

void output_init(void);
void output_check_event(void);
uint64_t output_current_type(void);
int32_t output_set_type(uint64_t new_type);
void output_triger(const gnss_t *gnss);
void output_triger_refstation(const gnss_t *gnss);
void output_silent(int32_t yes);
void output_test(int32_t yes);

#endif // __FILE_OUTPUT_OUTPUT_H__
