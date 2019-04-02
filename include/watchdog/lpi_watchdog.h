#ifndef LPI_WATCHDOG_H
#define LPI_WATCHDOG_H

#include "lpi_types.h"
DECLAR_HANDLE(HWDT);

typedef struct
{
    char wdt_dev[32];
}WdtParam;

HWDT wdt_start(const WdtParam *param);

int wdt_stop(HWDT h);

int wdt_get_timeout(HWDT h,int *timeout);

int wdt_set_timeout(HWDT h,int timeout);

int wdt_keep_alive(HWDT h);

#endif // LPI_WATCHDOG_H
