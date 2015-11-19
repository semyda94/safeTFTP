//
//  get.c
//  Safe_TFTP_Server
//
//  Created by Дима Семыкин on 07.11.15.
//  Copyright © 2015 Дима Семыкин. All rights reserved.
//

#include "get.h"

int get_msg(char *buf, message_t *msg, server_data_t *server_data){

    strcpy(msg->buf, buf);
    msg->s_port = server_data->port;
    strcpy(msg->s_addr, server_data->ip_address);
    
    return 0;
}

void check_len(off_t flen, off_t *recv_len){
    off_t proc = (flen / 200);
    
    if (proc <= *recv_len) {
        fprintf(stdout, "#");
        fflush (stdout);
        *recv_len = 0 - *recv_len - proc;
    }
}

off_t recv_file(int s, off_t flen, off_t recv_len, list_data_server_t list_data_server,
               server_data_t *server_data, struct sockaddr_in main_server, struct sockaddr_in my_server)
{
    socklen_t slen;
    fd_set rfds;
    struct timeval tv;
    off_t all_recv_len = 0;
    message_t msg;
    
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    
    FD_ZERO(&rfds);
    FD_SET(s, &rfds);
    
    while (1) {
        
        select(s + 1, &rfds, NULL, NULL, &tv);
        if (FD_ISSET(s, &rfds)){
            recvfrom(s, &recv_len, sizeof(recv_len), 0, (struct sockaddr*) &main_server, &slen);
            /*printf("recv from %hu", ntohs(main_server.sin_port));*/
            all_recv_len = all_recv_len + recv_len;
            if (flen == all_recv_len) {
                break;
            }
        } else {
            int return_code;
            return_code = choise_main_server(s, list_data_server, server_data, &main_server, &my_server);
            if (1 == return_code) {
                return all_recv_len;
                break;
            } else {
                printf("Reconecter with %s:%hu\n", list_data_server.s_addr[server_data->count_servers], list_data_server.s_port[server_data->count_servers]);
                main_server.sin_port = htons(list_data_server.s_port[server_data->count_servers]);
                main_server.sin_addr.s_addr = inet_addr(list_data_server.s_addr[server_data->count_servers]);
                get_msg("Reconnect", &msg, server_data);
                sendto(s, &msg.buf, sizeof(msg.buf), 0, (struct sockaddr*)&main_server, sizeof(main_server));
                recvfrom(s, &msg.buf, sizeof(msg.buf), 0, (struct sockaddr*)&main_server, &slen);
                printf("%s\n", msg.buf);
            }
        }
    }
    return all_recv_len;
}

int choise_main_server(int s, list_data_server_t list_data_server, server_data_t *server_data, struct sockaddr_in *main_server, struct sockaddr_in *my_server)
{
    if ((strcmp(list_data_server.s_addr[server_data->count_servers-1], server_data->ip_address)) == 0 &&
        list_data_server.s_port[server_data->count_servers-1] == server_data->port &&
        server_data->count_servers != 0) {
        
        server_data->count_servers -= 1;
        printf("This server now be main\n");
        return 1;
    } else {
        printf("This server now be reserv\n");
        server_data->count_servers -= 1;
        return 2;
    }
}