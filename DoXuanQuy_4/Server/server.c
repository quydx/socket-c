/**
Fullname : Do Xuan Quy
MSV: 14020634
Desciption : Server 
Use : ./server
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
#include "ctype.h"
#include "sys/sendfile.h"

int main(int argc, char const *argv[])
{
	int socket_desc = 0 , connfd = 0;  // socket_desc : socket describle, socket return from accept
	socklen_t clilen; // length of client socket 
	struct sockaddr_in cliaddr, servaddr; // client and server address
	
	if ( (socket_desc = socket(AF_INET, SOCK_STREAM, 0) ) < 0){
		perror("Error in create socket");
		exit(1);
	}
	else puts("Socket created");

	memset(&servaddr, '0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8888);

	// bind agument to serv address
	if (bind(socket_desc, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		perror("Error in bind()");
		exit(2);
	}
	else puts("Bind successful");
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
		else printf("A client is connect to server:\n");
		printf("Client port is :%d\n", ntohs(cliaddr .sin_port));
		char *z = inet_ntoa(*(struct in_addr *)&cliaddr.sin_addr.s_addr);
		printf("Client IP Adress is: %s\n", z);


		char filename[256];
		int n = read(connfd, filename, sizeof(filename));
		printf("Read successful %d byte filename\n", n  );
		filename[n] = '\0';
		if (n < 0){
			perror("Error in read()");
			break;
		}
		printf("Client require file '%s'\n", filename);

		FILE *rf = fopen(filename, "rb");
		if (rf == NULL ) {
			perror("error in open file");
			exit(1);
		}
		fseek (rf , 0 , SEEK_END);
		int lSize = ftell (rf);
		printf("File size is %d\n", lSize );
		rewind (rf);
		/***** su dung sendfile.h *****/
		// while ((n = sendfile(connfd, fileno(rf), 0, 1024)) > 0){}	
		
    		/** *** khong su dung sendfile.h**/
		char data[1024];
		while(1) {
			data[0] = '\0';
			int j = fread(data, 1, 1024 , rf);
			if (j< 0) break;
			write(connfd, data, j); 
			if (j < 1024 && feof(rf) ) break;
		}
	    	fclose(rf);
		close(connfd);
	}
	return 0;
}

