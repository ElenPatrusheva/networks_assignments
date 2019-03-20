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

#define CLIENTS_NUMBER 20
#define QUEUE_SIZE 300
#define BUFFER_SIZE 1024
#define FILE_NAME_SIZE 30
#define NUMBER_OF_FILES 20
#define IP_LENGHT 15

struct node{
	int is_active;
    char ip[IP_LENGHT + 1];
    unsigned long port;
	char files[NUMBER_OF_FILES][FILE_NAME_SIZE];
};

struct client_conn_data {
    int connection_socket_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
};

struct ping_response{
	char files[NUMBER_OF_FILES][FILE_NAME_SIZE];
};

struct host {
    char ip[IP_LENGHT + 1];
    unsigned long port;
};

int known_nodes_number;

pthread_t server_threads[CLIENTS_NUMBER];

struct node network_nodes[CLIENTS_NUMBER];

pthread_mutex_t threads_lock;

pthread_t threads[CLIENTS_NUMBER];
int thread_is_free[CLIENTS_NUMBER];

void client (){

	while (1){
		printf("Enter your command: ");
		char command [BUFFER_SIZE];

		scanf("%s", command);
		if (strcmp(command, "file_transfer") == 0) {
			int exists = 0;
			for (int i = 0; i < known_nodes_number; i++){
				int socket_fd;

			    char buffer[BUFFER_SIZE];

			    struct sockaddr_in server_addr;

			    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
			    
			    memset(&server_addr, 0, sizeof(struct sockaddr_in));
			    server_addr.sin_family = AF_INET;
			    server_addr.sin_addr.s_addr = inet_addr(network_nodes[i].ip);
			    server_addr.sin_port = htons(network_nodes[i].port);

			    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0){
			    	printf("Connection_failed");
			    }
			    send(socket_fd, command, sizeof(command), 0);

			    recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
			    if (strcmp(buffer, "ok") == 0){
			    	printf("Enter file name: ");
			    	char file_name[FILE_NAME_SIZE];
			    	scanf("%s", file_name);
			    	send(socket_fd, file_name, sizeof(file_name), 0);
			    	int words;
			    	recv(socket_fd, &words, sizeof(words), 0);
			    	if (words >= 0){
			    		exists = 1;
			    		char word[20];
			    		for (int i = 0; i < words; i++){
			    			recv(socket_fd, word, sizeof(word), 0);
			    			printf("%s", word);
			    		}
			    	printf("done\n");
			    	break;
			    	}
			    }
			}
			if (exists == 0){
				printf("no such_file\n");
			}
		    
		}
		else if (strcmp(command, "exit") == 0){
			return;
		}
	}
}

void client_test (){

	while (1){
		printf("Enter your command: ");
		char command [BUFFER_SIZE];

		scanf("%s", command);
		if (strcmp(command, "ping") == 0 ){
			int socket_fd;

			char buffer[BUFFER_SIZE];

		    struct sockaddr_in server_addr;

		    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
			    
		    memset(&server_addr, 0, sizeof(struct sockaddr_in));
		    server_addr.sin_family = AF_INET;
		    server_addr.sin_addr.s_addr = inet_addr("192.168.2.2");
		    server_addr.sin_port = htons(7568);
		    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0){
		    	printf("Connection_failed");
		    }
		    send(socket_fd, command, sizeof(command), 0);
		    recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
		}
		/*
		if (strcmp(command, "file_transfer") == 0) {
			int exists = 0;
			for (int i = 0; i < known_nodes_number; i++){
				int socket_fd;

			    char buffer[BUFFER_SIZE];

			    struct sockaddr_in server_addr;

			    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
			    
			    memset(&server_addr, 0, sizeof(struct sockaddr_in));
			    server_addr.sin_family = AF_INET;
			    server_addr.sin_addr.s_addr = inet_addr(network_nodes[i].ip);
			    server_addr.sin_port = htons(network_nodes[i].port);

			    if(connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0){
			    	printf("Connection_failed");
			    }
			    send(socket_fd, command, sizeof(command), 0);

			    recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
			    if (strcmp(buffer, "ok") == 0){
			    	printf("Enter file name: ");
			    	char file_name[FILE_NAME_SIZE];
			    	scanf("%s", file_name);
			    	send(socket_fd, file_name, sizeof(file_name), 0);
			    	int words;
			    	recv(socket_fd, &words, sizeof(words), 0);
			    	if (words >= 0){
			    		exists = 1;
			    		char word[20];
			    		for (int i = 0; i < words; i++){
			    			recv(socket_fd, word, sizeof(word), 0);
			    			printf("%s", word);
			    		}
			    	printf("done\n");
			    	break;
			    	}
			    }
			}
			if (exists == 0){
				printf("no such_file\n");
			}
		    
		}
		else if (strcmp(command, "exit") == 0){
			return;
		}*/
	}
}


void ping(){
	/*

	*/
}

void *pinger(){
	while(1){
		for (int i = 0; i < known_nodes_number; i++){
			ping(network_nodes[i]);
		}
	}

}

int have_file(char * file_name){
    FILE *fptr = fopen(file_name, "r");
    if (fptr == NULL)
        return 0;
    fclose(fptr);
    return 1;
}

