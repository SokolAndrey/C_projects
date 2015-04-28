/*
 ============================================================================
 Name        : udpChat.c
 Author      : Andrei
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int port_number;
const size_t packet_size= 1024;


int main(int argc, char *argv[]) {
	if(argc != 2){
		perror("You should open server with port number in par\n");
		exit(EXIT_FAILURE);
	}
	port_number = atoi(argv[2]);
	int sock;
	struct sockaddr_in addr;
	char buf[packet_size];
	int bytes_read;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		perror("Can not create socket");
		exit(EXIT_FAILURE);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_number);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0){
		perror("Can not bind socket");
		exit(EXIT_FAILURE);
	}
	for(;;){
		bytes_read = recvfrom(sock, buf, packet_size, 0, NULL, NULL);
		buf[bytes_read] = '\0';
		printf(buf);
	}

	return EXIT_SUCCESS;
}

