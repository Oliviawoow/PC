/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	mini-server de backup fisiere
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
	fprintf(stderr,"Usage: %s server_port file\n",file);
	exit(0);
}

/*
*	Utilizare: ./server server_port nume_fisier
*/
int main(int argc,char**argv)
{
	int fd;

	if (argc!=3)
		usage(argv[0]);
	
	struct sockaddr_in my_sockaddr, from_station ;
	char buf[BUFLEN];

	memset(&from_station, 0, sizeof(from_station));
	memset(&my_sockaddr, 0, sizeof(my_sockaddr));

	/*Deschidere socket*/

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("Eroare la deschiderea socket-ului(server)!\n");
		return -1;
	}
	
	/*Setare struct sockaddr_in pentru a asculta pe portul respectiv */

	my_sockaddr.sin_family = AF_INET;
	my_sockaddr.sin_port = htons(atoi(argv[2]));
	my_sockaddr.sin_addr.s_addr = INADDR_ANY;
	
	
	/* Legare proprietati de socket */

	bind(sock, (struct sockaddr*)(&my_sockaddr), sizeof(my_sockaddr));
	if (bind < 0) {
		printf("Eroare la binding!\n");
		return -1;
	}
	
	
	/* Deschidere fisier pentru scriere */
	DIE((fd=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644))==-1,"open file");
	
	/*
	*  cat_timp  mai_pot_citi
	*		citeste din socket
	*		pune in fisier
	*/

	socklen_t size = sizeof(from_station);
	printf("We re halfway there!\n");
	int rez;
	
	while(1){
		rez = recvfrom(sock, buf, BUFLEN, MSG_WAITALL, (struct sockaddr*)(&from_station), &size);
		if (rez == -1) {
			printf("Recvfrom failed!\n");
			return -1;
		}
		printf("%s\n", buf);
		write(fd, buf, sizeof(buf));
	}
	
	/*Inchidere socket*/	

	close(sock);
	
	/*Inchidere fisier*/

	close(fd);


	return 0;
}
