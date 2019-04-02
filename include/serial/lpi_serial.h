#ifndef LPI_SERIAL_H
#define LPI_SERIAL_H


#include <termios.h>
#include <time.h>

#define RS232_STRLEN 512
#define RS232_STRLEN_DEVICE 32

#define RS232_DEBUG_

#ifdef RS232_DEBUG_
#define DEBUG_(...) do{\
            printf(__VA_ARGS__);\
            }while(0)
#else
#define DEBUG_(...)
#endif // RS232_DEBUG_

#define RS232_PORT_POSIX "/dev/ttyO4"

/** baud **/
enum rs232_baud_e {
	RS232_BAUD_300,
	RS232_BAUD_2400,
	RS232_BAUD_4800,
	RS232_BAUD_9600,
	RS232_BAUD_19200,
	RS232_BAUD_38400,
	RS232_BAUD_57600,
	RS232_BAUD_115200,
	RS232_BAUD_460800,
	RS232_BAUD_MAX
};
/** data bit **/
enum rs232_data_e {
	RS232_DATA_5,
	RS232_DATA_6,
	RS232_DATA_7,
	RS232_DATA_8,
	RS232_DATA_MAX
};
/** parity **/
enum rs232_parity_e {
	RS232_PARITY_NONE,
	RS232_PARITY_ODD,
	RS232_PARITY_EVEN,
	RS232_PARITY_MAX
};

enum rs232_stop_e {
	RS232_STOP_1,
	RS232_STOP_2,
	RS232_STOP_MAX
};
/** flow contrl **/
enum rs232_flow_e {
	RS232_FLOW_OFF,
	RS232_FLOW_HW,
	RS232_FLOW_XON_XOFF,
	RS232_FLOW_MAX
};
/** status **/
enum rs232_status_e {
	RS232_PORT_CLOSED,
	RS232_PORT_OPEN,
};
/** Data Terminal Ready **/
enum rs232_dtr_e {
	RS232_DTR_OFF,
	RS232_DTR_ON,
	RS232_DTR_MAX
};
/** Request To Send **/
enum rs232_rts_e {
	RS232_RTS_OFF,
	RS232_RTS_ON,
	RS232_RTS_MAX
};
struct rs232_posix_t {
    int fd;
    struct termios oldterm;
};

struct rs232_port_t {
	char dev[RS232_STRLEN_DEVICE];
	enum rs232_baud_e baud;
	enum rs232_data_e data;
	enum rs232_stop_e stop;
	enum rs232_flow_e flow;
	enum rs232_parity_e parity;
	enum rs232_status_e status;
	enum rs232_dtr_e dtr;
	enum rs232_rts_e rts;

    struct rs232_posix_t pt;
};

enum rs232_error_e {
	RS232_ERR_NOERROR,
	RS232_ERR_UNKNOWN,
	RS232_ERR_OPEN,
	RS232_ERR_CLOSE,
	RS232_ERR_FLUSH,
	RS232_ERR_CONFIG,
	RS232_ERR_READ,
	RS232_ERR_WRITE,
	RS232_ERR_SELECT,
	RS232_ERR_TIMEOUT,
	RS232_ERR_IOCTL,
	RS232_ERR_PORT_CLOSED,
	RS232_ERR_MAX
};


struct rs232_port_t * rs232_init(void);
void rs232_end(struct rs232_port_t *p);
//void rs232_end();
unsigned int rs232_open(struct rs232_port_t *p);
unsigned int rs232_port_open(struct rs232_port_t *p);
unsigned int rs232_close(struct rs232_port_t *p);
unsigned int rs232_flush(struct rs232_port_t *p);
void rs232_set_device(struct rs232_port_t *p, const char *device);
unsigned int rs232_set_baud(struct rs232_port_t *p, unsigned int baud);
unsigned int rs232_set_stop(struct rs232_port_t *p, unsigned int stop);
unsigned int rs232_set_data(struct rs232_port_t *p, unsigned int data);
unsigned int rs232_set_parity(struct rs232_port_t *p, unsigned int parity);
unsigned int rs232_set_flow(struct rs232_port_t *p, unsigned int flow);
unsigned int rs232_set_dtr(struct rs232_port_t *p, unsigned int dtr);
unsigned int rs232_set_rts(struct rs232_port_t *p, unsigned int rts);
const char * rs232_get_device(struct rs232_port_t *p);
unsigned int rs232_get_baud(struct rs232_port_t *p);
unsigned int rs232_get_stop(struct rs232_port_t *p);
unsigned int rs232_get_data(struct rs232_port_t *p);
unsigned int rs232_get_parity(struct rs232_port_t *p);
unsigned int rs232_get_flow(struct rs232_port_t *p);
unsigned int rs232_get_dtr(struct rs232_port_t *p);
unsigned int rs232_get_rts(struct rs232_port_t *p);
unsigned int rs232_read(struct rs232_port_t *p, unsigned char *buf, unsigned int buf_len, unsigned int *read_len);
unsigned int rs232_read_timeout(struct rs232_port_t *p, unsigned char *buf, unsigned int buf_len, unsigned int *read_len, unsigned int timeout);
unsigned int rs232_read_timeout_forced(struct rs232_port_t *p, unsigned char *buf, unsigned int buf_len, unsigned int *read_len, unsigned int timeout);
unsigned int rs232_write(struct rs232_port_t *p, const unsigned char *buf, unsigned int buf_len, unsigned int *write_len);
unsigned int rs232_write_timeout(struct rs232_port_t *p, const unsigned char *buf, unsigned int buf_len, unsigned int *write_len, unsigned int timeout);
unsigned int rs232_in_qeue(struct rs232_port_t *p, unsigned int *in_bytes);
void rs232_in_qeue_clear(struct rs232_port_t *p);
const char * rs232_to_string(struct rs232_port_t *p);
const char * rs232_strerror(unsigned int error);
const char * rs232_strbaud(unsigned int baud);
const char * rs232_strdata(unsigned int data);
const char * rs232_strparity(unsigned int parity);
const char * rs232_strstop(unsigned int stop);
const char * rs232_strflow(unsigned int flow);
const char * rs232_strdtr(unsigned int dtr);
const char * rs232_strrts(unsigned int rts);
unsigned int rs232_fd(struct rs232_port_t *p);


#define GET_PORT_STATE(fd, state) \
	if (tcgetattr(fd, state) < 0) { \
		return RS232_ERR_CONFIG; \
	}

#define SET_PORT_STATE(fd, state) \
	if (tcsetattr(fd, TCSANOW, state) < 0) { \
		return RS232_ERR_CONFIG; \
	} \

#endif // LPI_SERIAL_H
