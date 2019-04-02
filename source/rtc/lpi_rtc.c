#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lpi_rtc.h"

typedef struct
{
	RtcParams param;
	int fd_rtc;
}rtc_t;

HRTC rtc_open(const RtcParams *param)
{
	if(!param)
		return NULL;

	/* open rtc device */
	int fd = open(param->rtc_dev, O_RDWR);
	if (fd ==  -1)
	{
		return NULL;
	}

	rtc_t * rtc = calloc(1,sizeof(rtc_t));
	if(!rtc)
	{
		close(fd);
		return NULL;
	}

	memcpy(&rtc->param,param,sizeof(RTCParams));
	rtc->fd_rtc = fd;

	return (HRTC)rtc;

}

int rtc_close(HRTC h)
{
	rtc_t *rtc = (rtc_t *)h;
	if(!rtc)
		return -1;

	close(rtc->fd_rtc);
	free(rtc);
	return 0;
}

int rtc_read_time(HRTC h, DateTime *currTime)
{
	rtc_t *rtc = (rtc_t *)h;
	if(!rtc || !currTime)
		return -1;

	int retval;
	struct rtc_time rtc_tm;
	/*read the RTC time/date*/
	retval = ioctl(rtc->fd_rtc, RTC_RD_TIME, &rtc_tm);
	if (retval == -1)
	{
		return -1;
	}

	currTime->day = rtc_tm.tm_mday;
	currTime->month = rtc_tm.tm_mon + 1;
	currTime->year= rtc_tm.tm_year + 1900;
	currTime->hour = rtc_tm.tm_hour;
	currTime->minute = rtc_tm.tm_min;
	currTime->second = rtc_tm.tm_sec;

	return 0;
}

int rtc_set_time(HRTC h, const DateTime *currTime)
{
	rtc_t *rtc = (rtc_t *)h;
	if(!rtc || !currTime)
		return -1;

	int retval;
	struct rtc_time rtc_tm;

	rtc_tm.tm_mday = currTime->day ;
	rtc_tm.tm_mon = currTime->month-1;
	rtc_tm.tm_year = currTime->year-1900;
	rtc_tm.tm_hour = currTime->hour;
	rtc_tm.tm_min = currTime->minute;
	rtc_tm.tm_sec = currTime->second;
	rtc_tm.tm_wday = rtc_tm.tm_yday = rtc_tm.tm_isdst = 0;

	retval = ioctl(rtc->fd_rtc, RTC_SET_TIME, &rtc_tm);
	if (retval == -1)
	{
		return -1;
	}

	return 0;

}

//读取下一次定时中断的时间
int rtc_read_alarm(HRTC h, DateTime *currTime)
{
	rtc_t *rtc = (rtc_t *)h;
	if(!rtc || !currTime)
		return -1;

	int retval;
	struct rtc_time rtc_tm;
	/*read the RTC time/date*/
	retval = ioctl(rtc->fd_rtc, RTC_ALM_READ, &rtc_tm);
	if (retval == -1)
	{
		return -1;
	}

	currTime->day = rtc_tm.tm_mday;
	currTime->month = rtc_tm.tm_mon + 1;
	currTime->year= rtc_tm.tm_year + 1900;
	currTime->hour = rtc_tm.tm_hour;
	currTime->minute = rtc_tm.tm_min;
	currTime->second = rtc_tm.tm_sec;

	return 0;
}

//设置下一次定时中断的时间
int rtc_set_alarm(HRTC h, const DateTime *currTime)
{
	rtc_t *rtc = (rtc_t *)h;
	if(!rtc || !currTime)
		return -1;

	int retval;
	struct rtc_time rtc_tm;

	rtc_tm.tm_mday = currTime->day ;
	rtc_tm.tm_mon = currTime->month-1;
	rtc_tm.tm_year = currTime->year-1900;
	rtc_tm.tm_hour = currTime->hour;
	rtc_tm.tm_min = currTime->minute;
	rtc_tm.tm_sec = currTime->second;
	rtc_tm.tm_wday = rtc_tm.tm_yday = rtc_tm.tm_isdst = 0;

	retval = ioctl(rtc->fd_rtc, RTC_ALM_SET, &rtc_tm);
	if (retval == -1)
	{
		return -1;
	}

	return 0;
}

int rtc_enable_alarm(HRTC h, int enbled)
{
	rtc_t *rtc = (rtc_t *)h;
	if(!rtc)
		return -1;

	int retval;
	int rtc_enabled = enbled ? RTC_AIE_ON : RTC_AIE_OFF;
	retval = ioctl(rtc->fd_rtc, rtc_enabled, NULL);
	if (retval == -1)
	{
		return -1;
	}

	return 0;

}


