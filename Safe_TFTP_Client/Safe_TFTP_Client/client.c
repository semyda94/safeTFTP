#include "../../include/general_data.h"
#include "../../include/get.h"

int main(void)
{
	struct sockaddr_in server;
    struct timeval tv;
	int s, i, count_servers, file = -1;
    message_t msg;
    server_data_t server_data;
    list_data_server_t list_data_server;
	socklen_t slen = sizeof(server);
    char *filename;
    off_t flen, recv_len;
    fd_set rfds;
    
    filename = malloc(sizeof(char) * 30);
    
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
		fprintf(stderr, "Error create sock");
	}

    server_data.main_port = 2009;
    strcpy(server_data.main_ip_address, "127.0.0.1");
    
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(server_data.main_port);
	server.sin_addr.s_addr = inet_addr(server_data.main_ip_address);
    
    printf("Hello, wating conecting to server ...\n");
    
    get_msg("Conect client", &msg, &server_data);
	sendto(s, &msg, sizeof(msg), 0, (struct sockaddr *)&server, sizeof(server));
    
    recvfrom(s, &count_servers, sizeof(int), 0, (struct sockaddr*)&server, &slen);
    printf("Count servers     :%d\n", count_servers);
    recvfrom(s, &list_data_server, sizeof(list_data_server), 0, (struct sockaddr*)&server, &slen);
    for (i = 0; i < count_servers; i++) {
        printf("%d) %s:%d\n", i + 1, list_data_server.s_addr[i], list_data_server.s_port[i]);
    }
    
    while (1) {
        recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr*)&server, &slen);
        if (0 == strcmp("File open good", msg.buf))
            break;
        printf("%s ", msg.buf);
        scanf("%s", filename);
    
        get_msg(filename, &msg, &server_data);
        sendto(s, &msg, sizeof(msg), 0, (struct sockaddr*)&server, sizeof(server));
    }
    
    recvfrom(s, &flen, sizeof(flen), 0, (struct sockaddr*)&server, &slen);

    file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file < 0)
        printf("Error open file\n");
    else
        printf("File %s open\n", filename);
    
    msg.size = sizeof(msg.buf);
    
    fprintf(stdout, "Start download ...\n [");
    fflush (stdout);
    
    tv.tv_sec = 6;
    tv.tv_usec = 0;
    
    FD_ZERO(&rfds);
    FD_SET(s, &rfds);
    
    while (1) {
        bzero(&msg.buf, sizeof(msg.buf));
        select(s + 1, &rfds, NULL, NULL, &tv);
        if (FD_ISSET(s, &rfds)){
            recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr*)&server, &slen);
            recv_len += sizeof(msg.buf);
            check_len(flen, &recv_len);
            if (msg.size == 512) {
                write(file, &msg.buf, sizeof(msg.buf));
            } else {
                write(file, &msg.buf, msg.size);
                break;
            }
        } else {
            fprintf(stderr, "\nError connect\n");
            exit(-1);
        }
    }
    printf("]\nDownload finish\n");
    close(s);
	return 0;
}
