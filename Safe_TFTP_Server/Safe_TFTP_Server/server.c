#include "../../include/general_data.h"
#include "../../include/get.h"
#include "main_server.h"
#include "reserv_server.h"

int check(int argc, char ** argv, server_data_t * server_data)
{
	
	
	if (argc < 4) {
		fprintf(stderr, "Error: Small number of arguments\nEnter the arguments:\n1)The number of servers\n2)IP address server\n3)Server port\n4)General server port\n");
		return -1;
	}
	
    server_data->count_servers 		= atoi(argv[1]);
    strcpy(server_data->main_ip_address, argv[2]);
    server_data->main_port			= atoi(argv[3]);
	if (argc < 5){
        strcpy(server_data->ip_address, server_data->main_ip_address);
		server_data->port			= server_data->main_port;
        printf("This is main server\n");
	} else {
		strcpy(server_data->ip_address, argv[4]);
		server_data->port			= atoi(argv[5]);
        printf("This is reserv server\n");
	}
    
	printf("Count servers    		: %d\n", server_data->count_servers);
	printf("IP address main server	: %s\n", server_data->main_ip_address);
	printf("Port main server		: %d\n", server_data->main_port);
	printf("IP address this server  : %s\n", server_data->ip_address);
	printf("Port this server		: %d\n", server_data->port);
	return 0;
}




int main(int argc, char ** argv)
{
	server_data_t server_data;
	
	check(argc, argv, &server_data);
		
	if (server_data.port == server_data.main_port /*&& server_data.ip_address == server_data.main_ip_address*/) {
		Mserver(&server_data);
	} else {
		Rserver(&server_data);
	}
	

	return 0;
}
