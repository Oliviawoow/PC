# Protocoale de comunicatii:
# Laborator 8: Multiplexare
# Makefile

CFLAGS = -Wall -g

all: server client_tcp client_udp

# Compileaza server.c
server: server.c

# Compileaza client.c
client_tcp: client_tcp.c

client_udp: client_udp.c

.PHONY: clean run_server run_client


clean:
	rm -f server client
