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

#define CLIENTS_NUMBER 100
#define QUEUE_SIZE 300
#define BUFFER_SIZE 1024

struct node{
	int is_active;
	int ip;//socket?
	char * file[100];//what format sould i use, I want to have just file names?
};

struct ping_response{
	int is_active;
	int files[100];
}
pthread_t server_threads[CLIENTS_NUMBER];

struct node network_nodes[100];


void client (){
	char file_name[FILE_NAME_SIZE];
	printf("What file do you want to get?");
	scanf(&file_name);
	find_such_node(file_name);
	/*
	for (int i = 0; i < CLIENTS_NUMBER; i++){
		for (int j = 0; i < 100;  i++){
			if (sttcmp(network_nodes[i]->files[j], file_name) == 0){//think more, what if system is not ready

			}

		}
	}*/

}
/*
changes the node info according to the ping success
*/
void ping (struct node node){

}
void *server (void *_thread_data){
	while(1){
		for (int i = 0; i < CLIENTS_NUMBER; i++){
			ping(network_nodes[i]);//how to make it correcte?
		}
	}
}

int main(){
	char command [BUFFER_SIZE];
	return 0;
	while (1){
		printf("Enter your command: ");
		scanf("%s", command);
		if (strcmp(command, "request_file") == 0) {
			client();
		}
		else if (strcmp(command, "exit") == 0){
			terminate_server(thread);
		}
		else {
			printf("You entered incorrect command.\n");
		}
	}
	return 0;
}