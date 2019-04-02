#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include "serial_posix.h"

static struct rs232_port_t *rs232_port = NULL;
#define rs232_G rs232_port

struct rs232_port_t *rs232_init(void)
{
	struct rs232_port_t *p = NULL;

    if(rs232_G == NULL)
    {
        rs232_G = (struct rs232_port_t *)malloc(sizeof(struct rs232_port_t));
        if(rs232_G == NULL)
            return NULL;
    }

    p = rs232_G;

	//p = (struct rs232_port_t *) malloc(sizeof(struct rs232_port_t));

	if (p == NULL)
		return NULL;

	//p->pt = (struct rs232_posix_t *) malloc(sizeof(struct rs232_posix_t));

	//if (p->pt == NULL)
	//	return NULL;

//	DEBUG_("p=%p p->pt=%p\n", (void *)p, p->pt);

	memset(&(p->pt), 0, sizeof(struct rs232_posix_t));
	memset(p->dev, 0, RS232_STRLEN_DEVICE+1);
	strncpy(p->dev, RS232_PORT_POSIX, RS232_STRLEN_DEVICE);

	p->baud = RS232_BAUD_115200;
	p->data = RS232_DATA_8;
	p->parity = RS232_PARITY_NONE;
	p->stop = RS232_STOP_1;
	p->flow = RS232_FLOW_OFF;
	p->status = RS232_PORT_CLOSED;
	p->dtr = RS232_DTR_OFF;
	p->rts = RS232_RTS_OFF;

	return p;
}

void rs232_end(struct rs232_port_t *p)
//void rs232_end()
{
	//struct rs232_posix_t *ux = p->pt;
    //struct rs232_posix_t *ux = &(rs232_G->pt);//&(p->pt);
	//DEBUG_("p=%p p->pt=%p\n", (void *)p, p->pt);

	if (rs232_G && !rs232_port_open(rs232_G)) {
		//free(p->pt);
		//free(p);
		free(rs232_G);
		rs232_G = NULL;
		return;
	}
#if 0
	rs232_flush(p);

	if (tcsetattr(ux->fd, TCSANOW, &ux->oldterm) < 0) {
		DEBUG_("tcsetattr() %d %s\n", errno, strerror(errno));
		return;
	}
#endif
    DEBUG_("rs232-end...\n");
	//rs232_close(p);
	rs232_close(p);
	if(rs232_G)
    {
        DEBUG_("free rs232 G\n");
        free(rs232_G);
        rs232_G = NULL;
    }
#if 0
	if(p->pt)
    {
        DEBUG_("free p->ptr\n");
        free(p->pt);
        //free(pt);
    }
	if(p)
    {
        free(p);
        DEBUG_("free p\n");
    }
#endif
}

unsigned int rs232_in_qeue(struct rs232_port_t *p, unsigned int *in_bytes)
{
	fd_set set;
	int ret;
	int b;
	struct timeval tv;
	struct rs232_posix_t *ux = &(p->pt);

//	DEBUG_("p=%p p->pt=%p\n", (void *)p, p->pt);

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	FD_ZERO(&set);
	FD_SET(ux->fd, &set);
	/* don't work reliable with 0 */
	tv.tv_usec = 1;
	tv.tv_sec = 0;

	select(ux->fd+1, &set, NULL, NULL, &tv);
	ret = ioctl(ux->fd, FIONREAD, &b);
	if (ret == -1) {
		*in_bytes = 0;
		DEBUG_("%s\n", "RS232_ERR_IOCTL");
		return RS232_ERR_IOCTL;
	}

	*in_bytes = b;
	DEBUG_("in_bytes=%d\n", b);

	return RS232_ERR_NOERROR;
}

void rs232_in_qeue_clear(struct rs232_port_t *p)
{
	fd_set set;
	unsigned int ret;
	unsigned int blen;
	unsigned char *buf = NULL;
	struct timeval tv;
	struct rs232_posix_t *ux = &(p->pt);

//	DEBUG_("p=%p p->pt=%p\n", (void *)p, p->pt);

	if (!rs232_port_open(p))
		return;

	rs232_in_qeue(p, &blen);
	if (blen > 0) {
		buf = (unsigned char*) malloc(blen * sizeof(unsigned char*)+1);
		if (buf == NULL)
			return;

		FD_ZERO(&set);
		FD_SET(ux->fd, &set);
		tv.tv_usec = 1;
		tv.tv_sec = 0;

		ret = select(ux->fd+1, &set, NULL, NULL, &tv);
		DEBUG_("select=%d\n", ret);
		if (ret <= 0) {
			free(buf);
			return;
		}

		ret = read(ux->fd, buf, blen);
		DEBUG_("read=%d\n", ret);
		free(buf);
	}
}

