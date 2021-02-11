#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <bits/ioctls.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

#include "arp.h"
#include "utils.h"

#define TIMEOUT_SEC 5

void init_eth_hdr(int sockfd, struct ether_header *eth_hdr)
{
	uint32_t dg_addr = get_default_gateway_ip();
	arp_resolve(sockfd, dg_addr, eth_hdr->ether_dhost);
	get_interface_mac(sockfd, IFNAME, eth_hdr->ether_shost);
	eth_hdr->ether_type = htons(ETHERTYPE_IP);
}

/*
	- Ping inceput nush daca functioneaza cum trebuie deoarece
	am avut azi probleme cu netul si am stat pe netul de la
	telefon si de abea se misca, dar imi afiseaza PING si
	o adresa dar acolo se opreste nu vrea sa mai arate ceva
	- Traceroute la fel inceput imi afiseaza o ruta nu stiu 
	daca este cea corecta
	Pe scurt nu am idee daca functioneaza cum trebuie ce e
	aici sau nu
	opreaolivia73@gmail.com in caz de am gresit si ai timp
	sa imi explici si ce si de ce :)
*/


void ping(int sockfd, struct in_addr ip_addr, int count)
{
	struct packet pkt;
	init_packet(&pkt);

	struct ether_header *eth_hdr = (struct ether_header *)pkt.payload;
	struct iphdr *ip_hdr = (struct iphdr *)(pkt.payload + IP_OFF);
	struct icmphdr *icmp_hdr = (struct icmphdr *)(pkt.payload + ICMP_OFF);

	/* Init Ethernet header. */
	init_eth_hdr(sockfd, eth_hdr);

	char *ip_s = inet_ntoa(ip_addr);
	printf("PING %s ...\n", ip_s);

	/* TODO 2 implementați un ciclu astfel încât să dați ping de count ori. */
	/* TODO 1 Dacă vreți ca pachetul de ICMP să conțină date in
	 * payload, updatați dimensiunea totală a pachetului). */
	int sequence = 0;
	while (count >= 0) {

		pkt.size = sizeof(struct ether_header) + sizeof(struct iphdr)
			+ sizeof(struct icmphdr);

		/* TODO 1 Inițializați headerul de IPv4 cu informațiile necesare.
		 * Calculați checksum. Folosiți funcția 'checksum' din utils.h
		 */
		ip_hdr->version = 4;

		ip_hdr->ihl = 5;
		ip_hdr->tos = 0;
		ip_hdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
		ip_hdr->id = htons(0);
		ip_hdr->frag_off = 0;
		ip_hdr->ttl = 64;
		ip_hdr->protocol = IPPROTO_ICMP;
		get_interface_ip(sockfd, IFNAME, &ip_hdr->saddr);
		ip_hdr->daddr = ip_addr.s_addr;
		ip_hdr->check = 0;
		ip_hdr->check = checksum(ip_hdr, 4 * ip_hdr->ihl);

		/* TODO 1 Inițializați headerul de ICMP cu informațiile necesare.
		 * Calculați checksum. Folosiți funcția 'checksum' din utils.h
		 */
		icmp_hdr->code = 0;

		icmp_hdr->type = ICMP_ECHO;
		icmp_hdr->un.echo.id = 0;
		icmp_hdr->un.echo.sequence = sequence;
		icmp_hdr->checksum = 0;
		icmp_hdr->checksum = checksum(icmp_hdr, sizeof(struct icmphdr));

		struct packet reply;
		double time_elapsed = time_packet(sockfd, &pkt, &reply);

		// normalize to miliseconds
		time_elapsed = time_elapsed * 1000;

		struct iphdr *rip_hdr = (struct iphdr *)(reply.payload + IP_OFF);
		struct icmphdr *ricmp_hdr = (struct icmphdr *)(reply.payload + ICMP_OFF);

		/* TODO 1 afișați informațiile relevante din pachetul reply */
		struct in_addr reply_ip;
		reply_ip.s_addr = rip_hdr->saddr;

		printf("from:%s ", inet_ntoa(reply_ip));
		printf("seq:%d ", ricmp_hdr->un.echo.sequence);
		printf("ttl:%d ", rip_hdr->ttl);
		printf("time:%f sec\n", time_elapsed);

		count --;
		sequence ++;
	}
}

