#include <sys/time.h>
#include <time.h>

#include "lpi_time.h"



/**
*@brief 获取当前精准时间
*@param
*@return 返回从1970-1-1到现在的时间(微秒)
**/
u64 get_cur_time()
{
    struct timeval cur_tm;
    gettimeofday(&cur_tm,NULL);
    return (u64)cur_tm.tv_sec *1000000 + cur_tm.tv_usec;
}

/**
*@brief 获取当前格式化时间
*@param
*   buf[out] 返回年-月-日 时分秒格式时间
*@return
**/
int get_format_time(char *result,unsigned int result_size,char *format)
{
    if(!result)
        return ;

    struct tm *t;
    time_t tt;

    time(&tt);
    t = localtime(&tt);
    if(t == NULL)
        return ;

    size_t size = strftime(result,result_size,(format != NULL)?format:"%c",t);
    return size;
}

