#ifndef LPI_LOCK_H
#define LPI_LOCK_H

#include "lpi_types.h"
/** 互斥锁
*1.默认是线程属性，如果是进程，需要设置属性值
*2.
**/
DECLARE_HANDLE(HMUTEX);

HMUTEX lpi_mutex_create();
void lpi_mutex_destroy(HMUTEX h);
void lpi_mutex_lock(HMUTEX h);
void lpi_mutex_trylock(HMUTEX h);
void lpi_mutex_unlock(HMUTEX h);

/** 条件变量
*
**/
DECLARE_HANDLE(HCOND);
HCOND lpi_cond_create();
void lpi_cond_destroy(HCOND h);
int lpi_cond_wait(HCOND cnd,HMUTEX mux,int time);
void lpi_cond_signal(HCOND cnd);

/** 读写锁
*
**/
DECLARE_HANDLE(HRWLOCK);
HRWLOCK lpi_rwlock_create();
void lpi_rwlock_destroy(HRWLOCK h);
void lpi_rwlock_wrlock(HRWLOCK h);
void lpi_rwlock_rdlock(HRWLOCK h);
void lpi_rwlock_unlock(HRWLOCK h);



#endif // LPI_LOCK_H
