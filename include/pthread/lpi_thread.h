#ifndef LPI_THREAD_H
#define LPI_THREAD_H

#include "lpi_types.h"

DECLAR_HANDLE(HTHREAD);

typedef void *(*thread_task)(void *);

/**
*@brief ����һ�����ɿ��Ƶ��߳�
*@param
*   priority ���ȼ�
*@return 1 �ɹ� -1ʧ��
**/
int thread_simple_create(thread_task task,void *arg,unsigned int stack_size,int priority);

/**
*@brief �����ɿ��߳�
*@param
**/
HTHREAD thread_create(thread_task task,void *arg,unsigned int stack_size,int priority);

/**
*@brief �ж��߳��Ƿ�������
*@param
**/
int thread_is_running(HTHREAD h);

/**
*@brief �ݻ��߳�
*/
void thread_destroy(HTHREAD h);

#endif // LPI_THREAD_H
