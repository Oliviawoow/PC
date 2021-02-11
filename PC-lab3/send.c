#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

char byte_parity(msg *t);

int main(int argc, char *argv[])
{
	msg t, ack;
	int BDP = atoi (argv[1]);
	
	printf("[SENDER] Starting.\n");	
	init(HOST, PORT);

	printf("[SENDER]: BDP=%d\n", atoi(argv[1])); 

	int w = (BDP * 1000) / (MSGSIZE * 8);
	int mesaje_trimise = 0;

	memset(&t, 0, sizeof(msg));
	t.len = MSGSIZE;

	for(int i = 0; i < w; i++) {
		t.parity = byte_parity(&t);
    	send_message(&t);
	}

	mesaje_trimise = w;
	
	while (mesaje_trimise < COUNT) {
		recv_message(&ack);
		t.parity = byte_parity(&t);
    	send_message(&t);
    	mesaje_trimise++;
	}

	for(int i = 0; i < w; i++) {
	    recv_message(&ack);
	}

	printf("[SENDER] Job done, all sent.\n");
		
	return 0;
}

char byte_parity(msg *t) {
	char rez = 0;
	char c;
	for (int i = 0; i < MSGSIZE; i++) {
		while (c) {
			c = t->payload[i];
			if ((c & 1) == 1) {
				rez++;
			}
			c = c >> 1;
		}
		rez = rez % 2;
	}

	return rez;
}