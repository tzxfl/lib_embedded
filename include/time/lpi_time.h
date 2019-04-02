#ifndef LPI_TIME_H
#define LPI_TIME_H

#include "lpi_types.h"

/**
struct timeval
{
    time_t tv_sec; //since 1970-01-01 UTC ����ʱ��
    suseconds_t tv_usec;
};
time_t
struct tm

//��������ʱ�䵽tv��
int gettimeofday(struct timeval *tv,struct timezone *tz);
//����1970-01-01������
time_t time(time_t *timep);

//ʱ��ת��
char *ctime(const time_t *tm)//ת��Ϊ�ַ���
char *asctime(const struct tm*)

struct tm *gmtime(const time_t *)
struct tm *localtime(const time_t *)
time_t mktime(struct tm*)

//��struct tm��ʽ��ת��
size_t strftime(char *out,size_t max,const char *format,const struct tm *)



**/

/**
*@brief ��ȡ��ǰ��׼ʱ��
*@param
*@return ���ش�1970-1-1�����ڵ�ʱ��(΢��)
**/
u64 get_cur_time();

/**
*@brief ��ȡ��ǰ��ʽ��ʱ��
*@param
*   buf[out] ������-��-�� ʱ�����ʽʱ��
*@return
**/
int get_format_time(char *result,unsigned int result_size,char *format);



#endif // LPI_TIME_H
