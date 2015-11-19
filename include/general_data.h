#ifndef __GENERAL_DATA_H__
#define __GENERAL_DATA_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>

int flg_init;

typedef struct
{
    char s_addr[10][17];
    uint16_t  s_port[10];
} list_data_server_t;

typedef struct
{
	int count_servers;
	char main_ip_address[17];
	uint16_t main_port;
	char ip_address[17];
	uint16_t port;
} server_data_t;

typedef struct
{
    char c_addr[17];
    uint16_t c_port;
} client_data_t;

typedef struct
{
	char buf[512];
    char s_addr[17];
    uint16_t s_port;
    char c_addr[17];
    off_t size;
} message_t;

#endif