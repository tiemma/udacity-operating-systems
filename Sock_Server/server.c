#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<errno.h>

#define PORT "8888"
#define NUM_QUEUE 10

struct addrinfo hints, *res;
struct sockaddr_storage client;
socklen_t sin_size;

int main() {
	int status, client_fd;
	char address[INET6_ADDRSTRLEN];
	const char* RESP = "pong";
        int yes = 1;

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
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); 

	if(bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
		printf("Bind failed: address in use\n");
		if(errno == EADDRINUSE) {
			printf("Address in use\n");
		}
	}


	printf("Server listening on port %s\n", PORT);
	if(listen(sockfd, NUM_QUEUE) < 0) {
		printf("Error on listen creation\n");
	}
	while(1) {	
	        sin_size = sizeof(client);
	        if((client_fd = accept(sockfd, (struct sockaddr*)&client, &sin_size)) < 0) {
		        printf("Error on setting up new connection\n");
	        }

	        inet_ntop(client.ss_family, &((struct sockaddr_in*)&client)->sin_addr, address, sizeof(address));
	        printf("Connection received from host %s\n", address);

	        if(send(client_fd, RESP, sizeof(RESP), 0) < 0) {
		        printf("Error on sending response message");
	        }
	
	        close(client_fd);
	}
	
	
	freeaddrinfo(res);
        
        return 0;
}




