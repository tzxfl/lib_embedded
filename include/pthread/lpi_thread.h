#ifndef LPI_THREAD_H
#define LPI_THREAD_H

#include "lpi_types.h"

DECLAR_HANDLE(HTHREAD);

typedef void *(*thread_task)(void *);

/**
*@brief 创建一个不可控制的线程
*@param
*   priority 优先级
*@return 1 成功 -1失败
**/
int thread_simple_create(thread_task task,void *arg,unsigned int stack_size,int priority);

/**
*@brief 创建可控线程
*@param
**/
HTHREAD thread_create(thread_task task,void *arg,unsigned int stack_size,int priority);

/**
*@brief 判断线程是否在运行
*@param
**/
int thread_is_running(HTHREAD h);

/**
*@brief 摧毁线程
*/
void thread_destroy(HTHREAD h);

#endif // LPI_THREAD_H
