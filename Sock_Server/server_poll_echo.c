#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<poll.h>
#include<errno.h>

#define PORT "8888"
#define NUM_QUEUE 10

struct addrinfo hints, *res;
struct sockaddr_storage client;
socklen_t addrlen;

int get_socket() {
        int status;
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
		perror("Error occuurred on creating socket file\n");
		exit(1);
	}


        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); 

	if(bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
		printf("Bind failed: ");
		if(errno == EADDRINUSE) {
			printf("Address in use\n");
		        exit(2);
                }
	}

        if(listen(sockfd, NUM_QUEUE) < 0) {
		perror("Error on listen creation\n");
	        exit(2);
        }

        printf("Socket obtained with id: %d\n", sockfd);

       return sockfd;
}


void add_pollfd(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size) {
        if(*fd_count  == *fd_size) {
               *fd_size *= 2;
               *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
        }
        
        (*pfds)[*fd_count].fd = newfd;
        (*pfds)[*fd_count].events = POLLIN;

        (*fd_count) += 1;
}


void del_pollfd(struct pollfd pfds[], int index, int* fd_count) {
        pfds[index] = pfds[(*fd_count) - 1];

        (*fd_count) -= 1;
}

int main() {
        const char* MESSAGE = "Hello again!";

        int sockfd = get_socket();
        int clientfd;
        
        char address[INET6_ADDRSTRLEN];
        char buf[INET6_ADDRSTRLEN];
        int fd_count = 1;
        int fd_size = 5;
        struct pollfd *pfds = malloc(sizeof(*pfds) * fd_size);
        
        pfds[0].fd = sockfd;
        pfds[0].events = POLLIN;

        printf("Server listening on port %s\n", PORT);

        while(1) {
                if(poll(pfds, fd_count, -1) < 0) {
                        printf("Error on calling poll()\n");
                }

                for(int i = 0; i < fd_count; i++) {
                        if(pfds[i].revents & POLLIN) {
                                if(pfds[i].fd == sockfd) {        
                                        addrlen = sizeof(client);
                                        if((clientfd = accept(sockfd, (struct sockaddr*)&client, &addrlen)) < 0) {
                                                perror("Error on creating new connection\n");
                                                continue;
                                        }
                                        add_pollfd(&pfds, clientfd, &fd_count, &fd_size);
	                                inet_ntop(client.ss_family, &((struct sockaddr_in*)&client)->sin_addr, address, sizeof(address));
                                        printf("Connection received from host %s\n", address);
                                
                                        if(send(clientfd, MESSAGE, sizeof(MESSAGE), 0) < 0) {
		                                perror("Error on sending response message\n");
	                                }               
	
	                                //close(clientfd);
                                } else {
                                        // We're a client
                                        int nbytes = recv(pfds[i].fd, buf, sizeof(buf), 0);  
                                        int sender_fd = pfds[i].fd;
                                        if(nbytes <= 0) { 
                                                printf("Error on socket recv()\n");
                                                
                                                close(sender_fd);
                                                printf("User on socket id %d disconnected\n", sender_fd); 
                                                del_pollfd(pfds, i, &fd_count);
                                        } else {
                                                for(int i = 0; buf[i] != '\0'; i++) {
                                                        if(96 < buf[i] && buf[i] < 123) {
                                                                buf[i] -= 32;
                                                        }
                                                }
                                                for(int j = 0; j < fd_count; j++) {
                                                        int dest_fd = pfds[j].fd;
                                                        if(dest_fd != sockfd) {
                                                                if(send(dest_fd, buf, nbytes, 0) < 0) {
                                                                        printf("Error occured on sending packet to socket %d\n", dest_fd);
                                                                }
                                                        }
                                                }        
                                        }
                                }
                        } 
                }
        }
}
