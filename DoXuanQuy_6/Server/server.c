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
#include <sys/file.h>

 // #define   LOCK_SH   1    /* shared lock */
 // #define   LOCK_EX   2    /* exclusive lock */
 // #define   LOCK_NB   4    /* don't block when locking */
 // #define   LOCK_UN   8    /* unlock */

int nsent = 0, nsending = 0;
pthread_mutex_t	counter_mutex = PTHREAD_MUTEX_INITIALIZER;
void *connection_handler();

int main(int argc, char const *argv[])
{
	int socket_desc = 0 ; // socket_desc : socket describle, socket return from accept
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
	listen(socket_desc, 100);
	while(1){
		clilen = sizeof(cliaddr);
		int *iptr = malloc(sizeof(int));
		*iptr = accept(socket_desc, (struct sockaddr *)&cliaddr, &clilen);
		if (*iptr == -1){
			perror("Error connection to client");
			exit(3);
		}
		else printf("A new client is connect to server:\n");
		printf("Client port is :%d\n", ntohs(cliaddr .sin_port));
		char *z = inet_ntoa(*(struct in_addr *)&cliaddr.sin_addr.s_addr);
		printf("Client IP Adress is: %s\n", z);

		pthread_t thread_id;
		if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) iptr) < 0)
      	{
            	perror("could not create thread");
            	return 1;
        	}
	}
	return 0;
}
void *connection_handler(void *connfd)
{
		pthread_detach(pthread_self()); 
		int sock = *(int *) connfd;
    		while(1)
		{	
			char filename[256];
			int n = read(sock, filename, sizeof(filename));
			filename[n] = '\0';
			if ( n == 1 && filename[0] == '@') {
				printf("%s\n", "Client end download file");
				break;
			}
			if (n < 0){
				perror("Error in read()");
				break;
			}
			printf("A client require file '%s'\n", filename);
			pthread_mutex_lock(&counter_mutex);
			nsending++;
			printf("Total file sending is %d\n", nsending);
			pthread_mutex_unlock(&counter_mutex);

			FILE *rf = fopen(filename, "rb");
			if (rf == NULL ) {
				perror("error in open file");
				exit(1);
			}
			// khoa file hien tai, tranh truong hop nhieu thread cung doc 1 file
			// flock(fileno(rf), LOCK_SH);
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
			// flock(fileno(rf), LOCK_UN);
			printf("%s\n", "Done");
			pthread_mutex_lock(&counter_mutex);
			nsent++;
			nsending--;
			printf("Total file sending is %d\n", nsending);
			printf("Sent successful %d file\n", nsent);
			pthread_mutex_unlock(&counter_mutex);
		}
		close(sock);
     return 0;
}