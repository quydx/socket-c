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

	while(1)
	{	
		char filename[256];
		char data[1024];
		// get file name 
		printf("%s", "FILENAME: ");
		fgets(filename, sizeof(filename), stdin);

		int length = strlen(filename);
		filename[length-1] = '\0';
		if (filename[0] == '\0') {
			printf("%s\n", "Nhap ten file zo -_-'");
			continue;
		}
		write(socket_desc, filename , length-1);
		if ( (int)strlen(filename) == 1 && filename[0] == '@') {
			printf("%s\n", "End download file");
			break;
		}
		FILE * wf = fopen(filename, "ab+");
		int n;
		while (1){
			n = read(socket_desc, data, sizeof(data));
			if ( n == 1 && data[0] == '@') {
				printf("%s\n", "File not exist");
				fseek (wf , 0 , SEEK_END);
				int lSize = ftell (wf);
				rewind (wf);
				if (lSize == 0){
					remove(filename);
				}
				break;
			}
			if (n == 1 && data[0] == '\0') {
				printf("%s\n", "Done" );
				break;
			}
			fwrite(data, 1 ,n, wf);
			if( n < 1024) {
				printf("%s\n", "Done");
				break;
			}
		}
		fclose(wf);
	}
	close(socket_desc);
	return 0;
}

