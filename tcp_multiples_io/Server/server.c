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
#include "pthread.h"
// #include "sockhelp.h"
#include <sys/file.h>
// #include <fcntl.h>

int nsent = 0, nsending = 0;
int connection_handler();

int main(int argc, char const *argv[])
{
	int msock = 0 ; // socket_desc : socket describle, socket return from accept
	socklen_t alen; // length of client socket 
	struct sockaddr_in cliaddr, servaddr; // client and server address

	fd_set rfds; // read file des set 
	fd_set afds; // active file des set 
	// int alen; // len of client addr
	int fd;
	// tao socket 
	if ( (msock = socket(AF_INET, SOCK_STREAM, 0) ) < 0){
		perror("Error in create socket");
		exit(1);
	}
	else {
		puts("Socket  was created");
	}
	//set master socket to allow multiple connections , this is just a good habit, it will work without this
	int opt = 1;
    	if( setsockopt(msock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    	{
       	perror("setsockopt");
        	exit(EXIT_FAILURE);
    	}
	// set 0 cho danh sach active 
	FD_ZERO (&afds);
	//set danh sach active vao msock
	FD_SET(msock, &afds );
	// set cac tham so  cua server addr 
	memset(&servaddr, '0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8888);
	// bind agument to serv address
	if (bind(msock, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		perror("Error in bind()");
		exit(2);
	}
	else {
		puts("Bind successful");
	}
	// listen to client connect
	listen(msock, 100);
	while(1){
		//copy mem tu afds vao rfds
		memcpy(&rfds , &afds, sizeof(rfds));
		//
		if ( select (FD_SETSIZE, &rfds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0) < 0)
		{
			perror("Error in select");
			exit(1);
		}
	 	if (FD_ISSET(msock, &rfds)){ // neu bit msock da duoc bat trong rfds
	 		int ssock;
	 		alen = sizeof(cliaddr);
	 		ssock = accept(msock, (struct sockaddr *)&cliaddr, &alen); // socket moi tu cac client 
	 		if (ssock < 0) {
	 			perror("Error in ssock line 74");
	 		}
	 		//set ssock vao danh sach afds
	 		FD_SET(ssock, &afds);
	 	}
	 	for (fd = 0; fd < FD_SETSIZE ; ++fd ){
	 		if (fd != msock  &&  FD_ISSET(fd , &rfds)){
	 			int res = connection_handler(fd);
	 			if (res == -1) 
	 				FD_CLR (fd, &afds); 
	 		}
	 	}
	}
	return 0;
}
int connection_handler(int sock)
{
	char filename[256];
	int n = read(sock, filename, sizeof(filename));
	filename[n] = '\0';
	if ( n == 1 && filename[0] == '@') {
		printf("%s\n", "Client end download file");
		close(sock);
		return -1;
	}
	if (n < 0){
		perror("Error in read()");
		close(sock);
		return -1;
	}
	printf("A client require file '%s'\n", filename);
	FILE *rf = fopen(filename, "rb");
	if (rf == NULL ) {
		write(sock, "@" , 1);
		printf("%s\n", "File not exist");
		return 1;
	}
	else {
		nsending++;
		printf("Total file sending is %d\n", nsending);
		char data[1024];
		while(1) {
			int j = fread(data, 1, 1024 , rf);
			if ( j == 0) {
				data[0] = '\0';
				j = 1;
			}
			int nw = write(sock, data, j);
			if (nw < 1024) break;
		}
		fclose(rf);
		printf("%s\n", "Done");
		nsent++;
		nsending--;
		printf("Total file sending is %d\n", nsending);
		printf("Sent successful %d file\n", nsent);
	}
	return 1;
}