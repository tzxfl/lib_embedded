#ifndef LPI_LOCK_H
#define LPI_LOCK_H

#include "lpi_types.h"
/** ������
*1.Ĭ�����߳����ԣ�����ǽ��̣���Ҫ��������ֵ
*2.
**/
DECLARE_HANDLE(HMUTEX);

HMUTEX lpi_mutex_create();
void lpi_mutex_destroy(HMUTEX h);
void lpi_mutex_lock(HMUTEX h);
void lpi_mutex_trylock(HMUTEX h);
void lpi_mutex_unlock(HMUTEX h);

/** ��������
*
**/
DECLARE_HANDLE(HCOND);
HCOND lpi_cond_create();
void lpi_cond_destroy(HCOND h);
int lpi_cond_wait(HCOND cnd,HMUTEX mux,int time);
void lpi_cond_signal(HCOND cnd);

/** ��д��
*
**/
DECLARE_HANDLE(HRWLOCK);
HRWLOCK lpi_rwlock_create();
void lpi_rwlock_destroy(HRWLOCK h);
void lpi_rwlock_wrlock(HRWLOCK h);
void lpi_rwlock_rdlock(HRWLOCK h);
void lpi_rwlock_unlock(HRWLOCK h);



#endif // LPI_LOCK_H