unsigned int rs232_read(struct rs232_port_t *p, unsigned char *buf, unsigned int buf_len,
	   unsigned int *read_len)
{
	int r;
	struct rs232_posix_t *ux = &(p->pt);

//	DEBUG_("p=%p p->pt=%p buf_len=%d\n", (void *)p, p->pt, buf_len);

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;
    DEBUG_("rs232 -read \n");
	r = read(ux->fd, buf, buf_len);
	if (r == -1) {
		*read_len = 0;
		DEBUG_("errno: %d strerror: %s %s\n",
		    errno, strerror(errno), "RS232_ERR_READ");

		return RS232_ERR_READ;
	}

	*read_len = r;

	return RS232_ERR_NOERROR;
}

static int duration(struct timeval *t1, struct timeval *t2)
{
	return (t2->tv_sec-t1->tv_sec)*1000 + (t2->tv_usec - t1->tv_usec)/1000;
}


unsigned int rs232_read_timeout_forced(struct rs232_port_t *p, unsigned char *buf,
		   unsigned int buf_len, unsigned int *read_len,
		   unsigned int timeout)
{
	int b;
	int ret;
	int reti;
	fd_set set;
	int r;
	struct rs232_posix_t *ux = &(p->pt);
	struct timeval tv;
	struct timeval t1;
	struct timeval t2;

//	DEBUG_("p=%p p->pt=%p buf_len=%d timeout=%d\n", (void *)p, p->pt, buf_len,
//	    timeout);

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	FD_ZERO(&set);
	FD_SET(ux->fd, &set);
	tv.tv_sec = (timeout * 1000) / 1000000;
	tv.tv_usec = (timeout * 1000) % 1000000;

	*read_len = 0;
	gettimeofday(&t1, NULL);
    DEBUG_("ux-> fd force = %d \n",ux->fd);
	while (1) {
		ret = select(ux->fd+1, &set, NULL, NULL, &tv);
		gettimeofday(&t2, NULL);

		if (ret == 0) {
			DEBUG_("%s\n", "select timeout");
			break;
		}

		if (ret == -1) {
			DEBUG_("%s\n", "select error");
			break;
		}

		if (duration(&t1, &t2) >= (int) timeout) {
			DEBUG_("%s\n", "timeout");
			break;
		}

		reti = ioctl(ux->fd, FIONREAD, &b);
		if (reti == -1) {
			DEBUG_("%s\n", "ioctl error");
			break;
		}

		if ((unsigned int) b >= buf_len) {
			DEBUG_("fionread=%d\n", b);
			break;
		}
	}

	switch (ret) {
	case 0:
		DEBUG_("%s\n", "RS232_ERR_TIMEOUT");
		return RS232_ERR_TIMEOUT;
	case 1:
		r = read(ux->fd, buf, buf_len);
		if (r == -1) {
			DEBUG_("errno: %d strerror: %s %s\n",
			    errno, strerror(errno), "RS232_ERR_READ");

			return RS232_ERR_READ;
		}

		*read_len = r;
		break;
	default:
		DEBUG_("%s\n", "RS232_ERR_SELECT");
		return RS232_ERR_SELECT;
	}

	return RS232_ERR_NOERROR;
}

unsigned int rs232_read_timeout(struct rs232_port_t *p, unsigned char *buf,
		   unsigned int buf_len, unsigned int *read_len,
		   unsigned int timeout)
{
	int ret;
	fd_set set;
	int r;
	struct timeval tv;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p buf_len=%d timeout=%d\n", (void *)p, p->pt,buf_len, timeout);

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	FD_ZERO(&set);
	FD_SET(ux->fd, &set);
	tv.tv_sec = (timeout * 1000) / 1000000;
	tv.tv_usec = (timeout * 1000) % 1000000;
	*read_len = 0;
    DEBUG_("read-fd = %d \n",ux->fd);
	ret = select(ux->fd+1, &set, NULL, NULL, &tv);
	switch (ret) {
	case 0:
		DEBUG_("%s\n", "RS232_ERR_TIMEOUT");
		return RS232_ERR_TIMEOUT;
	case 1:
		r = read(ux->fd, buf, buf_len);
		if (r == -1) {
			DEBUG_("errno: %d strerror: %s %s\n",
			    errno, strerror(errno), "RS232_ERR_READ");
			return RS232_ERR_READ;
		}

		*read_len = r;
		break;
	default:
		DEBUG_("%s\n", "RS232_ERR_SELECT");
		return RS232_ERR_SELECT;
	}

	return RS232_ERR_NOERROR;
}

