#ifndef LPI_RTC_H
#define LPI_RTC_H

typedef struct
{
    char rtc_dev[32];
}RtcParams;

//时间结构体
typedef struct
{
	unsigned short year;		/*1-9999*/
	char month;					/*1-12*/
	char day;					/*1-31*/
	char hour;					/*0-23*/
	char minute;				/*0-59*/
	char second;				/*0-59*/
}DateTime;

DECLAR_HANDLE(HRTC);

HRTC rtc_open(const RtcParams *param);

int rtc_close(HRTC h);

int rtc_read_time(HRTC h,DateTime *tm);

int rtc_set_time(HRTC h,DateTime *tm);

int rtc_read_alarm(HRTC h,DateTime *tm);

int rtc_set_alarm(HRTC h,const DateTime *tm);

int rtc_enable_alarm(HRTC h,int enabled);

#endif // LPI_RTC_H
