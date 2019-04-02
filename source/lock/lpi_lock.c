#include "lpi_lock.h"

#ifndef WIN32
#define T_MUTEX pthread_mutex_t
#define MUTEX_INIT(m) pthread_mutex_init(m,NULL)
#define MUTEX_UNINIT(m) pthread_mutex_destroy(m)
#define MUTEX_LOCK(m)   pthread_mutex_lock(m)
#define MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#endif

HMUTEX lpi_mutex_create()
{
    T_MUTEX *m = (T_MUTEX *)malloc(sizeof(T_MUTEX));
	MUTEX_INIT(m);
	return (HMUTEX)m;
}


void lpi_mutex_destroy(HMUTEX h)
{
	T_MUTEX *m = (T_MUTEX *)h;
	MUTEX_UNINIT(m);
	free(m);
}

void lpi_mutex_lock(HMUTEX h)
{
	T_MUTEX *m = (T_MUTEX *)h;
	MUTEX_LOCK(m);
}

void lpi_mutex_unlock(HMUTEX h)
{
	T_MUTEX *m = (T_MUTEX *)h;
	MUTEX_UNLOCK(m);
}

#ifndef WIN32
#define T_COND                  pthrad_cond_t
#define COND_INIT(m)			pthread_cond_init(m,NULL)
#define COND_UNINIT(m)			pthread_cond_destroy(m)
#define COND_WAIT(m,n)			pthread_cond_wait(m,n)
#define COND_TIMEWAIT(m,n,t)	pthread_cond_timedwait(m,n,t)
#define COND_SIGNAL(m)			pthread_cond_signal(m)
#endif // WIN32


HCOND lpi_cond_create()
{
	T_COND *m = (T_COND *)malloc(sizeof(T_COND));

	COND_INIT(m);
	return (HCOND)m;
}

void lpi_cond_destroy(HCOND h)
{
	T_COND *m = (T_COND *)h;

	while(EBUSY == COND_UNINIT(m));
	free(m);
}

int lpi_cond_wait(HCOND cnd, HMUTEX mtx)
{
	T_COND *m  = (T_COND *)cnd;
	T_MUTEX *n = (T_MUTEX *)mtx;
	return COND_WAIT(m,n);
}

int lpi_cond_timewait(HCOND cnd, HMUTEX mtx, int timeout)
{
	T_COND *m  = (T_COND *)cnd;
	T_MUTEX *n = (T_MUTEX *)mtx;

	struct timespec t;
	t.tv_sec=time(NULL)+ timeout/1000;
	t.tv_nsec= (timeout%1000)*1000;

	return COND_TIMEWAIT(m,n,&t);
}

void lpi_cond_signal(HCOND cnd)
{
	T_COND *m  = (T_COND *)cnd;
	COND_SIGNAL(m);
}

#ifndef WIN32
#define T_RWLOCK                    pthread_rwlock_t
#define RWLOCK_INIT(m)			    pthread_rwlock_init(m,NULL)
#define RWLOCK_UNINIT(m)			pthread_rwlock_destroy(m)
#define RWLOCK_WRLOCK(m)			pthread_rwlock_wrlock(m)
#define RWLOCK_RDLOCK(m)        	pthread_rwlock_rdlock(m)
#define RWLOCK_UNLOCK(m)			pthread_rwlock_unlock(m)
#endif // WIN32

HRWLOCK lpi_rwlock_create()
{
    T_RWLOCK *m = (T_RWLOCK *)malloc(sizeof(T_RWLOCK));
    RWLOCK_INIT(m);
    return (T_RWLOCK)m;
}

void lpi_rwlock_destroy(HRWLOCK h)
{
    T_RWLOCK *m = (T_RWLOCK *)h;
    RWLOCK_UNINIT(m);
    free(m);
}
void lpi_rwlock_wrlock(HRWLOCK h)
{
    T_RWLOCK *m = (T_RWLOCK *)h;
    RWLOCK_WRLOCK(m);
}
void lpi_rwlock_rdlock(HRWLOCK h)
{
    T_RWLOCK *m = (T_RWLOCK *)h;
    RWLOCK_RDLOCK(m);
}
void lpi_rwlock_unlock(HRWLOCK h)
{
    T_RWLOCK *m = (T_RWLOCK *)h;
    RWLOCK_UNLOCK(m);
}


