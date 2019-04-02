#include <sys/time.h>
#include <time.h>

#include "lpi_time.h"



/**
*@brief ��ȡ��ǰ��׼ʱ��
*@param
*@return ���ش�1970-1-1�����ڵ�ʱ��(΢��)
**/
u64 get_cur_time()
{
    struct timeval cur_tm;
    gettimeofday(&cur_tm,NULL);
    return (u64)cur_tm.tv_sec *1000000 + cur_tm.tv_usec;
}

/**
*@brief ��ȡ��ǰ��ʽ��ʱ��
*@param
*   buf[out] ������-��-�� ʱ�����ʽʱ��
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

