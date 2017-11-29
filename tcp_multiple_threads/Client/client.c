/**
Fullname: Do Xuan Quy
MSV: 14020634
Description : Client
Use : ./client <IP_SERVER>
*/
#include <stdio.h>
#include "stdlib.h"
#include "sys/socket.h"
#include "string.h"
#include "sys/types.h"
#include "netinet/in.h" // cac struct socket
#include "unistd.h"
#include "arpa/inet.h"

int main(int argc, char const *argv[])
{
	int socket_desc = 0;
	struct sockaddr_in  servaddr;

	if (argc != 2){
		printf("No IP server provide\n");
		printf("%s\n", "Use: ./client <IP server>" );
		return 1;
	}
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, '0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		printf("%s\n", "inet_pton error occured");
		return 1;
	}
	if (connect(socket_desc, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
		printf("%s\n", "Error in connect");
		return 1;
	}
	else printf("Connected to server %s:%d\n", inet_ntoa(*(struct in_addr*)&servaddr.sin_addr) , 8888);

	char username[256];
	printf("%s\n", "Nhap username:");
	fgets(username, sizeof(username), stdin);
	write(socket_desc, username , sizeof(username));
	char list_users[1000];
	read(socket_desc, list_users, sizeof(list_users));
	printf("List current users is : %s\n", list_users );
	char send_message[256];
	char recv_message[256];
	while(1)
	{	
		printf("%s", "Enter message: ");
		fgets(send_message, sizeof(send_message), stdin);
		send_message[strlen(send_message) - 1] = '\0';
		write(socket_desc, send_message , sizeof(send_message));
		if (strcmp(send_message, "@") == 0) {
			printf("%s\n", "End connection");
			break;
		} 
		int n = read(socket_desc, recv_message, sizeof(recv_message));
		if (n < 256 ) recv_message[n] = '\0';
		printf("Thong bao: %s\n", recv_message);
	}
	close(socket_desc);
	return 0;
}

