//
//  get.h
//  Safe_TFTP_Server
//
//  Created by Дима Семыкин on 07.11.15.
//  Copyright © 2015 Дима Семыкин. All rights reserved.
//

#ifndef get_h
#define get_h

#include <stdio.h>
#include "general_data.h"

int get_msg(char *buf, message_t *msg, server_data_t *server_data);
void check_len(off_t flen, off_t *recv_len);
int choise_main_server(int s, list_data_server_t list_data_server, server_data_t *server_data, struct sockaddr_in *main_server,
                        struct sockaddr_in *my_server);
off_t recv_file(int s, off_t flen, off_t recv_len, list_data_server_t list_data_server,
               server_data_t *server_data, struct sockaddr_in main_server, struct sockaddr_in my_server);
#endif /* get_h */
