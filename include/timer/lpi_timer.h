#ifndef LPI_TIMER_H
#define LPI_TIMER_H


class Timer
{
public:
    Timer();
    ~Timer();

    int Create(string name,int (*func)(void *),void *arg,double interval = 1,bool repeat = true);
    int SetTimerInternal(double interval);

    int Start();
    void Stop();
    void Exec(Timer &);

    static void Notify(int timer_id,double msec = 0,int sec = 1,bool repeat = true);

    int GetTimerId() {return mTimerId;}
    void SetTimerId(int value) {mTimerId = value;}

    int SetNonBlock(int fd);
private:
    int mTimerId;
    double mTimerinterval;
    bool mRepeat;//once or period
    int (*mTimerFunc)(void *);
    void *mArg;
    string mName;
};


#define TIMER_MAX   100
class TimerManager
{
private:
    TimerManager(int timer_max = TIMER_MAX);
    ~TimerManager();
    static TimerManager *mTimerManager;
public:

    static TimerManager *GetInstance();
    static void FreeInstance();

    static int AddTimer(Timer &timer);
    static int DelTimer(Timer &timer);

    int Listen();
    void DeActive() {mActive = 0;}

private:
    static int mEpollfd;
    int mActive;
    int mListenFlag;
    static map<int,Timer> mTimers;
};
#endif // LPI_TIMER_H