unsigned int rs232_write(struct rs232_port_t *p, const unsigned char *buf, unsigned int buf_len,
		unsigned int *write_len)
{
	int w;
	struct rs232_posix_t *ux = &(p->pt);

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	w = write(ux->fd, buf, buf_len);
	if (w == -1) {
		DEBUG_("errno: %d strerror: %s %s\n",
		    errno, strerror(errno), "RS232_ERR_WRITE");

		*write_len = 0;
		return RS232_ERR_WRITE;
	}

	*write_len = w;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_write_timeout(struct rs232_port_t *p, const unsigned char *buf,
			unsigned int buf_len, unsigned int *write_len,
			unsigned int timeout)
{
	int ret;
	fd_set set;
	int w;
	struct rs232_posix_t *ux = &(p->pt);
	struct timeval tv;

//	DEBUG_("p=%p p->pt=%p timeout=%d\n", (void *)p, p->pt, timeout);

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	FD_ZERO(&set);
	FD_SET(ux->fd, &set);
	tv.tv_sec = (timeout * 1000) / 1000000;
	tv.tv_usec = (timeout * 1000) % 1000000;
	*write_len = 0;

	ret = select(ux->fd+1, NULL, &set, NULL, &tv);
	switch (ret) {
	case 0:
		DEBUG_("%s\n", "RS232_ERR_TIMEOUT");
		return RS232_ERR_TIMEOUT;
	case 1:
		w = write(ux->fd, buf, buf_len);
		if (w == -1) {
			DEBUG_("errno: %d strerror: %s %s\n",
			    errno, strerror(errno), "RS232_ERR_WRITE");

			return RS232_ERR_WRITE;
		}

		*write_len = w;
		break;
	default:
		DEBUG_("%s\n", "RS232_ERR_SELECT");
		return RS232_ERR_SELECT;
	}

	return RS232_ERR_NOERROR;
}

unsigned int rs232_open(struct rs232_port_t *p)
{
	int flags;
	struct termios term;
	struct rs232_posix_t *ux = &(p->pt);

//	DEBUG_("p=%p p->pt=%p\n", (void *)p, p->pt);

	ux->fd = open(p->dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if (ux->fd < 0) {
		DEBUG_("open() %d %s\n", errno, strerror(errno));
		return RS232_ERR_OPEN;
	}

	/*
	 * On OSX (and maybe on more systems), we need to open() the port with
	 * O_NDELAY, because otherwise it would block forever waiting for DCD
	 * signal, so here we restore back to blocking operations.
	 */
    DEBUG_("open-fd = %d\n",ux->fd);
	flags = fcntl(ux->fd, F_GETFL);
	flags &= ~O_NDELAY;
	fcntl(ux->fd, F_SETFL, flags);

	if (tcflush(ux->fd, TCIOFLUSH) < 0) {
		DEBUG_("tcflush() %d %s\n", errno, strerror(errno));
		return RS232_ERR_CONFIG;
	}

	GET_PORT_STATE(ux->fd, &term)
	GET_PORT_STATE(ux->fd, &ux->oldterm)

	term.c_cflag |= (CREAD | CLOCAL);
	term.c_iflag = IGNPAR;
	term.c_oflag = 0;
	term.c_lflag = 0;
	term.c_cc[VINTR]  = _POSIX_VDISABLE;
	term.c_cc[VQUIT]  = _POSIX_VDISABLE;
	term.c_cc[VSTART] = _POSIX_VDISABLE;
	term.c_cc[VSTOP]  = _POSIX_VDISABLE;
	term.c_cc[VSUSP]  = _POSIX_VDISABLE;
	term.c_cc[VEOF]   = _POSIX_VDISABLE;
	term.c_cc[VEOL]   = _POSIX_VDISABLE;
	term.c_cc[VERASE] = _POSIX_VDISABLE;
	term.c_cc[VKILL]  = _POSIX_VDISABLE;

	SET_PORT_STATE(ux->fd, &term)

	rs232_set_baud(p, p->baud);
	rs232_set_data(p, p->data);
	rs232_set_parity(p, p->parity);
	rs232_set_stop(p, p->stop);
	rs232_set_flow(p, p->flow);
	p->status = RS232_PORT_OPEN;

	return RS232_ERR_NOERROR;
}

void rs232_set_device(struct rs232_port_t *p, const char *device)
{
	//DEBUG_("p=%p old=%s new=%s\n", (void *)p, p->dev, device);
	strncpy(p->dev, device, RS232_STRLEN_DEVICE);
	return;
}

unsigned int rs232_set_baud(struct rs232_port_t *p, enum rs232_baud_e baud)
{
	struct termios term;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p baud=%d (%s bauds)\n",(void *)p, p->pt, baud, rs232_strbaud(baud));

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

    DEBUG_("baud-fd = %d \n",ux->fd);
	GET_PORT_STATE(ux->fd, &term)

	switch (baud) {
	case RS232_BAUD_300:
		cfsetispeed(&term, B300);
		cfsetospeed(&term, B300);
		break;
	case RS232_BAUD_2400:
		cfsetispeed(&term, B2400);
		cfsetospeed(&term, B2400);
		break;
	case RS232_BAUD_4800:
		cfsetispeed(&term, B4800);
		cfsetospeed(&term, B4800);
		break;
	case RS232_BAUD_9600:
		cfsetispeed(&term, B9600);
		cfsetospeed(&term, B9600);
		break;
	case RS232_BAUD_19200:
		cfsetispeed(&term, B19200);
		cfsetospeed(&term, B19200);
		break;
	case RS232_BAUD_38400:
		cfsetispeed(&term, B38400);
		cfsetospeed(&term, B38400);
		break;
	case RS232_BAUD_57600:
		cfsetispeed(&term, B57600);
		cfsetospeed(&term, B57600);
		break;
	case RS232_BAUD_115200:
		cfsetispeed(&term, B115200);
		cfsetospeed(&term, B115200);
		break;
	case RS232_BAUD_460800:
		cfsetispeed(&term, B460800);
		cfsetospeed(&term, B460800);
		break;
	default:
		return RS232_ERR_UNKNOWN;
	}

	SET_PORT_STATE(ux->fd, &term)
	p->baud = baud;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_set_dtr(struct rs232_port_t *p, enum rs232_dtr_e state)
{
	int ret;
	int set;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p dtr=%d (dtr control %s)\n",(void *)p, p->pt, state, rs232_strdtr(state));

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	ret = ioctl(ux->fd, TIOCMGET, &set);
	if (ret == -1) {
		DEBUG_("%s\n", "TIOCMGET RS232_ERR_IOCTL");
		return RS232_ERR_IOCTL;
	}

	switch (state) {
	case RS232_DTR_OFF:
		set &= ~TIOCM_DTR;
		break;
	case RS232_DTR_ON:
		set |= TIOCM_DTR;
		break;
	default:
		return RS232_ERR_UNKNOWN;
	}

	ret = ioctl(ux->fd, TIOCMSET, &set);
	if (ret == -1) {
		DEBUG_("%s\n", "TIOCMSET RS232_ERR_IOCTL");
		return RS232_ERR_IOCTL;
	}

	p->dtr = state;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_set_rts(struct rs232_port_t *p, enum rs232_rts_e state)
{
	int ret;
	int set;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p rts=%d (rts control %s)\n",(void *)p, p->pt, state, rs232_strrts(state));

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	ret = ioctl(ux->fd, TIOCMGET, &set);
	if (ret == -1) {
		DEBUG_("%s\n", "TIOCMGET RS232_ERR_IOCTL");
		return RS232_ERR_IOCTL;
	}

	switch (state) {
	case RS232_RTS_OFF:
		set &= ~TIOCM_RTS;
		break;
	case RS232_RTS_ON:
		set |= TIOCM_RTS;
		break;
	default:
		return RS232_ERR_UNKNOWN;
	}

	ret = ioctl(ux->fd, TIOCMSET, &set);
	if (ret == -1) {
		DEBUG_("%s\n", "TIOCMSET RS232_ERR_IOCTL");
		return RS232_ERR_IOCTL;
	}

	p->rts = state;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_set_parity(struct rs232_port_t *p, enum rs232_parity_e parity)
{
	struct termios term;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p parity=%d (parity %s)\n",(void *)p, p->pt, parity, rs232_strparity(parity));

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	GET_PORT_STATE(ux->fd, &term)

	switch (parity) {
	case RS232_PARITY_NONE:
		term.c_cflag &= ~PARENB;
		break;
	case RS232_PARITY_ODD:
		term.c_cflag |= (PARENB | PARODD);
		break;
	case RS232_PARITY_EVEN:
		term.c_cflag &= ~PARODD;
		term.c_cflag |= PARENB;
		break;
	default:
		return RS232_ERR_UNKNOWN;
	}

	SET_PORT_STATE(ux->fd, &term)
	p->parity = parity;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_set_stop(struct rs232_port_t *p, enum rs232_stop_e stop)
{
	struct termios term;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p stop=%d (%s stop bits)\n",(void *)p, p->pt, stop, rs232_strstop(stop));

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	GET_PORT_STATE(ux->fd, &term)
	term.c_cflag &= ~CSTOPB;

	switch (stop) {
	case RS232_STOP_1:
		break;
	case RS232_STOP_2:
		term.c_cflag |= CSTOPB;
		break;
	default:
		return RS232_ERR_UNKNOWN;
	}

	SET_PORT_STATE(ux->fd, &term)
	p->stop = stop;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_set_data(struct rs232_port_t *p, enum rs232_data_e data)
{
	struct termios term;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p data=%d (%s data bits)\n",(void *)p, p->pt, data, rs232_strdata(data));

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	GET_PORT_STATE(ux->fd, &term)
	term.c_cflag &= ~CSIZE;

	switch (data) {
	case RS232_DATA_5:
		term.c_cflag |= CS5;
		break;
	case RS232_DATA_6:
		term.c_cflag |= CS6;
		break;
	case RS232_DATA_7:
		term.c_cflag |= CS7;
		break;
	case RS232_DATA_8:
		term.c_cflag |= CS8;
		break;
	default:
		return RS232_ERR_UNKNOWN;
	}

	SET_PORT_STATE(ux->fd, &term)
	p->data = data;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_set_flow(struct rs232_port_t *p, enum rs232_flow_e flow)
{
	struct termios term;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p flow=%d (flow control %s)\n",(void *)p, p->pt, flow, rs232_strflow(flow));

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	GET_PORT_STATE(ux->fd, &term)

	switch (flow) {
	case RS232_FLOW_OFF:
		term.c_cflag &= ~CRTSCTS;
		term.c_iflag &= ~(IXON | IXOFF | IXANY);
		break;
	case RS232_FLOW_HW:
		term.c_cflag |= CRTSCTS;
		term.c_iflag &= ~(IXON | IXOFF | IXANY);
		break;
	case RS232_FLOW_XON_XOFF:
		term.c_cflag &= ~CRTSCTS;
		term.c_iflag |= (IXON | IXOFF | IXANY);
		break;
	default:
		return RS232_ERR_UNKNOWN;
	}

	SET_PORT_STATE(ux->fd, &term)
	p->flow = flow;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_flush(struct rs232_port_t *p)
{
	int ret;
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p\n", (void *)p, p->pt);

	if (!rs232_port_open(p))
		return RS232_ERR_PORT_CLOSED;

	ret = tcflush(ux->fd, TCIOFLUSH);
	if (ret == -1)
		return RS232_ERR_FLUSH;

	return RS232_ERR_NOERROR;
}

unsigned int rs232_close(struct rs232_port_t *p)
{
	int ret;
	//struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p\n", (void *)p, p->pt);
    if(rs232_G == NULL)
        return RS232_PORT_CLOSED;
	//if (!rs232_port_open(p))
      if(!rs232_port_open(rs232_G))
		return RS232_ERR_PORT_CLOSED;

    DEBUG_("close-fd = %d \n",rs232_G->pt.fd);

	//ret = close(ux->fd);
	ret = close(rs232_G->pt.fd);
	if (ret == -1)
		return RS232_ERR_CLOSE;

	//p->status = RS232_PORT_CLOSED;
	rs232_G->status = RS232_PORT_CLOSED;
	return RS232_ERR_NOERROR;
}

unsigned int rs232_fd(struct rs232_port_t *p)
{
	struct rs232_posix_t *ux = &(p->pt);

	//DEBUG_("p=%p p->pt=%p ux->fd=%d\n", (void *)p, p->pt, ux->fd);

	return (unsigned int) ux->fd;
}


static const char *rs232_baud[] = {
	"300",
	"2400",
	"4800",
	"9600",
	"19200",
	"38400",
	"57600",
	"115200",
	"460800",
};

static const char *rs232_data[] = {
	"5",
	"6",
	"7",
	"8",
};

static const char *rs232_parity[] = {
	"none",
	"odd",
	"even",
};

static const char *rs232_stop[] = {
	"1",
	"2",
};

static const char *rs232_flow[] = {
	"off",
	"hardware",
	"xon/xoff",
};

static const char *rs232_dtr[] = {
	"off",
	"on",
};

static const char *rs232_rts[] = {
	"off",
	"on",
};

static const char *rs232_error[] = {
	"no error",
	"unknown error",
	"open error",
	"close error",
	"flush error",
	"get/set settings error",
	"read error",
	"write error",
	"select error",
	"timeout error",
	"ioctl error",
	"port closed error",
};


const char *rs232_strerror(unsigned int error)
{
	DEBUG_("error=%d\n", error);

	if (error >= RS232_ERR_MAX)
		return NULL;

	return rs232_error[error];
}

const char *rs232_strbaud(unsigned int baud)
{
	DEBUG_("baud=%d\n", baud);

	if (baud >= RS232_BAUD_MAX)
		return NULL;

	return rs232_baud[baud];
}

const char *rs232_strdata(unsigned int data)
{
	DEBUG_("data=%d\n", data);

	if (data >= RS232_DATA_MAX)
		return NULL;

	return rs232_data[data];
}

const char *rs232_strparity(unsigned int parity)
{
	DEBUG_("parity=%d\n", parity);

	if (parity >= RS232_PARITY_MAX)
		return NULL;

	return rs232_parity[parity];
}

const char *rs232_strstop(unsigned int stop)
{
	DEBUG_("stop=%d\n", stop);

	if (stop >= RS232_STOP_MAX)
		return NULL;

	return rs232_stop[stop];
}

const char *rs232_strflow(unsigned int flow)
{
	DEBUG_("flow=%d\n", flow);

	if (flow >= RS232_FLOW_MAX)
		return NULL;

	return rs232_flow[flow];
}

const char *rs232_strdtr(unsigned int dtr)
{
	DEBUG_("dtr=%d\n", dtr);

	if (dtr >= RS232_DTR_MAX)
		return NULL;

	return rs232_dtr[dtr];
}

const char *rs232_strrts(unsigned int rts)
{
	DEBUG_("rts=%d\n", rts);

	if (rts >= RS232_RTS_MAX)
		return NULL;

	return rs232_rts[rts];
}

const char *rs232_to_string(struct rs232_port_t *p)
{
	static char str[RS232_STRLEN+1];

	DEBUG_("p=%p\n", (void *)p);

	if (p == NULL)
		return NULL;

	snprintf(str, RS232_STRLEN, "device: %s, baud: %s, data bits: %s,"
					" parity: %s, stop bits: %s,"
					" flow control: %s",
					p->dev,
					rs232_strbaud(p->baud),
					rs232_strdata(p->data),
					rs232_strparity(p->parity),
					rs232_strstop(p->stop),
					rs232_strflow(p->flow));

	return str;
}

const char *rs232_get_device(struct rs232_port_t *p)
{
	//DEBUG_("p=%p device: %s\n", (void *)p, p->dev);
	return p->dev;
}

unsigned int rs232_get_baud(struct rs232_port_t *p)
{
	//DEBUG_("p=%p baud: %d\n", (void *)p, p->baud);
	return p->baud;
}

unsigned int rs232_get_stop(struct rs232_port_t *p)
{
	//DEBUG_("p=%p baud: %d\n", (void *)p, p->stop);
	return p->stop;
}

unsigned int rs232_get_data(struct rs232_port_t *p)
{
	//DEBUG_("p=%p data: %d\n", (void *)p, p->data);
	return p->data;

}

unsigned int rs232_get_parity(struct rs232_port_t *p)
{
	//DEBUG_("p=%p parity: %d\n", (void *)p, p->parity);
	return p->parity;
}

unsigned int rs232_get_flow(struct rs232_port_t *p)
{
	//DEBUG_("p=%p flow: %d\n", (void *)p, p->flow);
	return p->flow;
}

unsigned int rs232_port_open(struct rs232_port_t *p)
{
	//DEBUG_("p=%p p->status=%d --open\n", (void *)p, p->status);
	return p->status;
}

unsigned int rs232_get_dtr(struct rs232_port_t *p)
{
	//DEBUG_("p=%p dtr: %d\n", (void *)p, p->dtr);
	return p->dtr;
}

unsigned int rs232_get_rts(struct rs232_port_t *p)
{
	//DEBUG_("p=%p rts: %d\n", (void *)p, p->rts);
	return p->rts;
}

