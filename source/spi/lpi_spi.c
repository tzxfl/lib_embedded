#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>

#include "lpi_spi.h"


typedef struct
{
	SpiParam param;
	int spifd;
}spi_t;

HSPI spi_open(const SpiParam *param)
{
	 if(!param)
	 	return NULL;

	int statusVal = -1;
	int spifd = open(param->spi_dev, O_RDWR);
	if(spifd < 0)
	{
		ERROR("[a2d] could not open SPI device\n");
		return NULL;
	}

	statusVal = ioctl (spifd, SPI_IOC_WR_MODE, &(param->spiMode));
	if(statusVal < 0)
	{
		ERROR("[a2d] Could not set SPIMode (WR)...ioctl fail\n");
		close(spifd);
		return NULL;
	}

	statusVal = ioctl (spifd, SPI_IOC_RD_MODE, &(param->spiMode));
	if(statusVal < 0)
	{
		ERROR("[a2d] Could not set SPIMode (RD)...ioctl fail\n");
		close(spifd);
		return NULL;
	}

	statusVal = ioctl (spifd, SPI_IOC_WR_BITS_PER_WORD, &(param->spibitsPerWord));
	if(statusVal < 0)
	{
		ERROR("[a2d] Could not set SPI bitsPerWord (WR)...ioctl fail\n");
		close(spifd);
		return NULL;
	}

	statusVal = ioctl (spifd, SPI_IOC_RD_BITS_PER_WORD, &(param->spibitsPerWord));
	if(statusVal < 0)
	{
		ERROR("[a2d] Could not set SPI bitsPerWord(RD)...ioctl fail\n");
		close(spifd);
		return NULL;
	}

	statusVal = ioctl (spifd, SPI_IOC_WR_MAX_SPEED_HZ, &(param->spiSpeed));
	if(statusVal < 0)
	{
		ERROR("[a2d] Could not set SPI speed (WR)...ioctl fail\n");
		close(spifd);
		return NULL;
	}

	statusVal = ioctl (spifd, SPI_IOC_RD_MAX_SPEED_HZ, &(param->spiSpeed));
	if(statusVal < 0)
	{
		ERROR("[a2d] Could not set SPI speed (RD)...ioctl fail\n");
		close(spifd);
		return NULL;
	}

	a2d_t *a2d = calloc(1, sizeof(a2d_t) );
	if(!a2d)
		return NULL;

	a2d->spifd = spifd;
	memcpy(&a2d->param,param,sizeof(a2d->param));

	return (HA2D)(a2d);
}

int spi_close(HSPI h)
{
	spi_t *spi = (spi_t*)h;
	if(!spi)
		return -1;

	int statusVal = -1;
	statusVal = close(a2d->spifd);
	if(statusVal < 0)
	{
		ERROR("[a2d] Could not close SPI device\n");
		return -1;
	}

	free(a2d);

	return 0;
}


/********************************************************************
* This function writes data "data" of length "length" to the spidev
* device. Data shifted in from the spidev device is saved back into
* "data".
*******************************************************************/
static int spiWriteRead( a2d_t *a2d, unsigned char *data, int length)
{
//要根据12bit的时序图来，需要两个时钟周期所以传输两次(全双工)
	struct spi_ioc_transfer spi[length];
	int i = 0;
	int retVal = -1;   // one spi transfer for each byte
	for (i = 0 ; i < length ; i++)
	{
		spi[i].tx_buf  = (unsigned long)(data + i); // transmit from "data"
		spi[i].rx_buf  = (unsigned long)(data + i) ; // receive into "data"
		spi[i].len  = sizeof(*(data + i)) ;
		spi[i].delay_usecs   = 0 ;
		spi[i].speed_hz      = a2d->param.spiSpeed;
		spi[i].bits_per_word = a2d->param.spibitsPerWord ;
		spi[i].cs_change = 0;
	}

	retVal = ioctl (a2d->spifd, SPI_IOC_MESSAGE(length), &spi) ;
	if(retVal < 0)
	{
		ERROR("[a2d] Problem transmitting spi data..ioctl\n");
		return -1;
	}
	return 0;
}

typedef enum
{
	OUT_LEN_8bit	= 0x01,
	OUT_LEN_12bit	= 0x02,
	OUT_LEN_16bit	= 0x03,
}OutputLen;

enum
{
	MSB = 0x00,
	LSB,
};

C_API int a2d_getADValue(HA2D h, unsigned char achannel, int *value)
{
	a2d_t *a2d = (a2d_t*)h;
	if(!a2d)
		return -1;

	if(achannel > MAX_ACHANNEL)
	{
		ERROR("[a2d] parameter achannel > %d \n",MAX_ACHANNEL);
		return -1;
	}

	int  inout = 0;
        unsigned char  data[2];
	data[0] = (achannel << 4) | (OUT_LEN_12bit<<2) | (MSB <<1);
       data[1] = 0;
	if(spiWriteRead(a2d, data,2) < 0)
		return -1;

       INFO("[a2d] channel:%d, B0:%02x  B1:%02x\n",achannel, data[0],data[1]);

        inout = ((int)data[0]<<4)&0xff0; //返回的值是A11~A4
        inout |=(data[1]>>4);  //返回的值是A3~A0
	*value = inout;

	return 0;
}





