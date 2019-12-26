#ifndef LPI_I2C_H
#define LPI_I2C_H

#include "lpi_types.h"

DECLAR_HANDLE(HI2C)

HI2C open_i2cdev(const char *i2c_dev);

int close_i2cdev(HI2C h);

int i2c_transfer();

int i2c_read_byte(HI2C h,char *data,int pos);

int i2c_write_byte(HI2C h, char *data,int pos);

int i2c_read_data(HI2C h, char *data,int pos, int *len);

int i2c_write_data(HI2C h, char *data, int pos, int *len);

int i2c_config(HI2C h,int timeout,int retries);

#endif // LPI_I2C_H
