
#include "datetime.h"
#include <time.h>
#include <stdio.h>

/*************************************
 * 函数名称：Get_UTCTime
 * 作者：GNSS-LINING
 * 时间：2010-5-29
 * 入口参数：指向存放时间的GNSSTime结构体
 * 出口参数：无
 * 函数功能：利用GPS周数与周秒计算出当前时间
 * 历史记录：
 **************************************/
void datetime_fromWeekSecond(datetime_t *datetime,uint32_t week,double second)
{
	int day_tab[2][13] =
	{
			{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
			{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};
    int32_t   lp = 0;
    int32_t day_l = 0;
    int32_t  day_t = 0;
    uint32_t i;
	day_t = week*7+(((int32_t)(second))/(24*3600));//计算出从1980.01.06到现在共有多少天
	day_l = day_t%7;//整除周后余数，就是当前时间是星期几
	datetime->year = (uint32_t)(day_t+1)/365+1980;//计算出年
	day_t += 6;
	for (i=1980; i<datetime->year; i++)
	{
		lp = ((i%4==0 && i%100!=0) || (i%400==0));
		day_t -= 365+lp;
	}
	if (day_t<=0)
	{
		day_t = day_t+365+lp;
		datetime->year--;
	}
	datetime->day = day_t;
	lp = ((datetime->year%4==0 && datetime->year%100!=0) || (datetime->year%100==0));

	for (i=1; i<=12; i++)
	{
		day_t -= day_tab[lp][i];
		if (day_t<=0)
		{
			datetime->month = i;
			day_t += day_tab[lp][i];

			break;
		}
	}
	datetime->day = day_t;//日
	datetime->hour = (uint8_t)(second/3600)-day_l*24;
	datetime->minute = (uint8_t)(second/60-day_l*24*60-datetime->hour*60);
	datetime->second = (second-datetime->minute*60-datetime->hour*3600-day_l*24*3600);
}

// 比较时间。
// 返回值：
//  -1 => left <  right
//   0 => left == right
//   1 => left >  right
int32_t datetime_compare(const datetime_t *left,const datetime_t *right)
{
    // 先判断年，不相等就可以得出结果了。
    if(left->year  > right->year)
        return 1;
    if(left->year  < right->year)
        return -1;
    //
    // 如果年相等就判断月。
    if(left->month > right->month)
        return 1;
    if(left->month < right->month)
        return -1;
    //
    // 年月都相等才判断日。
    if(left->day   > right->day)
        return 1;
    if(left->day   < right->day)
        return -1;
    //
    // 年月日都相等就判断小时。
    if(left->hour  > right->hour)
        return 1;
    if(left->hour  < right->hour)
        return -1;
    //
    // 连小时都相等，判断分钟。
    if(left->minute > right->minute)
        return 1;
    if(left->minute < right->minute)
        return -1;
    //
    // 最后判断秒。
    if(left->second > right->second)
        return 1;
    if(left->second  < right->second)
        return -1;
    //
    // 相等。
    return 0;
}

void datetime_set(datetime_t *dt,uint32_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,float second)
{
    dt->year    = year  ;
    dt->month   = month ;
    dt->day     = day   ;
    dt->hour    = hour  ;
    dt->minute  = minute;
    dt->second  = second;
}

void datetime_set_to_19700101(datetime_t *dt)
{
    dt->year    = 1970;
    dt->month   = 1;
    dt->day     = 1;
    dt->hour    = 0;
    dt->minute  = 0;
    dt->second  = 0;
}

//utc转北京时间
void datetime_utc2beijing(datetime_t *dt)
{
	dt->hour += 8;  // 格林威治时间 + 8 小时 = 北京时间
	if( dt->hour < 24 ) return; // 没有跨天，则计算完成
	
	// 下面是跨天后的计算 
	dt->hour -= 24;
	dt->day ++;        //日期加一天
	switch( dt->month ) //按月判断
	{
		case 4: // 跨小月的判断
		case 6:  
		case 9: 
		case 11:
		if( dt->day > 30 ) 
		{
			dt->day = 1;    
			dt->month ++;
        }break;
									   
		case 1:// 跨大月的判断
		case 3:         
    case 5:
		case 7:
    case 8: 
		case 10:
		if( dt->day > 31 ) 
		{
			dt->day = 1;    
			dt->month ++;
		}break;

    case 12:// 12 月，要判断是否跨年                        
		if( dt->day > 31 )
		{
			dt->year ++;    
			dt->day = 1;   
			dt->month = 1;
		}
		break;

 		case 2:// 2 月，要判断是否是闰年
		if( ( (dt->year+2000)%400 == 0 ) ||  // 能被400整除，一定是闰年

            (( (dt->year+2000)%4 ==0 ) && ( (dt->year+2000)%100 !=0 ) ) ) // 能被4整除，但不能被100整除，一定是闰年

    {
			if( dt->day>29 )    // 闰年2月，可以有29号
			{
				 dt->month = 3;    
				 dt->day = 1;
			}
   }
	 else if( dt->day>28 )    // 非闰年2月，可以有28号
	 {
			dt->month = 3; 
			dt->day = 1;
	 }
	 break;

  }//end swtich
}

void datetime_sprintf(const datetime_t *dt,char *str)
{
    sprintf(str,"%04d-%02d-%02d %02d:%02d:%04.1f",dt->year,dt->month,dt->day,dt->hour,dt->minute,dt->second);
}

void datetime_printf(const datetime_t *dt)
{
    printf("%04d-%02d-%02d %02d:%02d:%04.1f",dt->year,dt->month,dt->day,dt->hour,dt->minute,dt->second);
}




//#define __TEST_DATETIME__
#ifdef  __TEST_DATETIME__

#include <stdio.h>

int main(void)
{
    struct
    {
        uint32_t week;
        double second;
        const char *desc;
    }tab[] =
    {
        { 1769 , 262181.200 , "2013-12-04 1:xx" },
        { 1456 , 4*24*60*60 , "2007-12-6" },
        { 1457 , 0*24*60*60 , "2007-12-9" },
        { 1503 , 6*24*60*60 , "2008-11-1" },
        { 1504 , 2*24*60*60 , "2008-11-4" },
        { 1740 , 3*24*60*60 , "2013-05-15" },
    };
    //
    for(int i=0;i<sizeof(tab)/sizeof(tab[0]);i++)
    {
        datetime_t d;
        datetime_fromWeekSecond(&d,tab[i].week,tab[i].second);
        datetime_printf(&d);
        printf(" === %s\n",tab[i].desc);
    }
    //
    datetime_t d;
    datetime_set(&d,2014,7,2,12,03,54.21);
    datetime_printf(&d);
    printf(" utc2beijing => ");
    datetime_utc2beijing(&d);
    datetime_printf(&d);
    printf("\n");
    datetime_set(&d,2014,7,2,16,03,54.21);
    datetime_printf(&d);
    printf(" utc2beijing => ");
    datetime_utc2beijing(&d);
    datetime_printf(&d);
    printf("\n");
    datetime_set(&d,2012,2,28,19,02,57.21);
    datetime_printf(&d);
    printf(" utc2beijing => ");
    datetime_utc2beijing(&d);
    datetime_printf(&d);
    printf("\n");
    //
    return 0;
}

#endif // __TEST_DATETIME__

