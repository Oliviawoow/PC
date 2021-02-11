#include "skel.h"

int interfaces[ROUTER_NUM_INTERFACES];
struct route_table_entry *rtable;
int rtable_size;

struct arp_entry *arp_table;
int arp_table_len;

char my_arp_table[4][18] = {
	"de:ad:be:ef:00:00",
	"de:ad:be:ef:00:01",
	"de:ad:be:ef:00:02",
	"de:ad:be:ef:00:03",
};

void sort_table(struct route_table_entry *rtable, int size_table){
	struct route_table_entry aux;
	for (int i = 0; i < size_table; i++){
		for (int j = 0; j < size_table; j++){
			if (rtable[i].mask > rtable[j].mask){
				memcpy(&aux, &rtable[i], sizeof(struct route_table_entry));
				memcpy(&rtable[i], &rtable[j], sizeof(struct route_table_entry) );
				memcpy(&rtable[j], &aux, sizeof(struct route_table_entry));
			}
		}
	}
}

int longest_prefix_match(struct route_table_entry *rtable, int size_rt, uint32_t destination_addr){
	for (int i = 0; i < size_rt; i++){
		if ((rtable[i].prefix & rtable[i].mask) == (destination_addr & rtable[i].mask )){
			return i;
		}
	}
	return -1;
}

/*
 Returns a pointer (eg. &rtable[i]) to the best matching route
 for the given dest_ip. Or NULL if there is no matching route.
*/
struct route_table_entry *get_best_route(__u32 dest_ip) {
	/* TODO 1: Implement the function */
	int i, max_bits = 0; 
	for(i = 0; i < rtable_size; i ++){
		if(((dest_ip && rtable[i].mask == rtable[i].prefix) && rtable[i].mask)
		 && (__builtin_popcount(rtable[i].mask) > max_bits)){
			max_bits = __builtin_popcount(rtable[i].mask);
			return &rtable[i];
		}
	}
	return NULL;
}

/*
 Returns a pointer (eg. &arp_table[i]) to the best matching ARP entry.
 for the given dest_ip or NULL if there is no matching entry.
*/
struct arp_entry *get_arp_entry(__u32 ip) {
    /* TODO 2: Implement */
    int i;
	for(i=0;i<arp_table_len;i++){
		if(ip == arp_table[i].ip){
			return &arp_table[i];
		}
	}

    return NULL;
}

int main(int argc, char *argv[])
{
	msg m;
	int rc;

	init();
	rtable = malloc(sizeof(struct route_table_entry) * 100);
	arp_table = malloc(sizeof(struct  arp_entry) * 100);
	DIE(rtable == NULL, "memory");
	rtable_size = read_rtable(rtable);
	parse_arp_table();
	/* Students will write code here */
	sort_table( rtable, rtable_size);
	uint8_t *mac = malloc(6 * sizeof(uint8_t));

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		struct ether_header *eth_hdr = (struct ether_header *)m.payload;
		struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
		if (ntohs(eth_hdr->ether_type) != ETHERTYPE_IP) {
			//printf("something1\n");
			continue;
		}
		/* TODO 3: Check the checksum */
		if (ip_checksum (ip_hdr, sizeof(struct iphdr)) != 0) {
			printf("Pachet eronat\n");
			continue; //drop
		}

		/* TODO 4: Check TTL >= 1 */
		if ( ip_hdr->ttl <= 1){
			//printf("something3\n");
			continue;
		}

		/* TODO 5: Find best matching route (using the function you wrote at TODO 1) */
		int next_hop_index = longest_prefix_match( rtable, rtable_size, ip_hdr->daddr);
		if ( next_hop_index < 0){
			//printf("something4\n");
			continue;
		}

		/* TODO 6: Update TTL and recalculate the checksum */
		ip_hdr->check = 0;
		ip_hdr->ttl--;
		ip_hdr->check = ip_checksum(ip_hdr, sizeof(struct iphdr));


		/* TODO 7: Find matching ARP entry and update Ethernet addresses */
		//extract MAC destination address
		hwaddr_aton(my_arp_table[(ip_hdr->daddr<<8)>>24], mac);
		memcpy(eth_hdr->ether_dhost, mac, 6);

		//extract MAC source address
		hwaddr_aton(my_arp_table[0], mac);
		memcpy(eth_hdr->ether_shost, mac, 6);
		printf("something5, %d\n", next_hop_index);

		/* TODO 8: Forward the pachet to best_route->interface */
		send_packet( rtable[next_hop_index].interface, &m);
	}
}
