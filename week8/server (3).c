//
// Base code for tcp connection was taken from this repo:
// https://github.com/Regzon/c-tcp-connection
//
//
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

#define MAX_CLIENTS 20  // Max amount of clients that can communicate with the server at the same time
#define MAX_QUEUE 100   // Max amount of requests that can be pending for the listen call
#define BUF_SIZE 1024

int in_progress;

int server_sockfd;

pthread_t threads[MAX_CLIENTS];
int threads_availability[MAX_CLIENTS];
pthread_mutex_t threads_lock;

struct client_conn_data {
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
};

struct host_data {
    char host[BUF_SIZE];
    unsigned long port_num;
};


int get_thread_index() {
    pthread_mutex_lock(&threads_lock);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (threads_availability[i] == 1) {
            pthread_mutex_unlock(&threads_lock);
            return i;
        }
    }

    pthread_mutex_unlock(&threads_lock);

    return -1;
}


int release_thread_index(pthread_t thread) {
    pthread_mutex_lock(&threads_lock);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        // If thread is not available and thread ids are matching
        if (threads_availability[i] == 0 && threads[i] == thread) {
            threads_availability[i] = 1;
            threads[i] = 0;
            pthread_mutex_unlock(&threads_lock);
            return 0;
        }
    }

    pthread_mutex_unlock(&threads_lock);

    return -1;
}


void *request_processing(void *thread_data) {
    struct client_conn_data *data = (struct client_conn_data *)thread_data;

    int return_code;
    ssize_t data_len;
    char buf[BUF_SIZE];

    // Get (BUF_SIZE - 1) number of bytes.
    // Last byte of the buffer is used for the stop character.
    data_len = recv(data->connfd, buf, BUF_SIZE - 1, 0);

    if (data_len == -1) {
        fprintf(stderr, "Receive from the client failed\n");
    }
    else {
        buf[data_len] = '\0';
        printf("Received %ld bytes from the client\nData: \"%s\"\n", data_len, buf);
    }

    sleep(4);

    return_code = release_thread_index(pthread_self());
    if (return_code == -1) {
        fprintf(stderr, "Thread release failed. Ignoring...\n");
    }

    close(data->connfd);
    free(thread_data);

    return NULL;
}


void cleanup_server() {
    pthread_t thread;

    close(server_sockfd);

    printf("Finishing all remaining connections...\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_mutex_lock(&threads_lock);
        if (threads_availability[i] == 0) {
            thread = threads[i];
            pthread_mutex_unlock(&threads_lock);
            pthread_join(thread, NULL);
        }
        else {
            pthread_mutex_unlock(&threads_lock);
        }
    }
}


void *server(void *_thread_data) {
    int connfd;
    int return_code;   // Variable for intermediate results
    int thread_index;  // Variable for finding free thread index

    socklen_t client_addr_len;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    struct client_conn_data *thread_data;

    struct host_data *host;

    host = (struct host_data *)_thread_data;

    // Set all threads available by default
    for (int i = 0; i < MAX_CLIENTS; i++) threads_availability[i] = 1;

    printf("Setting up server...\n");

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        fprintf(stderr, "Server socket creation failed\n");
        return NULL;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host->host);
    server_addr.sin_port = htons(host->port_num);

    return_code = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (return_code != 0) {
        fprintf(stderr, "Socket bind failed\n");
        return NULL;
    }

    return_code = listen(server_sockfd, MAX_QUEUE);
    if (return_code != 0) {
        fprintf(stderr, "Listen failed\n");
        return NULL;
    }

    printf("Waiting for incoming connections...\n");

    while (1) {
        connfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (connfd == -1) {
            fprintf(stderr, "Accept in server failed\n");
            continue;
        }

        printf("Accepted new connection from the client\nCreating thread...\n");

        thread_data = malloc(sizeof(struct client_conn_data));
        thread_data->connfd = connfd;
        thread_data->client_addr = client_addr;
        thread_data->client_addr_len = client_addr_len;

        while ((thread_index = get_thread_index()) == -1) {
            printf("All threads are busy, waiting...");
        }

        pthread_mutex_lock(&threads_lock);
        threads_availability[thread_index] = 0;
        pthread_create(&(threads[thread_index]), NULL, request_processing, (void *)thread_data);
        pthread_mutex_unlock(&threads_lock);
    }

    cleanup_server();
    free(host);

    return 0;
}

void terminate_server(pthread_t server_thread) {
    cleanup_server();
    pthread_cancel(server_thread);
}

void client() {
    int sockfd;
    int return_code;
    ssize_t data_len;

    const char message[] = "Hello, Server! It's client";

    struct sockaddr_in server_addr;

    struct host_data remote_host;

    printf("Enter remote host ip:\n");
    scanf("%s", remote_host.host);

    printf("Enter remote host port:\n");
    scanf("%lu", &remote_host.port_num);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "Socket creation failed\n");
        return;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(remote_host.host);
    server_addr.sin_port = htons(remote_host.port_num);

    return_code = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (return_code != 0) {
        fprintf(stderr, "Connection to server failed\n");
        return;
    }

    data_len = send(sockfd, message, sizeof(message), 0);
    if (data_len == -1) {
        fprintf(stderr, "Send to server failed\n");
        return;
    }

    close(sockfd);
}


int main(int argc, char **argv) {
    char command[BUF_SIZE];
    struct host_data *_host_data;

    pthread_t thread;

    if (argc != 3) {
        fprintf(stderr, "Invalid command format\nUsage: %s [host] [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    _host_data = malloc(sizeof(struct host_data));
    strcpy(_host_data->host, argv[1]);
    _host_data->port_num = strtoul(argv[2], (char **)NULL, 10);

    pthread_create(&thread, NULL, server, (void *)_host_data);

    while (1) {
        printf("Command:\n");
        scanf("%s", command);

        if (strcmp(command, "connect") == 0) {
            client();
        }
        else if (strcmp(command, "exit") == 0) {
            terminate_server(thread);
            free(_host_data);
            break;
        }
        else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
