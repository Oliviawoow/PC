#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "helpers.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	
	int tcp_sockfd, newtcp_sockfd, portno, udp_sockfd;
	char buffer[BUFLEN];
	struct sockaddr_in tcp_addr, udp_addr, cli_addr;
	int n, i, ret;
	socklen_t clilen;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	int sum = 0;

	if (argc < 2) {
		usage(argv[0]);
	}

	// se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	udp_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	DIE(udp_sockfd < 0, "udp_socket");

	tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(tcp_sockfd < 0, "tcp_socket");

	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");

	udp_addr.sin_family = tcp_addr.sin_family = AF_INET;
	udp_addr.sin_port = tcp_addr.sin_port = htons(portno);
	udp_addr.sin_addr.s_addr = tcp_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(udp_sockfd, (struct sockaddr *) &udp_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "udp_bind");

	ret = bind(tcp_sockfd, (struct sockaddr *) &tcp_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "tcp_bind");

	ret = listen(tcp_sockfd, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(udp_sockfd, &read_fds);
	FD_SET(tcp_sockfd, &read_fds);
	fdmax = tcp_sockfd;

	socklen_t size = sizeof(cli_addr);

	while (1) {
		tmp_fds = read_fds; 
		
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				if (i == tcp_sockfd) {
					// a venit o cerere de conexiune TCP pe socketul inactiv (cel cu listen),
					// pe care serverul o accepta
					clilen = sizeof(cli_addr);
					newtcp_sockfd = accept(tcp_sockfd, (struct sockaddr *) &cli_addr, &clilen);
					DIE(newtcp_sockfd < 0, "accept");

					memset(buffer, 0, BUFLEN);
					sprintf(buffer, "Suma este %d", sum);

					send(newtcp_sockfd, buffer, strlen(buffer) +1, 0);
					close(newtcp_sockfd);

				} else if (i == udp_sockfd) {
					int rez = recvfrom(udp_sockfd, buffer, BUFLEN, MSG_WAITALL, (struct sockaddr*)(&cli_addr), &size);
					if (rez == -1) {
						printf("Recvfrom failed!\n");
						return -1;
					}
					sum = sum + atoi(buffer);
					printf("Am primit %s de la %s:%d\n", buffer, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
				} 
			}
		}
	}

	close(tcp_sockfd);

	return 0;
}
