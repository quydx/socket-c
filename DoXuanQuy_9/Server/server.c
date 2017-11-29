/**
Fullname : Do Xuan Quy
MSV: 14020634
Desciption : Server 
Use : ./server
*/
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/file.h>
#include <netdb.h>

#define MAX_CLI 1000
// cau truc du lieu cua cac users
typedef struct {
	char name[256];
	int port;
	struct sockaddr_in addr;
	int useFlag;
} User;
// khai bao mang users
User users[MAX_CLI];
// var i : max index of users array
int i;
// 
pthread_mutex_t	counter_mutex = PTHREAD_MUTEX_INITIALIZER;
// func xu ly khi co ket noi moi den
void *connection_handler();
// main
int main(int argc, char const *argv[])
{
	// variable declare 
	int socket_desc = 0 ; // socket_desc : socket describle, socket return from accept
	socklen_t clilen; // length of client socket 
	struct sockaddr_in servaddr; // client and server address
	/*** */
	//create udp socket 
	if ( (socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0){ 
		perror("Error in create socket");
		exit(1);
	}
	else {
		puts("Socket created");
	}
	// config server address
	memset(&servaddr, '0', sizeof(servaddr));
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
	listen(socket_desc, 100);
	// moi khi co client ket noi den , tao thread xu ly moi ket noi

	struct hostent *hostp; /* client host info */
	char *hostaddrp; /* dotted decimal host addr string */
	char username[256];
	int n;
	// char list_users[1000];
	while(1){
		// lay do dai cua dia chi client
		struct sockaddr_in clientaddr;
		clilen = sizeof(clientaddr);
		// nhan data tu bat ki client nao
		n = recvfrom(socket_desc, username, sizeof(username), 0, (struct sockaddr *) &clientaddr, &clilen );
		username[strlen(username) -1] ='\0';
		//lay host cua client vua gui du lieu
		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	    if (hostp == NULL)
	    { 
	      perror("ERROR on gethostbyaddr");
	    }
	    // 
   		hostaddrp = inet_ntoa(clientaddr.sin_addr);
    	if (hostaddrp == NULL){
      		perror("ERROR on inet_ntoa\n");
    	}
    	int curr_port = ntohs(clientaddr .sin_port);
    	printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
    	printf("server received %d/%d bytes: %s\n", (int)strlen(username), n, username);
		printf("Client port is :%d\n", curr_port );
		
		// kiem tra danh sach user xem co user do chua, neu chua chen user vao vi tri trong trong danh sach
		int t ; // bien dem t
		int cur_index = -1; // bien xac dinh index cua user se duoc su dung, chua tim dc de = -1
		int isUserExist = 0; // bien kiem tra user da co trong danh sach hay chua
		// kiem tra xem user ton tai trong danh sach chua
		for (t = 0; t < i; t++)
		{
			if (users[t].port == curr_port) {
				isUserExist = 1;
				break;
			}
		}
		// neu user ton tai, thi xac dinh tin nhan user gui den la message
		if (isUserExist == 1 ) {
			int j ; 
			for ( j = 0; j < i; j ++){
				if (users[j].useFlag == 1 && users[j].port != curr_port) {
					n = sendto(socket_desc, username, sizeof(username), 0, (struct sockaddr *) &users[j].addr, sizeof(users[j].addr));
				}
			}
		}
		//neu khong, tin nhan user gui den la username,tim vi tri va them user vao danh sach user
		else {
			for (t = 0; t < i; ++t){
				if (users[t].useFlag == 0 ){
					cur_index = t;
				}
			}
			if (cur_index == -1 ) {
				cur_index = i;
				i++;
			};
			printf("Current index is  %d\n", cur_index );
			// ghi cac thong tin cua client vao slot
			strcpy(users[cur_index].name, username);
			users[cur_index].name[strlen(username)]='\0';
			users[cur_index].useFlag = 1;
			users[cur_index].port = ntohs(clientaddr .sin_port);
			users[cur_index].addr = clientaddr;
			// in ra vi tri va ten cua user hien tai
			printf("user[%d]: %s port %d\n", cur_index ,users[cur_index].name, users[cur_index].port);
			// in ra danh sach user hien tai
			printf("%s\n", "List current user: ");
			// bien dem j
			int j = 0;
			// string list_user ghi lai thong tin ve list users hien tai
			char list_users[1000];
			strcpy(list_users,  "");
			// ghi thong tin ve cac user vao list_users
			for ( j = 0; j < i; ++j)
			{
				if (users[j].useFlag == 1){
					printf("%d : %s\n",j, users[j].name);
					strcat(list_users , users[j].name);
					strcat(list_users, " ");
				}
			}
			printf("List current users is : %s\n", list_users );
			n = sendto(socket_desc, list_users, sizeof(list_users), 0, (struct sockaddr *) &clientaddr, clilen);
		    if (n < 0) {
		      perror("ERROR in sendto");
		      exit(2);
		    }
		}
	}
	return 0;
}
/*
void *connection_handler(void *connfd)
{
		// chuyen thread thanh doc lap
		pthread_detach(pthread_self()); 
		// lay socket description 
		int sock = *(int *) connfd;
		// char message[1024];
		char username[256];
		// get username from user
		read(sock, username, sizeof(username));
		username[strlen(username) - 1] = '\0';
		// khoa danh sach user de them user vao danh sach user toan cuc
		pthread_mutex_lock(&counter_mutex);
		// in ra username
		printf("Client username : %s\n", username  );
		// kiem tra danh sach user, chen user vao vi tri trong trong danh sach
		int t ; // bien dem t
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
		// in ra slot hien tai cua user
		printf("Current index is  %d\n", cur_index );
		// ghi cac thong tin cua client vao slot
		strcpy(users[cur_index].name, username);
		users[cur_index].name[strlen(username)]='\0';
		users[cur_index].useFlag = 1;
		users[cur_index].sockfd = sock;
		// in ra vi tri va ten cua user hien tai
		printf("user[%d]: %s\n", cur_index ,users[cur_index].name );
		//mo khoa list user
		pthread_mutex_unlock(&counter_mutex);
		// in ra danh sach user hien tai
		printf("%s\n", "List current user: ");
		// bien dem j
		int j = 0;
		// string list_user ghi lai thong tin ve list users hien tai
		char list_users[1000];
		strcpy(list_users,  "");
		// ghi thong tin ve cac user vao list_users
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
*/