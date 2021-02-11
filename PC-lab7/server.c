/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "helpers.h"

// Primeste date de pe connfd1 si trimite mesajul receptionat pe connfd2, la serverul de echo connfd1 == connfd2
int receive_and_send(int connfd1, int connfd2)
{
    char buf[BUFLEN];
    int bytes_send = 0;
    int bytes_remaining;
    int bytes_received = 0;

    // TODO 6: Receptionati un mesaj de la connfd1
    bytes_received = recv(connfd1, buf, BUFLEN, 0);

    if (bytes_received != 0) {
        fprintf(stdout, "Received: %s", buf);
    } else {
        return bytes_received;
    }

//-----------------------------------------------
    bytes_remaining = bytes_received;

    // TODO 7: Timiteti mesajul la connfd2
    do{
        int aux = send(connfd2, &buf[bytes_send], bytes_received, 0);
        bytes_send += aux;
        bytes_remaining -= aux;
    } while (bytes_remaining > 0);

    return bytes_received;
}

void run_echo_server(int listenfd)
{
    struct sockaddr_in client_addr;

    int bytes_received;
    int connfd = -1;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    // TODO 4: Ascultati pentru un singur client pe socketul dat
    listen(listenfd, 1);

    // TODO 5: Acceptati o conexiune
    connfd = accept(listenfd, (struct sockaddr *) &client_addr, &socket_len);
    DIE(connfd < 0, "FAIL");

    do {
        bytes_received = receive_and_send(connfd, connfd);
    } while (bytes_received > 0);

    // TODO 8: Inchideti conexiunea si socket-ul clientului
    close(connfd);
}

void run_chat_server(int listenfd)
{
    struct sockaddr_in client_addr1;
    struct sockaddr_in client_addr2;

    int bytes_received;
    int connfd1 = -1;
    int connfd2 = -1;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    // TODO 4: Ascultati pentru doi clineti pe socketul dat
    listen(listenfd, 50); // 

    // TODO 5: Acceptati doua conexiuni
    connfd1 = accept(listenfd, (struct sockaddr *) &client_addr1, &socket_len);
    DIE(connfd1 < 0, "FAIL");

    listen(listenfd, 50);

    connfd2 = accept(listenfd, (struct sockaddr *) &client_addr2, &socket_len);
    DIE(connfd2 < 0, "FAIL");

    
    do {
        bytes_received = receive_and_send(connfd1, connfd2);

        if (bytes_received == 0)
            break;

        bytes_received = receive_and_send(connfd2, connfd1);
    } while (bytes_received > 0);

    // TODO 8: Inchideti conexiunile si socketii creati
    close(connfd1);
    close(connfd2);
    
}

int main(int argc, char* argv[])
{
    int my_socket = -1;
    struct sockaddr_in serv_addr;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    if (argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // TODO 1: Creati un socket TCP pentru receptionarea conexiunilor
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    DIE(my_socket < 0, "FAIL");

    // TODO 2: Completati in serv_addr adresa serverului pentru bind, familia de adrese si portul rezervat pentru server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[2]));

    // TODO 3: Asociati adresa serverului cu socketul creat folosind bind
    bind(my_socket, (struct sockaddr *) &serv_addr, socket_len);



    //run_echo_server(my_socket);
    run_chat_server(my_socket);


    // TODO 9: Inchideti socketul creat
    close(my_socket);

    return 0;
}