int numb_of_words(char * file_name) 
{ 
    FILE *fptr=fopen(file_name,"r"); 
    int words = 0;
    char ch = fgetc(fptr); 
    while(ch!=EOF) 
    { 
        printf("%c",ch); 
        if(ch==' '||ch=='\n')
            { 
                words++; 
            }
        ch=fgetc(fptr); 
    }
    fclose(fptr); 
    return words;
}

void *process_client_connection(void *request_data){
	char buffer[BUFFER_SIZE];
	struct client_conn_data *client_data = (struct client_conn_data *) request_data;
	if (recv(client_data->connection_socket_fd, buffer, BUFFER_SIZE - 1, 0) == -1){
		printf("Recv from client failed");
	}
	if (strcmp(buffer, "ping") == 0){
		//write his data
		send(client_data->connection_socket_fd, "ok", 3, 0);
		return 0;	
	}
	else if (strcmp(buffer, "file_transfer") == 0){//file transfer initialization

		send(client_data->connection_socket_fd, "ok", 3, 0);
		if (recv(client_data->connection_socket_fd, buffer, BUFFER_SIZE - 1, 0) == -1){//название файла
			printf("Recv from client failed");
		}

		if (have_file(buffer)){


			int words = numb_of_words(buffer);
			send(client_data->connection_socket_fd, &words, sizeof(words), 0);

			FILE *fptr = fopen(buffer, "r");
			char word[100];
    		while (fscanf(fptr, " %1023s", word) == 1) {
        		send(client_data->connection_socket_fd, word, sizeof(word), 0);
    		}
			return 0;


		}
		else {
			int minus_odin = -1;
			send(client_data->connection_socket_fd, &minus_odin, sizeof(int), 0);
			return 0;
		}
	} 
}
void *process_client_connection_test(void *request_data){
	char buffer[BUFFER_SIZE];
	struct client_conn_data *client_data = (struct client_conn_data *) request_data;
	if (recv(client_data->connection_socket_fd, buffer, BUFFER_SIZE - 1, 0) == -1){
		printf("Recv from client failed");
	}
	if (strcmp(buffer, "ping") == 0){
		//write his data
		printf("kek");
		send(client_data->connection_socket_fd, "ok", 3, 0);
		return 0;	
	}
	/*
	else if (strcmp(buffer, "file_transfer") == 0){//file transfer initialization

		send(client_data->connection_socket_fd, "ok", 3, 0);
		if (recv(client_data->connection_socket_fd, buffer, BUFFER_SIZE - 1, 0) == -1){//название файла
			printf("Recv from client failed");
		}

		if (have_file(buffer)){


			int words = numb_of_words(buffer);
			send(client_data->connection_socket_fd, &words, sizeof(words), 0);

			FILE *fptr = fopen(buffer, "r");
			char word[100];
    		while (fscanf(fptr, " %1023s", word) == 1) {
        		send(client_data->connection_socket_fd, word, sizeof(word), 0);
    		}
			return 0;


		}
		else {
			int minus_odin = -1;
			send(client_data->connection_socket_fd, &minus_odin, sizeof(int), 0);
			return 0;
		}
	}*/
}



void *listener (void *_thread_data){

    socklen_t client_addr_len;
    struct sockaddr_in server_addr, client_addr;
    struct client_conn_data *request_data;
    struct host *_host;
    _host = (struct host *)_thread_data;
    int connection_socket_fd;

    for (int i = 0; i < CLIENTS_NUMBER; i++){
    	thread_is_free[i] = 1;
	}

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        printf("Socket creation failed\n");
        return NULL;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(_host->ip);
    server_addr.sin_port = htons(_host->port);

    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0){
        printf("Bind failed\n");
        return NULL;
    }

    if (listen(server_sockfd, QUEUE_SIZE) != 0) {
        printf("Listen failed\n");
        return NULL;
    }

    //*

    while (1) {
        connection_socket_fd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (connection_socket_fd == -1) {
            printf("Acception failed\n");
            continue;
        }

        request_data = malloc(sizeof(struct client_conn_data));
        request_data->connection_socket_fd = connection_socket_fd;
        request_data->client_addr = client_addr;
        request_data->client_addr_len = client_addr_len;
        int thread_number = 0;//first available thread
        while (thread_is_free[thread_number] != 1) {
        	thread_number ++;
        }

        pthread_mutex_lock(&threads_lock);
        thread_is_free[thread_number] = 0;
        pthread_create(&(threads[thread_number]), NULL, process_client_connection_test, (void *)request_data);
        pthread_mutex_unlock(&threads_lock);
    }

    //cleanup_server();
    //free(host);
    return 0;
}

int main(int argc, char **argv){

	char command [BUFFER_SIZE];
	if (argc != 3){
		printf("Incorrect number of arguments, you should enter ip and port number.\n");
		return(1);
	}

	struct host *_host;
	_host = malloc(sizeof(struct host));
    strcpy(_host->ip, argv[1]);
    _host->ip[IP_LENGHT] = '\0';
    _host->port = strtoul(argv[2], (char **)NULL, 10);

//    pthread_t ping_thread;
    pthread_t listen_thread;
//	pthread_create(&ping_thread, NULL, pinger, (void *)_host);
	pthread_create(&listen_thread, NULL, listener, (void *)_host);

	client_test();

//	pthread_cancel(ping_thread);
	pthread_cancel(listen_thread);
	return 0;
}