#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#define BUF_SIZE 1024
#define PORT 8888
#define ADDRESS "127.0.0.1"

struct sockaddr_in serv_addr;

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	const char* MESSAGE = "Hello from this part of the world!";
	char buf[BUF_SIZE] = {0};
	if (sockfd < 0) {
		printf("Error occuurred on creating socket file\n");
		exit(1);
	}	

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) != 1) {
		printf("Error occurred on serializing server address");
		exit(1);
	}	

	if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
		printf("Error occurred whilst connecting to socket on address %s", ADDRESS);
		exit(1);
	}

	if(send(sockfd, MESSAGE, sizeof(MESSAGE), 0) < 0) {
		printf("Error on sending user message");
		exit(1);
	}

	// Read the response into the output buffer
	read(sockfd, buf, BUF_SIZE);

	printf("Output from server is %s", buf);
	
	return 0;	
}
