/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
*/

#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <string.h>

#include "helpers.h"

void usage(char*file)
{
	fprintf(stderr,"Usage: %s ip_server port_server\n",file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/
int main(int argc,char**argv)
{
	if (argc!=3)
		usage(argv[0]);
	
	int fd;
	struct sockaddr_in to_station;
	char buf[BUFLEN];


	/*Deschidere socket*/

	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("Eroare la deschiderea socket-ului(client)!\n");
		return -1;
	}
			

	memset(&to_station, 0, sizeof(to_station));

	to_station.sin_family = AF_INET;
	to_station.sin_port = htons (atoi(argv[2])); 
	inet_aton(argv[1], &(to_station.sin_addr));

	scanf("%s", buf);
	sendto(sock, buf, strlen(buf) + 1, MSG_CONFIRM, (struct sockaddr*)(&to_station), sizeof(to_station));

	printf("AM TRIMIS %s\n", buf);

	/*Inchidere socket*/

	close(sock);

	return 0;
}
