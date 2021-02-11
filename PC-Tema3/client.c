// LAB 10

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

#define BUFFLEN 4096
#define LINELEN 1000

int main(int argc, char const *argv[]) {
	char *message;
    char *response;
    int sockfd;

    char buf[BUFFLEN];
    char* reg[BUFFLEN];
    char* cookie;
    char* token;
    char* acces;

    while (1) {
    	printf("comanda: ");
    	scanf("%s", buf);

    	if (strcmp(buf, "register") == 0) {
    		memset(buf, 0, BUFFLEN);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		printf("username: ");
    		scanf("%s", buf);
    		strcat(reg, "username=");
    		strcat(reg, buf);
    		memset(buf, 0, BUFFLEN);
    		printf("password: ");
    		scanf("%s", buf);
    		strcat(reg, "&password=");
    		strcat(reg, buf);

    		message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/tema/auth/register", "application/x-www-form-urlencoded", reg, NULL, 0, NULL);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		free(response);
    		close(sockfd);
    	}

    	if (strcmp(buf, "login") == 0) {
    		memset(buf, 0, BUFFLEN);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		printf("username: ");
    		scanf("%s", buf);
    		strcat(reg, "username=");
    		strcat(reg, buf);
    		memset(buf, 0, BUFFLEN);
    		printf("password: ");
    		scanf("%s", buf);
    		strcat(reg, "&password=");
    		strcat(reg, buf);

    		message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/tema/auth/login", "application/x-www-form-urlencoded", reg, NULL, 0, NULL);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		cookie = get_cookie(response);
    		free(response);
    		close(sockfd);
    	}

    	if (strcmp(buf, "enter_library") == 0) {
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/tema/library/access", NULL, cookie, NULL);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		token = get_token(response);
    		free(response);
    		close(sockfd);
    	}

    	if (strcmp(buf, "get_books") == 0) {
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/tema/library/books", NULL, cookie, token);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		free(response);
    		close(sockfd);
    	}

    	if (strcmp(buf, "get_book") == 0) {
    		memset(buf, 0, BUFFLEN);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		printf("id: ");
    		scanf("%s", buf);
    		acces = calloc(BUFLEN, sizeof(char));
    		acces = strdup("/api/v1/tema/library/books/");
    		strcat(acces, buf);
    		message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", acces, NULL, cookie, token);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		free(response);
    		free(acces);
    		close(sockfd);
    	}

    	if(strcmp(buf, "add_book") == 0) {
    		memset(buf, 0, BUFFLEN);
    		memset(reg, 0, BUFFLEN);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		printf("title= ");
    		scanf("%s", buf);
    		strcat(reg, "title=");
    		strcat(reg, buf);
    		memset(buf, 0, BUFFLEN);
    		printf("author= ");
    		scanf("%s", buf);
    		strcat(reg, "&author=");
    		strcat(reg, buf);
    		memset(buf, 0, BUFFLEN);
    		printf("genre= ");
    		scanf("%s", buf);
    		strcat(reg, "&genre=");
    		strcat(reg, buf);
    		memset(buf, 0, BUFFLEN);
    		printf("publisher= ");
    		scanf("%s", buf);
    		strcat(reg, "&publisher=");
    		strcat(reg, buf);
    		memset(buf, 0, BUFFLEN);
    		printf("page_count= ");
    		scanf("%s", buf);
    		strcat(reg, "&page_count=");
    		strcat(reg, buf);
    		memset(buf, 0, BUFFLEN);

    		message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/tema/library/books", "application/x-www-form-urlencoded", reg, cookie, 0, token);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		free(response);
    		close(sockfd);
    	}

    	if (strcmp(buf, "delete_book") == 0) {
    		memset(buf, 0, BUFFLEN);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		printf("id: ");
    		scanf("%s", buf);
    		acces = calloc(BUFLEN, sizeof(char));
    		acces = strdup("/api/v1/tema/library/books/");
    		strcat(acces, buf);
    		message = compute_delete_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", acces, NULL, cookie, token);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		free(response);
    		free(acces);
    		close(sockfd);
    	}

    	if (strcmp(buf, "logout") == 0) {
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/tema/auth/logout", NULL, cookie, NULL);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		printf("%s\n", response);
    		free(response);
    		close(sockfd);
    	}

    	if (strcmp(buf, "exit") == 0) {
    		break;
    	}
    }
    
	return 0;
}
