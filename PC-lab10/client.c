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

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

        
    // Ex 1.1: GET dummy from main server
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/dummy", NULL, NULL, 0);
    send_to_server(sockfd, message);
    printf("%s", message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    shutdown(sockfd, 2);
    close_connection(sockfd);
    // Ex 1.2: POST dummy and print response from main server
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/dummy", "application/x-www-form-urlencoded", NULL, 0, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    shutdown(sockfd, 2);
    close_connection(sockfd);
    // Ex 2: Login into main server
    char **parametrii = calloc(2, sizeof(char *));
    parametrii[0] = "username=student";
    parametrii[1] = "password=student";
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request("3.8.116.10", "/api/v1/auth/login", "application/x-www-form-urlencoded", parametrii, 2, NULL, 0);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // Ex 3: GET weather key from main server
    message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", "/api/v1/auth/login", NULL, NULL, 0);
    send_to_server(sockfd, message);
    printf("%s", message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // Ex 4: GET weather data from OpenWeather API
    char **key = calloc(1, sizeof(char*));
    key[0] = "connect.sid=s%3AyLjxV7DWqBbDD6AXRAbf-mOmiZr_YOde.BBn9im0io1%2BMhSDj6jrBm0CmRuNj3IJ6kfIazW8k9Wo";
    message=compute_get_request("3.8.116.10", "/api/v1/weather/key", NULL, key, 1);
    send_to_server(sockfd,message);
    response=receive_from_server(sockfd);
    printf("%s\n",response);
    message = compute_get_request("3.8.116.10", "/api/v1/auth/logout", NULL, key, 1);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n",response);
    // Ex 5: POST weather data for verification to main server
    char **keyverif = calloc(1, sizeof(char*));
    keyverif[0] = "b912dd495585fbf756dc6d8f415a7649";
    sockfd = open_connection("api.openweathermap.org", 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("api.openweathermap.org", "/data/2.5/weather", NULL, keyverif, 1);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    printf("%s\n", response);
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!
    shutdown(sockfd, 2);
    close_connection(sockfd);

    return 0;
}
