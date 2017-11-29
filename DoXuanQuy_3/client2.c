/**
Fullname: Do Xuan Quy
MSV: 14020634
Description : Client bai 2
*/
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "string.h"
#include "sys/types.h"
#include "netinet/in.h" // cac struct socket
#include "unistd.h"
#include "arpa/inet.h"

#define BUFFER_SIZE 1024*sizeof(char)

/** list of struct for review /*
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
//  	char sa_data[14]; 
// }; 
/** End */

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

	char *msg_recv = (char *)malloc(BUFFER_SIZE);
	char *msg_send = (char *)malloc(BUFFER_SIZE);

	while(1){

		memset(msg_send, '0', BUFFER_SIZE);
		memset(msg_recv, '0', BUFFER_SIZE);
		printf("%s", "Message to server: ");
		fgets(msg_send, BUFFER_SIZE, stdin);

		write(socket_desc, msg_send, BUFFER_SIZE);
		
		if (*(msg_send + 0) == '\n'){
			printf("%s\n", "Close connection");
			close(socket_desc);
			break;
		}

		int n = read(socket_desc, msg_recv, BUFFER_SIZE );
		if (n < 0) {
			perror("Error in read()");
			break;
		}
		*(msg_recv + n) = '\0';
		printf("Server Reply: %s", msg_recv);

	}
	free(msg_recv);
	free(msg_send);
	return 0;
}

