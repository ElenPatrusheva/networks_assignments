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
	int files[100];//has files with file descriptors
};

struct ping_response{
	int is_active;
	int files[100];
}
pthread_t server_threads[CLIENTS_NUMBER];

struct node network_nodes[100];


void client (){

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
}