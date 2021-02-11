// Copyright 2020 Oprea Olivia Maria-Magdalena 323CA <opreaolivia73@gmail.com>
// In colaborare cu echipa de pc care a facut laburile din care am luat codul :)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"

void usage(char *file) {
	fprintf(stderr, "Usage: %s client_id server_address server_port\n", file);
	exit(0);
}

struct tcp_msg_t {
	uint16_t udp_port;
	char* ip;
	char topic_name[50];
	char type;
	char data[1501];
};

int main(int argc, char *argv[]) {
	int sockfd, n, ret;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];
	struct tcp_msg_t* recv_msg;

	fd_set read_fds;
    fd_set tmp_fds;

    int fdmax;

    FD_ZERO(&tmp_fds);
    FD_ZERO(&read_fds);

	if (argc < 4) {
		usage(argv[0]);
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	FD_SET(sockfd, &read_fds);
    fdmax = sockfd;
    FD_SET(0, &read_fds);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

	n = send(sockfd, argv[1], strlen(argv[1]) + 1, 0);
	DIE(n < 0, "send_id");

	while (1) {
		tmp_fds = read_fds;

		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		if(FD_ISSET(0, &tmp_fds)){
	  		// se citeste de la tastatura
			memset(buffer, 0, BUFLEN);
			fgets(buffer, BUFLEN - 1, stdin);

			if (strncmp(buffer, "exit", 4) == 0) {
				break;
			}

			// se trimite mesaj la server
			n = send(sockfd, buffer, strlen(buffer), 0);
			DIE(n < 0, "send");
		} else {
			memset(buffer, 0, BUFLEN);
			int received = recv(sockfd, buffer, BUFLEN, 0);
			if (received == 0) {
				break;
			}

			recv_msg = (struct tcp_msg_t*) &buffer;
			printf("%s:%hu - %s - %s - %s\n", recv_msg->ip, recv_msg->udp_port,
                   recv_msg->topic_name, recv_msg->type, recv_msg->data);
		}
	}

	close(sockfd);

	return 0;
}
