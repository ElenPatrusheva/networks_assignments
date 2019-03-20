#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#define QUEUE_SIZE 30
#define BUFFER_SIZE 1024
#define MAX 80 
#define SERV_PORT 8080 
#define SA struct sockaddr 
#define IP_LENGHT 15
#define CLIENTS_NUMBER 30


struct client_conn_data {
    int connection_socket_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
};

struct host {
    char ip[IP_LENGHT + 1];
    unsigned long port;
};

//for threads
pthread_t client_connection_threads[CLIENTS_NUMBER];
int available_threads[CLIENTS_NUMBER];
pthread_mutex_t threads_lock;

void *func_serv(void * _sockfd) 
{
	int sockfd = (int)_sockfd;
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(sockfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
  
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 
/*
it works!!
*/
void * listener(void *current_host_data){
	int sockfd, connfd, len; 
    struct sockaddr_in servaddr, client; 
    struct host * current_host;
    current_host = (struct host *)current_host_data;
    for (int i = 0; i < CLIENTS_NUMBER; i++){
    	available_threads[i] = 1;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else{
        printf("Socket successfully created..\n"); 
    }

    memset(&servaddr, 0, sizeof(struct sockaddr_in));

    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(current_host->ip);
    servaddr.sin_port = htons(current_host->port);
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else{
        printf("Socket successfully binded..\n"); 
    }
  
    // Now server is ready to listen and verificate
    if ((listen(sockfd, QUEUE_SIZE)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else{
        printf("Server listening..\n"); 
    }
    printf("ready to accept");
    while(1){
	    len = sizeof(client); 
	  
	    // Accept the data packet from client and verification 
	    connfd = accept(sockfd, (SA*)&client, &len); 
	    if (connfd < 0) { 
	        printf("server acccept failed...\n"); 
	        exit(0); 
	    } 
	    else{
	        printf("server acccept the client...\n");
	    }
	    struct client_conn_data *client_data;
	    client_data = malloc(sizeof(struct client_conn_data));
	    client_data->connection_socket_fd = connfd;
	    client_data->client_addr = client;
	    client_data->client_addr_len = len;
	    int available_thread = 0;
	    while(available_threads[available_thread] != 1){
	    	available_thread++;
	    }
        pthread_mutex_lock(&threads_lock);
        available_threads[available_thread] = 0;
	    pthread_create(&available_threads[available_thread], NULL, func_serv, (void *)connfd);
        pthread_mutex_unlock(&threads_lock);
	}
}



void * process_client_connection(){

}

void func_client(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
        write(sockfd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        printf("From Server : %s", buff); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
    } 
} 

void client(){
	int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(SERV_PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    func_client(sockfd); 
  
    // close the socket 
    close(sockfd); 
}

int main(int argc, char **argv){
	if (argc != 3){
		printf("Incorrect number of arguments, you should enter ip and port number.\n");
		return(1);
	}
	struct host *current_host;
	current_host = malloc(sizeof(struct host));
	strcpy(current_host->ip, argv[1]);
	current_host->ip[IP_LENGHT] = '\0';
	current_host->port = strtoul(argv[2], (char **)NULL, 10);

	pthread_t listen_thread;
	pthread_create(&listen_thread, NULL, listener, (void *)current_host);
	printf("Do you want to work like a client?\n");
	char ansver[4];
	scanf("%s", ansver);
	if (strcmp(ansver, "yes") == 0){
		client();
	}
	while(1){

	}
	pthread_cancel(listen_thread);
	return 0;
}