#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include "common.h"

#define DEST_PORT 61299
#define SERVER_IP_ADDRESS   "192.168.2.2"

test_struct_t client_data;
//result_struct_t result;

struct Student {
	char name[50];
	int group;
	int age;

} student, result;

void setup_tcp_communication() {
    /*Step 1 : Initialization*/
    /*Socket handle*/
    int sockfd = 0, 
        sent_recv_bytes = 0;

    int addr_len = 0;

    addr_len = sizeof(struct sockaddr);

    /*to store socket addesses : ip address and port*/
    struct sockaddr_in dest;

    /*Step 2: specify server information*/
    /*Ipv4 sockets, Other values are IPv6*/
    dest.sin_family = AF_INET;

    /*Client wants to send data to server process which is running on server machine, and listening on 
     * port on DEST_PORT, server IP address SERVER_IP_ADDRESS.
     * Inform client about which server to send data to : All we need is port number, and server ip address. Pls note that
     * there can be many processes running on the server listening on different no of ports, 
     * our client is interested in sending data to server process which is lisetning on PORT = DEST_PORT*/ 
    dest.sin_port = DEST_PORT;
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP_ADDRESS);
    dest.sin_addr = *((struct in_addr *)host->h_addr);

    /*Step 3 : Create a TCP socket*/
    /*Create a socket finally. socket() is a system call, which asks for three paramemeters*/
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    connect(sockfd, (struct sockaddr *)&dest,sizeof(struct sockaddr));

    /*Step 4 : get the data to be sent to server*/
    /*Our client is now ready to send data to server. sendto() sends data to Server*/

    while(1) {

	    /*Prompt the user to enter data*/
	    /*You will want to change the promt for the second task*//*
	    printf("Enter a : ?\n");
	    scanf("%u", &client_data.a);
	    printf("Enter b : ?\n");
	    scanf("%u", &client_data.b);
	    */
	    /*Code for task 2 goes here*/
	    /*....*/
	    printf("Enter student name : ?\n");
	    //gets(student.name);
	    scanf("%s", student.name);
	    //printf("student name : %s\n", student.name);

	    printf("Enter student age: ?\n");
	    scanf("%d", &student.age);
	    //printf("student age : %u\n", student.age);


	    printf("Enter student group : ?\n");
	    scanf("%d", &student.group);
	    //printf("student group : %s\n", student.group);

	    /*step 5 : send the data to server*/
	    /*Again, keep in mind the type of our msg*/

	    /*Code for task 2 goes here*/
	    /*....*/

	    sent_recv_bytes = sendto(sockfd, 
		   &student,//&client_data
		   sizeof(student), 
		   0, 
		   (struct sockaddr *)&dest, 
		   sizeof(struct sockaddr));
	    
	    printf("No of bytes sent = %d\n", sent_recv_bytes);
	   
	    /*Step 6 : Client also wants a reply from server after sending data*/
	    
	    /*recvfrom is a blocking system call, meaning the client program will not run past this point
	     * until the data arrives on the socket from server*/
	    /*Once more, watch the types!!!*/
	    /*Code for task 2 goes here*/
	    /*....*/

	    sent_recv_bytes =  recvfrom(sockfd, (char *)&result, sizeof(student), 0,
		            (struct sockaddr *)&dest, &addr_len);

	    printf("No of bytes received = %d\n", sent_recv_bytes);
	    
	    printf("Name received: %s\nAge recieved: %d\nGroup recieved: %d\n", result.name, result.age, result.group);
    }
}
    

int main(int argc, char **argv) {
    setup_tcp_communication();
    printf("application quits\n");
    return 0;
}

