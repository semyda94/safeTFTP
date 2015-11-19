//
//  main_server.c
//  safeTFTP
//
//  Created by Дима Семыкин on 05.11.15.
//  Copyright © 2015 Дима Семыкин. All rights reserved.
//

#include "main_server.h"

int Mserver(server_data_t * server_data)
{
    int i, j, s;
    message_t msg;
    socklen_t slen;
    struct sockaddr_in main_server, reserv_servers[server_data->count_servers-1], client;
    list_data_server_t list_data_server;
    int file = -1;
    char *filename;
    off_t flen, send_len;
    
    filename = malloc(sizeof(char) * 30);
    
    if ((s=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
        fprintf(stderr, "Error: Create socket\n");
        return -1;
    }
    
    bzero(&main_server, sizeof(main_server));
    main_server.sin_family = AF_INET;
    main_server.sin_port = htons(server_data->main_port);
    main_server.sin_addr.s_addr = inet_addr(server_data->main_ip_address);
    if (bind(s, (const struct sockaddr *)&main_server, sizeof(main_server)) == -1) {
        fprintf(stderr, "Error: Bind server\n");
        return -1;
    }
    listen(s, 10);
    
    list_data_server.s_port[0] = server_data->main_port;
    strcpy(list_data_server.s_addr[0], server_data->main_ip_address);
    
    printf("Wait coneccted reserv servers!\n");
    for (i = 0; i < server_data->count_servers - 1; i++) {
        recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr *)&reserv_servers[i], &slen);
        
        printf("Conected server %s:%d\n", msg.s_addr, msg.s_port);
        list_data_server.s_port[i + 1] = msg.s_port;
        strcpy(list_data_server.s_addr[i + 1], msg.s_addr);
        
        get_msg("Good connect", &msg, server_data);
        sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
    }
    
    for (i = 0; i < server_data->count_servers - 1; i++) {
        sendto(s, &reserv_servers, sizeof(reserv_servers[i]), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
    }
    
    printf("List servers\n");
    for (i = 0; i < server_data->count_servers; i++) {
        printf("%d) %s:%d\n", i+1, list_data_server.s_addr[i], list_data_server.s_port[i]);
    }
    
    for (i = 0; i < server_data->count_servers - 1; i++) {
        sendto(s, &list_data_server, sizeof(list_data_server), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
    }
    printf("Wait conected client\n");
    
    recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr *) &client, &slen);
    
    sendto(s, &server_data->count_servers, sizeof(int), 0, (struct sockaddr *)&client, sizeof(client));
    sendto(s, &list_data_server, sizeof(list_data_server), 0, (struct sockaddr *)&client, sizeof(client));
    
    printf("(:%d)%s\n", htons(client.sin_port), msg.buf);
    
    get_msg("Client connected!\n", &msg, server_data);
    
    
    for (i = 0; i < server_data->count_servers - 1; i++) {
        sendto(s, &client, sizeof(client), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
        sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
    }
    
    
    while (file < 0) {
    
        get_msg("Enter file name:", &msg, server_data);
        sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&client, sizeof(client));
    
        recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr*)&client, &slen);
        printf("Client enter name: %s\n", msg.buf);
        
        file = open(msg.buf, O_RDONLY, 0666);
        if (file < 0)
            printf("Error open file\n");
    }
    
    flen = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);
    
    for (i = 0; i < server_data->count_servers - 1; i++) {
        sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
        sendto(s, &flen, sizeof(flen), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
    }
    
    get_msg("File open good", &msg, server_data);
    sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&client, sizeof(client));
    
    sendto(s, &flen, sizeof(flen), 0, (struct sockaddr*)&client, sizeof(client));
    msg.size = sizeof(msg.buf);
    printf("Begining download...\n");
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
    
    for (i = 0; i < server_data->count_servers - 1; i++) {
        sendto(s, &send_len, sizeof(send_len), 0, (struct sockaddr*)&reserv_servers[i], sizeof(reserv_servers[i]));
    }
    printf("Finishing download\n");
    
    close(s);
    return 0;
}
