#ifndef NET_H
#define NET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <getopt.h> // getopt
#include <stdio.h>  // stderr
#include <stdlib.h> // strtoul
#include <errno.h>  // errno
#include <string.h> // strerror

#define UDP_EOD '\n'

int parse_optarg_client(int argc, char *argv[], in_addr_t *dst_addr, in_port_t *dst_port);
int connect_tcp_client(in_addr_t *dst_addr, in_port_t *dst_port);
int connect_udp_client(in_addr_t *dst_addr, in_port_t *dst_port);

#endif
