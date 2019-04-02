#include <fcntl.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>

#include "lpi_timer.h"

#ifdef TIMER_DEBUG
#define timer_debug(...) do{\
                    printf(__VA_ARGS__);\
                    }while(0)
#else
#define timer_debug(...)
#endif // TIMER_DEBUG

using namespace zhim200_timer;
/** timer **/

Timer::Timer(): mTimerId(-1),mTimerinterval(1),mRepeat(false),mTimerFunc(NULL),mArg(NULL)
{
}

Timer::~Timer()
{
    Stop();
}
int Timer::SetNonBlock(int fd)
{
    int flags = fcntl(fd,F_GETFL,0);
    flags |= O_NONBLOCK;

    if(fcntl(fd,F_SETFL,flags) == -1)
        return -1;

    return 1;
}
int Timer::Create(string name,int (*func)(void *),void *arg,double interval,bool repeat)
{
    if(func == NULL)
        return false;
    mTimerId = timerfd_create(CLOCK_REALTIME,0);
    if(mTimerId > 0)
    {
        mTimerFunc = func;
        mRepeat = repeat;
        mArg = arg;
        mTimerinterval = interval;
        mName = name;
        SetNonBlock(mTimerId);
    }
    return 1;
}

int Timer::SetTimerInternal(double interval)
{
    mTimerinterval = interval;
    Start();

    return 1;
}

void Timer::Exec(Timer &timer)
{
    char buf[32] = "";
    int ret = read(mTimerId,buf,sizeof(buf));
    if(ret > 0 && mTimerFunc)
    {
        mTimerFunc(timer.mArg);
    }
}

int Timer::Start()
{
    struct itimerspec l_time = {0};

    l_time.it_value.tv_sec = (int)mTimerinterval;
    l_time.it_value.tv_nsec = (mTimerinterval - (int)mTimerinterval) * 1000000;

    if(mRepeat)
    {
        /** 如果为0 表示不是周期性 **/
        l_time.it_interval.tv_sec = l_time.it_value.tv_sec;
        l_time.it_interval.tv_nsec = l_time.it_value.tv_nsec;
    }

    return timerfd_settime(mTimerId,0,&l_time,NULL);
}

void Timer::Notify(int timer_id,double msec,int sec,bool repeat)
{
    struct itimerspec l_time = {0};

    l_time.it_value.tv_sec = sec;
    l_time.it_value.tv_nsec = msec * 1000000;

    if(repeat)
    {
        /** 如果为0 表示不是周期性 **/
        l_time.it_interval.tv_sec = l_time.it_value.tv_sec;
        l_time.it_interval.tv_nsec = l_time.it_value.tv_nsec;
    }

    timerfd_settime(timer_id,0,&l_time,NULL);
}

void Timer::Stop()
{
    if(mTimerId > 0)
        close(mTimerId);
}



/** timer manager **/
int TimerManager::mEpollfd = -1;
map<int,Timer> TimerManager::mTimers;

TimerManager *TimerManager::mTimerManager = new TimerManager();

TimerManager *TimerManager::GetInstance()
{
    if(getuid() != 0)
    {
        timer_debug("please used root account access\n");
        return NULL;
    }


    return mTimerManager;
}

void TimerManager::FreeInstance()
{
    if(mTimerManager)
        delete mTimerManager;
}

TimerManager::TimerManager(int timer_max):mActive(0),mListenFlag(-1)
{
    mEpollfd = epoll_create(timer_max);
    if(mEpollfd > 0)
        mActive = 1;
}

TimerManager::~TimerManager()
{
    for(map<int,Timer>::iterator it = mTimers.begin();it != mTimers.end();it++)
    {
        DelTimer(it->second);
    }
}

int TimerManager::AddTimer(Timer &timer)
{
    int timerId = timer.GetTimerId();

    struct epoll_event event;
    event.data.fd = timerId;
    event.events = EPOLLIN | EPOLLET;


    mTimers[timerId] = timer;

    int ret = epoll_ctl(mEpollfd,EPOLL_CTL_ADD,timerId,&event);


    return ret;
}

int TimerManager::DelTimer(Timer &timer)
{
    int timerId = timer.GetTimerId();

    struct epoll_event event;
    event.data.fd = timerId;
    event.events = EPOLLIN | EPOLLET;



    int ret = epoll_ctl(mEpollfd,EPOLL_CTL_DEL,timerId,&event);

    mTimers.erase(timerId);



    return ret;
}

int TimerManager::Listen()
{
    mListenFlag++;
    if(mListenFlag == 1)
    {
        timer_debug("listen repeat error....\n");
        return 0;
    }
    struct epoll_event events[TIMER_MAX] = {0};
    int fd_num = -1;
    int pos = 0;
    map<int,Timer>::iterator it;
    for(;;)
    {
        if(TimerManager::mActive == 0)
            break;

        fd_num = epoll_wait(mEpollfd,events,TIMER_MAX,-1);
        if(fd_num < 0)
        {
            timer_debug("[TimerManager] epoll wait return error\n");
            continue;
        }

        for(pos = 0;pos < fd_num;++pos)
        {
            if(events[pos].events & EPOLLIN)
            {
                it = mTimers.find(events[pos].data.fd);
                if(it != mTimers.end())
                {
                    it->second.Exec(it->second);
                }
            }
        }
    }

    return 0;
}





