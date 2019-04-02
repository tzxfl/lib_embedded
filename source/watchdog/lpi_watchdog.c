#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <fcntl.h>

#include "lpi_watchdog.h"

#define SUCCESS 0
#define FAILED -1

typedef struct
{
	WdtParam param;
	int fd;
	int g_timeout;

}wdt_t;


/**
 * brief: open a watchdog
 * @Param: device: Which devices you want to open
 * @return: -1 if an error occurred .on the success, return a new file descriptor
 */
WDT wdt_start(const WdtParam *param)
{

	int fd;

	if (NULL == param)
	{
		return (WDT)NULL;
	}

	if ((fd = open(param->wdt_dev,O_RDWR)) < 0)
	{
		return (WDT)NULL;
	}

	wdt_t *wdt = calloc(1,sizeof(wdt_t));
	if (NULL == wdt)
	{
		return (WDT)NULL;
	}
	memcpy(&(wdt->param),param,sizeof(wdt->param));
	wdt->fd = fd;

	return (WDT)wdt;

}

/**
 * brief: close a watchdog
 * @Param: fd: a file descriptor
 * @return: -1 if an error occurred. on the success, return 0
 */
int wdt_stop(WDT w)
{
	wdt_t *wdt = (wdt_t *)w;
  	if (NULL == wdt)
		return FAILED;

	if (wdt->fd)
	{
		close(wdt->fd);
	}

	free(wdt);
	return SUCCESS;

}

/**
 * brief: get overflow time
 * @Param: fd: a file descriptor
 * @return: -1 if an error occurred .on the success, return a overflow time
 */
int wdt_get_timeout(WDT w, int *timeout)
{
	wdt_t *wdt = (wdt_t *)w;

	int ret;
	ret = ioctl(wdt->fd,WDIOC_GETTIMEOUT,&(wdt->g_timeout));
	if (ret < 0)
	{
		return FAILED;
	}
	else
	{
		*timeout = wdt->g_timeout;
		return SUCCESS;
	}
}

/**
 * brief: get overflow time
 * @Param: fd: a file descriptor
 * @Param: timeout: want to set the timeout
 * @return: -1 if an error occurred .on the success, return 0
 */
int wdt_set_timeout(WDT w,int timeout)
{
	wdt_t *wdt = (wdt_t *)w;

	int ret;
	ret = ioctl(wdt->fd,WDIOC_SETTIMEOUT,&timeout);
	if (ret < 0)
	{
		return FAILED;
	}
	else
	{
		return SUCCESS;
	}
}

/**
 * brief: get system status
 * @Param: fd: a file descriptor
 * @Param: slp: How long time to feed a dog
 * @return: -1 if an error occurred .on the success, return 0
 */
int  wdt_keep_alive(WDT w)
{
	wdt_t *wdt = (wdt_t *)w;

	int ret;
	ret = write(wdt->fd,"\0",1);
	if (ret != 1)
	{
		return FAILED;
	}
	else
	{
		return SUCCESS;
	}
}



