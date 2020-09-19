#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>

#define BUF_SIZE 1024
#define PORT "8888"

struct addrinfo hints, *res;

int main() {
	const char* MESSAGE = "Hello from this part of the world!";
	char buf[BUF_SIZE] = {0};
	char address[INET6_ADDRSTRLEN];
	int status;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
		printf("Error occurred on getting address info: %s", gai_strerror(status));
		exit(2);
	}

	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0) {
		printf("Error occuurred on creating socket file\n");
		exit(1);
	}	

	inet_ntop(res->ai_family, (struct sockaddr_in*)res->ai_addr, address, sizeof(address)); 
	if(connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
		printf("Error occurred whilst connecting to socket on address %s", address);
		exit(1);
	}
	printf("Successfully connected to server %s on port %s\n", address, PORT);

	if(send(sockfd, MESSAGE, sizeof(MESSAGE), 0) < 0) {
		printf("Error on sending user message");
		exit(1);
	}

	// Read the response into the output buffer
	read(sockfd, buf, BUF_SIZE);

	printf("Output from server is %s", buf);

	freeaddrinfo(res);
	
	return 0;	
}
