#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000


int main(int argc,char** argv){
  init(HOST,PORT);
  msg t;
  int citit_fisier;

  int f = open("fisier_in.txt", O_RDONLY);
  if (f < 0){
    perror("ERROR\n");
    exit(1);
  } // deschidere fisier

  //Send dummy message:
  printf("[send] Sending dummy...\n");
  while (1) { // cat timp are ce citii
    citit_fisier = read(f, t.payload, sizeof(t.payload)); // citeste continutul fisierului
    t.len = citit_fisier; // lungime ce a citit
    if (t.len != 0) { // cat timp lungimea a ceea ce a citit nu e 0
      send_message(&t); // trimite si lungime si mesaj
      // Check response:
      if (recv_message(&t)<0){
        perror("Receive error ...");
        return -1;
      }
     // else {
        //printf("[send] Got reply with payload: %s\n", t.payload);
      //}
    } else {
      break;
    }
  }
  send_message(&t); // cand a ajuns sa nu mai aiba ce sa citeasca ca sa nu astepte ACK

  return 0;
}
