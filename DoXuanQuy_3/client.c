/**
Hoten : Do Xuan Quy
MSV: 14020634
Description : Client bai 1
*/
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "string.h"
#include "sys/types.h"
#include "netinet/in.h" // cac struct socket
#include "unistd.h"
#include "arpa/inet.h"

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
//  	char sa_msg_recv[14]; 
// }; 
/** End */

int main(int argc, char const *argv[])
{
	int socket_desc = 0, n = 0;
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
	if (connect(socket_desc, (struct sockaddr *)&servaddr, sizeof(servaddr))){
		printf("%s\n", "Error in connect");
		return 1;
	}
	char *msg_recv = (char *)malloc(1000 * sizeof(char));
	n = read(socket_desc, msg_recv, 1000);
	*(msg_recv + n) = '\0';
	fputs(msg_recv, stdout);
	free(msg_recv);
	char msg_send[] = "Hi server, I'am client";
	write(socket_desc, msg_send, sizeof(msg_send));
	sleep(5);
	close(socket_desc);

	printf("\n");
	return 0;
}

