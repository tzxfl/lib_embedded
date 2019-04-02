#ifndef LPI_NETWORK_H
#define LPI_NETWORK_H


enum{NET_TYPE_ETH,NET_TYPE_4G,NET_TYPE_LOOP,NET_TYPE_NONE};

int net_set_nonblock(int sockfd);

int net_get_ipaddr(const char *devname,char *result);

int net_get_macaddr(const char *devname,char *result);

int net_get_nettype(const char *devname);

int net_get_destaddr(const char *devname,char *result);

int net_get_mask(const char *devname,char *result);

int net_get_ipbydomain(const char *devname,char *result);
/**check net name is or not exist.. ppp0 **/
int net_check_netname(const char *devname);

int net_access(const char *name);

int net_test_constatus(const char *ipaddr,unsigned int port);

int net_interface_up(const char *devname);

int net_interface_down(const char *devname);

int net_ping(const char *ip,unsigned int port,const int count,const int packsize);

#endif // LPI_NETWORK_H
