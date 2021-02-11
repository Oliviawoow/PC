#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001


int main(int argc,char** argv){
  msg r;
  init(HOST,PORT);
  int scriere_fisier;

  int g = open("fisier_out.txt", O_WRONLY);
  if (g < 0){
    perror("ERROR\n");
    exit(1);
  }

  while (1) { // cat timp are ce sa scrie
    if (recv_message(&r)<0){
      perror("Receive message");
      return -1;
    }
    //printf("[recv] Got msg with payload: <%s>, sending ACK...\n", r.payload);
    scriere_fisier = write(g, r.payload, r.len);
    if (r.len == 0) {
      break;
    }
  
    // Send ACK:
    sprintf(r.payload,"%s", "ACK");
    r.len = strlen(r.payload) + 1;
    send_message(&r);
    printf("[recv] ACK sent\n");
  }

  return 0;
}
