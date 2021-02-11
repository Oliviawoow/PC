#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001


char byte_parity(msg *t);

int main(void)
{
	msg r;
	int i, res;
	
	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);
	
	for (i = 0; i < COUNT; i++) {
		/* wait for message */
		res = recv_message(&r);
		if (res < 0) {
			perror("[RECEIVER] Receive error. Exiting.\n");
			return -1;
		}

		if (r.parity = byte_parity(&r)) {
			
		}
		
		/* send dummy ACK */
		res = send_message(&r);
		if (res < 0) {
			perror("[RECEIVER] Send ACK error. Exiting.\n");
			return -1;
		}
	}

	printf("[RECEIVER] Finished receiving..\n");
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