void traceroute(int sockfd, struct in_addr ip_addr)
{
	struct packet pkt;
	init_packet(&pkt);

	struct ether_header *eth_hdr = (struct ether_header *)pkt.payload;
	struct iphdr *ip_hdr = (struct iphdr *)(pkt.payload + IP_OFF);
	struct icmphdr *icmp_hdr = (struct icmphdr *)(pkt.payload + ICMP_OFF);

	/* Init Ethernet header. */
	init_eth_hdr(sockfd, eth_hdr);

	char *ip_s = inet_ntoa(ip_addr);
	printf("TRACEROUTE %s ...\n", ip_s);

	/* TODO 3 Implementați funcționalitatea de traceroute. Porniți de la
	 * funcționalitatea implementată în funcția de ping. */
	int found = 0, ttl = 0;
	while (found != 1 ){
		pkt.size = sizeof(struct ether_header) + sizeof(struct iphdr)
			+ sizeof(struct icmphdr);

		ip_hdr->version = 4;

		ip_hdr->ihl = 5;
		ip_hdr->tos = 0;
		ip_hdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
		ip_hdr->id = htons(0);
		ip_hdr->frag_off = 0;
		ip_hdr->ttl = ttl;
		ip_hdr->protocol = IPPROTO_ICMP;
		get_interface_ip(sockfd, IFNAME, &ip_hdr->saddr);
		ip_hdr->daddr = ip_addr.s_addr;
		ip_hdr->check = 0;
		ip_hdr->check = checksum(ip_hdr, 4 * ip_hdr->ihl);

		icmp_hdr->code = 0;

		icmp_hdr->type = ICMP_ECHO;
		icmp_hdr->un.echo.id = 0;
		icmp_hdr->un.echo.sequence = htons(0);
		icmp_hdr->checksum = 0;
		icmp_hdr->checksum = checksum(icmp_hdr, sizeof(struct icmphdr));

		struct packet reply;
		double time_elapsed = time_packet(sockfd, &pkt, &reply);

		time_elapsed = time_elapsed * 1000;

		struct iphdr *rip_hdr = (struct iphdr *)(reply.payload + IP_OFF);
		struct icmphdr *ricmp_hdr = (struct icmphdr *)(reply.payload + ICMP_OFF);

		struct in_addr reply_ip;
		reply_ip.s_addr = rip_hdr->saddr;		
		if (ricmp_hdr->type == 0 ){
			found = 1;
			printf(" %s\n", inet_ntoa(reply_ip));
		}else {
			ttl++;
			printf(" %s ->", inet_ntoa(reply_ip));
		}	

	}
}

int get_sock(const char *ifname)
{
	int res;
	int sockfd = socket(AF_PACKET, SOCK_RAW, 768);
	DIE(sockfd == -1, "socket");

	struct ifreq intf;
	strcpy(intf.ifr_name, ifname);
	res = ioctl(sockfd, SIOCGIFINDEX, &intf);
	DIE(res == -1, "ioctl SIOCGIFINDEX");

	struct sockaddr_ll addr;
	memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = intf.ifr_ifindex;

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	res = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	DIE(res == -1, "setsockopt");

	res = bind(sockfd, (struct sockaddr *)&addr , sizeof(addr));
	DIE(res == -1, "bind");
	return sockfd;
}

void show_usage(const char *pname)
{
	fprintf(stderr, "Usage:\n"
			"%1$s ping <ip> [<count>]\n"
			"OR\n"
			"%1$s traceroute <ip>\n", pname);
	exit(1);
}

enum run_mode {
	PING,
	TRACEROUTE,
};

enum run_mode validate(int argc, char **argv)
{
	int v = 1;
	enum run_mode mode;
	if (!strcmp(argv[1], "ping")) {
		mode = PING;
	} else if (!strcmp(argv[1], "traceroute")) {
		mode = TRACEROUTE;
	} else {
		show_usage(argv[0]);
	}

	if (!(argc == 3 || (mode == PING && argc == 4)))
		show_usage(argv[0]);

	return mode;
}

int main(int argc, char **argv)
{
	int res;
	enum run_mode mode = validate(argc, argv);

	char *ip_str;
	ip_str = dns_lookup(argv[2]);
	DIE(ip_str == NULL, "dns_lookup");
	struct in_addr ip_addr;
	res = inet_aton(ip_str, &ip_addr);
	DIE(!res, "inet_aton");

	int sockfd = get_sock(IFNAME);
	int count = -1;

	switch (mode) {
	case PING:
		if (argc == 4)
			count = atoi(argv[3]);

		ping(sockfd, ip_addr, count);
		break;
	case TRACEROUTE:
		traceroute(sockfd, ip_addr);
		break;
	}

	return 0;
}
