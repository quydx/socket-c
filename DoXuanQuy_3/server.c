/**
Hoten : Do Xuan Quy
MSV: 14020634
Description : Server bai 1
*/
#include "sys/socket.h"
#include "stdio.h"
#include "unistd.h"
#include "netinet/in.h"
#include "sys/types.h"
#include "stdlib.h"
#include "string.h"
#include "arpa/inet.h"
#include "inttypes.h"

/** list of struct for interview /*
// struct sockaddr_in {
//     short            sin_family;   // e.g. AF_INET
//     unsigned short   sin_port;     // e.g. htons(3490)
//     struct in_addr   sin_addr;     // see struct in_addr, below
//     char             sin_zero[8];  // zero this if you want to
// };

// struct in_addr {
//     unsigned long s_addr;  // load with inet_aton()
// };

// struct sockaddr {
//  	unsigned short sa_family;  protocol family 
//  	char sa_msg_send[14]; 
// }; 
/** End */

int main(int argc, char const *argv[])
{
	int socket_desc = 0 , connfd = 0;  // socket_desc : socket describle, socket return from accept
	socklen_t clilen; // length of client socket 
	struct sockaddr_in cliaddr, servaddr; // client and server address

	// create socket, exit if error
	if ( (socket_desc = socket(AF_INET, SOCK_STREAM, 0) ) < 0){
		perror("Error in create socket");
		exit(1);
	}
	else {
		puts("Socket created");
	}
	// set all server address bit to 0
	memset(&servaddr, '0', sizeof(servaddr));

	// set value to serv address fields
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8888);
	// bind agument to serv address
	if (bind(socket_desc, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		perror("Error in bind()");
		exit(2);
	}
	else {
		puts("Bind successful");
	}
	// listen to client connect
	listen(socket_desc, 10); 
	while(1){
		puts("Waiting for incoming connection ... ");
		clilen = sizeof(cliaddr);
		connfd = accept(socket_desc, (struct sockaddr *)&cliaddr, &clilen);
		if (connfd == -1){
			perror("Error connection to client");
			exit(3);
		}
		else{
			printf("A client is connect to server:\n");
		}
		// print client information
		printf("Client port is :%d\n", ntohs(cliaddr .sin_port));
		char *z = inet_ntoa(*(struct in_addr *)&cliaddr.sin_addr.s_addr);
		printf("Client IP Adress is: %s\n", z);

		char msg_send[] = "Hello client, I am server!"; // msg send to client 	

		// send msg to client 
		write(connfd, msg_send, sizeof(msg_send));
		//read client msg
		char *msg_recv = (char *)malloc(1000*sizeof(char));
		int n = read(connfd, msg_recv, 1000);
		*(msg_recv + n) = '\0';
		puts(msg_recv);
		free(msg_recv);
		//close connection
		
	}
	printf("\n");
	return 0;
}

