//
//  reserv_server.c
//  Safe_TFTP_Server
//
//  Created by Дима Семыкин on 06.11.15.
//  Copyright © 2015 Дима Семыкин. All rights reserved.
//

#include "reserv_server.h"

int Rserver(server_data_t * server_data)
{
    int i, s, file;
    message_t msg;
    struct sockaddr_in main_server, my_server, reserv_servers[server_data->count_servers-1], client;
    struct timeval tv;
    list_data_server_t list_data_server;
    socklen_t slen;
    unsigned long count;
    off_t flen, recv_len;
    fd_set rfds;
    
    if ((s=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
        fprintf(stderr, "Error: Create socket\n");
        return -1;
    }
				
    bzero(&main_server, sizeof(main_server));
    bzero(&my_server, sizeof(my_server));

    main_server.sin_family = AF_INET;
    main_server.sin_port = htons(server_data->main_port);
    main_server.sin_addr.s_addr = inet_addr(server_data->main_ip_address);
    
    my_server.sin_family = AF_INET;
    my_server.sin_port = htons(server_data->port);
    my_server.sin_addr.s_addr = inet_addr(server_data->ip_address);
    
    if (bind(s, (const struct sockaddr *)&my_server, sizeof(my_server)) == -1) {
        fprintf(stderr, "Error: Bind server\n");
        return -1;
    }
    
    listen(s, 10);
    
    slen = sizeof(main_server);

    strcpy(msg.s_addr, server_data->ip_address);
    msg.s_port = server_data->port;
                                     
    sendto(s, &msg, sizeof(msg), 0, (struct sockaddr *)&main_server, slen);
    recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr*)&main_server, &slen);
    printf("Reply main server(%s:%d): %s\n", msg.s_addr, msg.s_port, msg.buf);
    printf("Wait data about other servers!\n");
    
    recvfrom(s, &reserv_servers, sizeof(reserv_servers), 0, (struct sockaddr*) &main_server, &slen);
    
    recvfrom(s, &list_data_server, sizeof(list_data_server), 0, (struct sockaddr*)&main_server, &slen);
    
    printf("List servers\n");
    for (i = 0; i < server_data->count_servers; i++) {
        printf("%d) %s:%d\n", i+1, list_data_server.s_addr[i], list_data_server.s_port[i]);
    }
    
    printf("Wait conected client\n");
    
    recvfrom(s, &client, sizeof(client), 0, (struct sockaddr*) &main_server, &slen);
    recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr*) &main_server, &slen);

    printf("(%s:%d) - %s\n", msg.s_addr, msg.s_port, msg.buf);
    
    printf("Wait geting Filename\n");
    
    recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr*) &main_server, &slen);
    recvfrom(s, &flen, sizeof(flen), 0, (struct sockaddr*) &main_server, &slen);
    
    printf("(%s:%d) - Client enter name: %s (size : %lld) \n", msg.s_addr, msg.s_port, msg.buf, flen);
    file = open(msg.buf, O_RDONLY, 0666);
    flen = lseek(file, 0, SEEK_END);
    /*tv.tv_sec = 3;
    tv.tv_usec = 0;
    
    FD_ZERO(&rfds);
    FD_SET(s, &rfds);*/
    printf("Begining download...\n");
    recv_len = recv_file(s, flen, recv_len, list_data_server, server_data, main_server, my_server);
    if (recv_len != flen){
        printf("Enter reserv work\n");
        off_t send_len;
        
        for (i =0; i < server_data->count_servers -1 ; i++) {
            recvfrom(s, &msg.buf, sizeof(msg.buf), 0, (struct sockaddr*)&reserv_servers[i], &slen);
            printf("(%s:%d) - Reconnected\n", msg.buf, htons(reserv_servers[i].sin_port));
            get_msg("Good reconnect", &msg, server_data);
            sendto(s, msg.buf, sizeof(msg.buf), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
        }
        
        msg.size = sizeof(msg.buf);
        flen -= recv_len;
        lseek(file, recv_len, SEEK_SET);
        while (flen >= msg.size) {
            bzero(&msg.buf, sizeof(msg.buf));
            read(file, &msg.buf, sizeof(msg.buf));
            usleep(100);
            sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&client, sizeof(client));
            send_len = sizeof(msg.buf);
            for (i = 0; i < server_data->count_servers - 1; i++) {
                sendto(s, &send_len, sizeof(send_len), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
            }
            flen = flen - sizeof(msg.buf);
        }
        
        bzero(&msg.buf, sizeof(msg.buf));
        read(file, &msg.buf, flen);
        msg.size = flen;
        sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&client, sizeof(client));
        send_len = msg.size;
    }
    /*while (1) {
        select(s + 1, &rfds, NULL, NULL, &tv);
        if (FD_ISSET(s, &rfds)){
            recvfrom(s, &recv_len, sizeof(recv_len), 0, (struct sockaddr*) &main_server, &slen);
            all_recv_len = all_recv_len + recv_len;
            if (flen == all_recv_len) {
                break;
            }
        } else {
            choise_main_server(list_data_server, server_data, &main_server, &my_server);
            if ((strcmp(list_data_server.s_addr[server_data->count_servers], server_data->ip_address)) == 0 &&
                list_data_server.s_port[server_data->count_servers] == server_data->port) {
                
            }
        }
    }*/
    printf("Finishing download\nEnd work with client\n");
    return 0;
}