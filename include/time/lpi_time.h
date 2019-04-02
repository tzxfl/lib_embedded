#ifndef LPI_TIME_H
#define LPI_TIME_H

#include "lpi_types.h"

/**
struct timeval
{
    time_t tv_sec; //since 1970-01-01 UTC 日历时间
    suseconds_t tv_usec;
};
time_t
struct tm

//返回日历时间到tv中
int gettimeofday(struct timeval *tv,struct timezone *tz);
//返回1970-01-01的秒数
time_t time(time_t *timep);

//时间转换
char *ctime(const time_t *tm)//转换为字符串
char *asctime(const struct tm*)

struct tm *gmtime(const time_t *)
struct tm *localtime(const time_t *)
time_t mktime(struct tm*)

//将struct tm格式化转换
size_t strftime(char *out,size_t max,const char *format,const struct tm *)



**/

/**
*@brief 获取当前精准时间
*@param
*@return 返回从1970-1-1到现在的时间(微秒)
**/
u64 get_cur_time();

/**
*@brief 获取当前格式化时间
*@param
*   buf[out] 返回年-月-日 时分秒格式时间
*@return
**/
int get_format_time(char *result,unsigned int result_size,char *format);



#endif // LPI_TIME_H
