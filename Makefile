BUILD_DIR_SERVER = $(PWD)/Server_space
BUILD_DIR_CLIENT = $(PWD)/Client_space
PATH_CLIENT = $(PWD)/Safe_TFTP_Client/Safe_TFTP_Client
PATH_SERVER = $(PWD)/Safe_TFTP_Server/Safe_TFTP_Server
NAME_SERVER = Safe_TFTP_Server
NAME_CLIENT = Safe_TFTP_Client
INCLUDE_PATH = -I$(PWD)/safeTFTP/include

.PHONY: all
all: server client
		mkdir -p $(BUILD_DIR_CLIENT)
		mkdir -p $(BUILD_DIR_SERVER)
		cd $(PATH_SERVER); \
		cp $(PATH_SERVER)/$(NAME_SERVER) $(BUILD_DIR_SERVER); \
		cp $(PATH_CLIENT)/$(NAME_CLIENT) $(BUILD_DIR_CLIENT)

.PHONY: server
server: 
		cd $(PATH_SERVER); \
		gcc -std=c89 server.c main_server.c reserv_server.c ../../include/get.c -o $(NAME_SERVER)

.PHONY: client
client:
		cd $(PATH_CLIENT); \
		gcc -std=c89 client.c ../../include/get.c -o $(NAME_CLIENT)

.PHONY: clean
clean:
		-rm -Rf $(BUILD_DIR_SERVER)
		-rm -Rf $(BUILD_DIR_CLIENT)
		cd $(PATH_SERVER); \
		rm $(NAME_SERVER)
		cd $(PATH_CLIENT); \
		rm $(NAME_CLIENT)


