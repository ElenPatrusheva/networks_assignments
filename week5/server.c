#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <time.h>

#define PORT     61299  
#define MAXLINE 1024 
struct Student {
    char name[50];
    int group;
    int age;

} student; 

struct for_thread{
    struct Student st;
    struct sockaddr_in cliaddr;
    int sockfd;
    int len;
};


void fun(void * i){
    struct for_thread * data = (struct for_thread *) i;
    int tid = pthread_self();
    char buffer[MAXLINE]; 
    sprintf(buffer, "Student name is: %s, student age is: %d, student group is: %d.", data->st.name, data->st.age, data->st.group);

    printf("Thread id is: %d, time is: %li.\n %s\n", tid, time(0), buffer);
    sleep(10);
    sendto(data->sockfd, (char *)buffer, MAXLINE,  
        0, (const struct sockaddr *) &data->cliaddr, 
            data->len);
}
// Driver code 
int main() { 
    int sockfd; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    int len, n; 
    pthread_t thread;
    while(1){
        recvfrom(sockfd, (char *) &student, sizeof(student),  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len); 
        printf("%s\n", student.name);
        printf("%d\n", student.age);
        printf("%d\n", student.group);

        struct for_thread * client_data = malloc(sizeof(struct for_thread));
        client_data->st = student;
        client_data->sockfd = sockfd;
        client_data->cliaddr = cliaddr;
        client_data->len = len;
        pthread_create(&thread, NULL, fun, (void*) client_data);
    }
    return 0; 
} 