
#include "lpi_i2c.h"

typedef struct i2c_inner{
	int fd;
}i2c_t;


HI2C open_i2cdev(const char *i2c_dev)
{
	if (i2c_dev == NULL){
		printf("open %s failed,Don't is empty\n",i2c_dev);
		return NULL;
	}

	i2c_t *i2c = calloc(1,sizeof(struct i2c_t));
	if (i2c == NULL){
		return NULL;
	}
	i2c->fd = open(i2c_dev,O_RDWR);

	if (i2c->fd < 0){
		printf("open %s failed\n");
		return NULL;
	}

	return (HI2C)i2c;
}

int close_i2cdev(HI2C h)
{
	i2c_t *i2c = (i2c_t *)h;
	if (i2c == NULL){
		return -1;
	}
	if (i2c->fd){
		close(i2c->fd);
	}
	free(i2c);
	return 0;
}

int i2c_transfer(HI2C h,int addr, char rdwr,int num)
{
	i2c_t *i2c = (i2c_t *)h;
	if (i2c == NULL || msg == NULL || (num == 0 && num < 64))
		return -1;

	struct i2c_rdwr_ioctl_data i2c_ioctl;
	i2c_ioctl.nmsgs = num;
	struct i2c_msg *i2c_msgs = calloc(num * sizeof(struct i2c_msg));
	int i = 0;

	for (i; i < num; i++){
		i2c->i2c_ioctl.msgs[i].addr = ;
		i2c->i2c_ioctl.msgs[i].len = ;
		i2c->i2c_ioctl.msgs[i].flags = ;
		i2c->i2c_ioctl.msgs[i].buf = ;
	}

	ioctl(i2c->fd,I2C_RDWR,&(i2c->i2c_ioctl));
}

int i2c_read_byte(HI2C h,char *data,int pos)
{
	int size = 1;
	return i2c_read_data(h,data,pos,&size);
}

int i2c_write_byte(HI2C h, unsigned char addr,char *data,int pos)
{
	int size = 1;
	return i2c_write_data(h,addr,data,pos,&size);
}

int i2c_read_data(HI2C h,unsigned char addr,char *data,int pos, int *len)
{
	i2c_t *i2c = (i2c_t *)h;
	if (i2c==NULL || data == NULL || len == NULL || pos < 0){
			return -1;
	}

    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg i2c_msg[2];

    i2c_data.nmsgs = 2;
    i2c_data.msgs = &i2c_msg[0];

    i2c_config(h,1,2);

    // write reg
    i2c_msg[0].len = 1;
    i2c_msg[0].addr = addr;
    i2c_msg[0].flags = 0;
    i2c_msg[0].buf = data;

    //read data
    i2c_msg[1].len = *len;
    i2c_msg[1].addr = addr;
    i2c_msg[1].flags = 1;
    i2c_msg[1].buf = data;

    int ret = ioctl(i2c->fd,I2C_RDWR,&i2c_data);

    return ret;
}

int i2c_write_data(HI2C h,unsigned char addr,char *data, int pos,int *len)
{
    i2c_t *i2c = (i2c_t *)h;

    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg i2c_msg;

    i2c_data.nmsgs = 1;
    i2c_data.msgs = &i2c_msg;

    i2c_config(h,1,2);

    i2c_msg.len = *len;
    i2c_msg.addr = addr;
    i2c_msg.flags = 0; // 0 write 1 read
    i2c_msg.buf = data;

    int ret = ioctl(i2c->fd,I2C_RDWR,&i2c_data);

    return ret;
}

int i2c_config(HI2C h,int timeout,int retries)
{
    i2c_t *i2c = (i2c_t *)h;
    if(NULL == i2c)
        return -1;
    //超时时间
    ioctl(i2c->fd,I2C_TIMEOUT,timeout > 0 ? timeout : 0);
    //重试次数
    ioctl(i2c->fd,I2C_RETRIES,retries > 0 ? retries : 0);

    return 0;
}
