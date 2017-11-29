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
#include "sys/file.h"

#define MAX_CLI 1000

typedef struct {
	char name[256];
	int sockfd;
	int useFlag;
} User;
User users[MAX_CLI];
int i;
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
		// chuyen thread thanh doc lap
		pthread_detach(pthread_self()); 

		int sock = *(int *) connfd;
		// char message[1024];
		char username[256];
		read(sock, username, sizeof(username));
		username[strlen(username) - 1] = '\0';
		pthread_mutex_lock(&counter_mutex);
		printf("Client username : %s\n", username  );
		int t ;
		int cur_index = -1;
		for (t = 0; t < i; t++)
		{
			if ( users[t].useFlag == 0){
				cur_index = t;
			}
		}
		if (cur_index == -1 ) {
			cur_index = i;
			i++;
		};
		printf("Current index is  %d\n", cur_index );
		strcpy(users[cur_index].name, username);
		users[cur_index].name[strlen(username)]='\0';
		users[cur_index].useFlag = 1;
		users[cur_index].sockfd = sock;
		printf("user[%d]: %s\n", cur_index ,users[cur_index].name );
		pthread_mutex_unlock(&counter_mutex);
		printf("%s\n", "List current user: ");
		int j = 0;
		char list_users[1000];
		strcpy(list_users,  "");
		pthread_mutex_lock(&counter_mutex);
		for ( j = 0; j < i; ++j)
		{
			if (users[j].useFlag == 1){
				printf("%d : %s\n",j, users[j].name);
				strcat(list_users , users[j].name);
				strcat(list_users, " ");
			}
		}
		pthread_mutex_unlock(&counter_mutex);
		printf("List current users is : %s\n", list_users );
		write(sock, list_users , sizeof(list_users));
		char message[256];
    		while(1)
		{	
			printf("%s\n", "waiting for message ..." );
			int n = read(sock, message, sizeof(message));
			if (strcmp(message, "@") == 0) {
				printf("User %s end connect\n", users[cur_index].name);
				users[cur_index].useFlag = 0;
				close(sock);
				break;
			}
			if (n < 256 ) message[n] = '\0';
			printf("User %s send message %s \n", users[cur_index].name, message );
			int k = 0 ;
			pthread_mutex_lock(&counter_mutex);
			char send[256];
			for (k = 0; k <= i - 1 ; k++)
			{
				send[0] = '\0';
				strcat(send, users[cur_index].name);
				strcat(send, ":");
				strcat(send, message);
				strcat(send, " ");
				if (users[k].useFlag == 1){
					if (k != cur_index){
						write(users[k].sockfd , send, strlen(send));
					}
				}
			}
			pthread_mutex_unlock(&counter_mutex);
		}
     return 0;
}