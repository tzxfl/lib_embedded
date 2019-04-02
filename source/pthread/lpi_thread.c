
#include "lpi_thread.h"

//获取调度方式
static int get_thread_policy(pthread_attr_t *attr)
{
    int policy;
    int rs = pthread_attr_getschedpolicy( attr, &policy );
    assert( rs == 0 );
    if(rs < 0)
        return -1;
    switch ( policy )
    {
    case SCHED_FIFO:
        INFO("policy = SCHED_FIFO \n");
        break;

    case SCHED_RR:
        INFO("policy = SCHED_RR \n");
        break;

    case SCHED_OTHER:
        INFO("policy = SCHED_OTHER \n");
        break;

    default:
        INFO("policy = UNKNOWN \n");
        break;
    }

    return policy;
}

//设置调度方式
static void set_thread_policy( pthread_attr_t *attr,  int policy )
{
    pthread_attr_setschedpolicy( attr, policy );

}
//获取线程优先级
static int get_thread_priority( pthread_attr_t *attr )
{
    struct sched_param param;

    int rs = pthread_attr_getschedparam( attr, &param );
    assert( rs == 0 );
    if( rs < 0)
        return -1;

    return param.__sched_priority;
}
//设置线程优先级
static void set_thread_priority( pthread_attr_t *attr, int priority )
{

    struct sched_param param;
    int rs = pthread_attr_getschedparam( attr, &param );
    assert( rs == 0 );
    if( rs < 0)
        return;
    param.__sched_priority = priority;
    pthread_attr_setschedparam( attr, &param );

}

int thread_simple_create(thread_task task,void *arg,unsigned int stack_size,int priority)
{
    if(!task)
        return -1;

    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    if(stack_size != 0)
    {
        pthread_attr_setstacksize(&thread_attr,stack_size);
    }

    if(priority > 0)
    {
        pthread_attr_setschedpolicy(&thread_attr,SCHED_RR);
        struct sched_param thread_param;
        int ret = pthread_attr_getschedparam(&thread_attr,&thread_param);
        if(ret < 0)
        {
            thread_debug("[thread] get thread param fail\n");
            return -1;
        }

        thread_param.__sched_priority = priority;
        pthread_attr_setschedparam(&thread_attr,&thread_param);
    }

    /** create thread **/
    pthread_t tid;
    int ret = pthread_create(&tid,&thread_attr,task,arg);
    if(ret != 0 || tid == 0)
    {
        thread_debug("[thread] pthread create fail\n");
        return -1;
    }

    return 1;
}

typedef struct
{
    volatile int flag;
    thread_task task;
    void *context;
    int tid;
    int priority;
} thread_t;

static int inner_create(thread_task task,int stack_size,int priority,void *context)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (stack_size != 0)
    {
        pthread_attr_setstacksize(&attr, stack_size);
    }

    if(priority > 0)
    {
        set_thread_policy(&attr,SCHED_RR);
        set_thread_priority(&attr,priority);
    }

    //pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_t tid = 0;
    int ret = pthread_create(&tid, &attr, task, context);
    if (ret != 0 || tid == 0)
    {
        printf("[litepi]pthread_create fail!\n");
        return -1;
    }
    return 0;
}

static void *inner_task(void *p)
{
    thread_t *thread = (thread_t *)p;
    if(!thread)
        return NULL;
    thread->tid = thread_gettid();
    void *ret = thread->task((THREAD)thread,thread->context);

    return ret;
}

HTHREAD thread_create(thread_task task,void *arg,unsigned int stack_size,int priority)
{
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
    if(!thread)
    {
        return NULL;
    }

    thread->flag = 1;
    thread->task = task;
    thread->context = arg;
    thread->tid = 0;
    thread->priority = priority;

    if(0 != inner_create(inner_task,stack_size,priority,thread))
    {
        free(thread);
        return NULL;
    }

    return (HTHREAD)thread;
}

int thread_is_running(HTHREAD h)
{
    thread_t *th = (thread_t*)h;
    if(!th)
        return 0;
    return th->flag;
}

int thread_getpid()
{
    return getpid();
}

int thread_gettid()
{
    return (int)pthread_self();
}

void thread_destroy(HTHREAD h)
{
    thread_t *th = (thread_t *)h;
    if(th)
    {
        th->flag = 0;
        usleep(500);
        ignore_signal(15);
        kill(th->tid,15);
        pthread_join(th->tid,NULL);
        free(th);
    }
}
