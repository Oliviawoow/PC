#include "./include/skel.h"
#include <bits/stdc++.h>

void citire_tabela_rutare(std::vector <std::unordered_map <int, std::pair <int, int>>> &tabela_de_rutare);
void citire_tabela_arp(std::vector <std::pair <int, std::vector <uint8_t>>> &tabela_de_arp);
char* conversie(std::string &param);
std::pair <int, int> interogare_tabela_de_rutare(std::vector <std::unordered_map <int, std::pair <int, int>>> &tabela_de_rutare, __u32 &destinatie);
// ciordita din lab 4
uint16_t ip_checksum(void* vdata,size_t length);

int main(int argc, char *argv[])
{
	packet m;
	int rc;

	// hash-map
	std::vector <std::unordered_map <int, std::pair <int, int>>> tabela_de_rutare(33);
	std::vector <std::pair <int, std::vector <uint8_t>>> tabela_de_arp;

	citire_tabela_rutare(tabela_de_rutare);
	citire_tabela_arp(tabela_de_arp);

	init();

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		/* Students will write code here */
		// Sterpelit din lab 4
		struct ether_header *eth_hdr = (struct ether_header *)m.payload;
		struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
		
		// ciordit din lab 4
		// checksum
		uint16_t oldChecksum = ip_hdr->check;
		ip_hdr->check = 0;

		if (oldChecksum != ip_checksum(ip_hdr, sizeof(struct iphdr))) {
			// pachet eronat
			continue;
		}

		// tot din lab 4
		// check TTL
		if (ip_hdr->ttl < 1) {
			continue;
		}
		
		// cauta perechea unde se afla si next_hop
		std::pair <int, int> pereche_gasita = interogare_tabela_de_rutare(tabela_de_rutare, ip_hdr -> daddr);
		if (pereche_gasita. first == 0 && pereche_gasita.second == 0) {
			// perechea nu e gasita
			continue;
		}
		int IP_next_hop = pereche_gasita.first;
		int IP_interfata = pereche_gasita.second;

		// guess what tot lab 4 :)
		ip_hdr->ttl--;
		ip_hdr->check = 0;
		ip_hdr->check = ip_checksum(ip_hdr, sizeof(struct iphdr));

		// cautam adresa MAC in tabela arp
		std::vector <uint8_t> MAC_next_hop(0);
		for (int i = 0; i < tabela_de_arp.size(); ++i) {
			if (IP_next_hop == tabela_de_arp[i].first) {
				MAC_next_hop = tabela_de_arp[i].second;
			}
		}
		if (MAC_next_hop.size() == 0) {
			continue;
		}

		uint8_t MAC_char_next_hop[6];
		for (int i = 0; i < 6; ++i) {
			MAC_char_next_hop[i] = MAC_next_hop[i];
		}

		// setare Ethernet header si trimitere pachet
		get_interface_mac(IP_interfata, (uint8_t *)&eth_hdr->ether_shost);
		memcpy(eth_hdr->ether_dhost, MAC_char_next_hop, sizeof(MAC_char_next_hop));
		send_packet(IP_interfata, &m);

	}
}

void citire_tabela_rutare(std::vector <std::unordered_map <int, std::pair <int, int>>> &tabela_de_rutare) {
	std::ifstream in("rtable.txt");
	std::string prefix, next_hop, masca, interfata;


	while (1) {
		// am stringuri
		in >> prefix;
		// daca se termina citirea iese :)
		if (in.eof() != 0) {
			break;
		}
		in >> next_hop >> masca >> interfata;

		// convertim din string in int
		char* convert;
		// ce era string o sa fie int
		int convert_prefix, convert_next_hop, convert_masca, convert_interfata;

		convert = conversie(prefix);
		inet_pton(AF_INET, convert, &convert_prefix);
		free(convert);

		convert = conversie(next_hop);
		inet_pton(AF_INET, convert, &convert_next_hop);
		free(convert);

		convert = conversie(masca);
		inet_pton(AF_INET, convert, &convert_masca);
		free(convert);

		convert = conversie(interfata);
		convert_interfata = atoi(convert);
		free(convert);

		/* populam tabela de rutare; nr de biti de 1 din masca e pozitia in vector
		cheia e prefixul si valoarea e o pereche cu next_hop si interfata */
		tabela_de_rutare[__builtin_popcount(convert_masca)][convert_prefix] =
		 std::pair <int, int> (convert_next_hop, convert_interfata);
	}
	in.close();
}

// cam ca functia de mai sus :)
void citire_tabela_arp(std::vector <std::pair <int, std::vector <uint8_t>>> &tabela_de_arp) {
	std::ifstream in("arp_table.txt");
	std::string IP, MAC;

	while(1) {
		in >> IP;
		if (in.eof() != 0) {
			break;
		}
		in >> MAC;

		char* convert;
		int convert_IP;
		uint8_t convert_MAC[6];
		std::vector <uint8_t> std_MAC;

		convert = conversie(IP);
		inet_pton(AF_INET, convert, &convert_IP);
		free(convert);

		convert = conversie(MAC);
		hwaddr_aton(convert, convert_MAC);
		free(convert);
		for (int i = 0; i < 6; ++i) {
			std_MAC.push_back(convert_MAC[i]);
		}

		tabela_de_arp.push_back(std::pair <int, std::vector <uint8_t>> (convert_IP, std_MAC));
	}

	in.close();
}

char* conversie(std::string &param) {
	// aloc memoria
	char* parametru = (char*) malloc((param.size() + 1) * sizeof(char));

	for (int i = 0; i < param.size(); ++i) {
		parametru[i] = param[i];
	}
	// adaugam manual caracterul terminal
	parametru[param.size()] = 0;

	return parametru;
}

std::pair <int, int> interogare_tabela_de_rutare(std::vector <std::unordered_map <int, std::pair <int, int>>> &tabela_de_rutare, __u32 &destinatie) {
	unsigned int masca = 0xffffffff;
	for (int i = 32; i > 0; --i) {
		auto gasit = tabela_de_rutare[i].find(destinatie & htonl(masca));
		if (gasit == tabela_de_rutare[i].end()) {
			// daca masca nu e buna o siftam la stanga pt a incerca alta masca
			masca <<= 1;
			continue;
		} else {
			// intoarce perechea cu next_hop si cu interfata
			return gasit -> second;
		}
	}
	// daca nu exista in tabela
	return std::pair <int, int> (0,0);
}

uint16_t ip_checksum(void* vdata, size_t length) {
	// Cast the data pointer to one that can be indexed.
	char* data = (char*)vdata;

	// Initialise the accumulator.
	uint64_t acc = 0xffff;

	// Handle any partial block at the start of the data.
	unsigned int offset = ((uintptr_t)data) &3;
	if (offset) {
		size_t count = 4 - offset;
		if (count > length) count = length;
		uint32_t word = 0;
		memcpy(offset + (char*) &word, data,count);
		acc += ntohl(word);
		data += count;
		length -= count;
	}

	// Handle any complete 32-bit blocks.
	char* data_end = data + (length&~3);
	while (data != data_end) {
		uint32_t word;
		memcpy(&word, data, 4);
		acc += ntohl(word);
		data += 4;
	}
	length &= 3;

	// Handle any partial block at the end of the data.
	if (length) {
		uint32_t word = 0;
		memcpy(&word, data, length);
		acc += ntohl(word);
	}

	// Handle deferred carries.
	acc = (acc & 0xffffffff) + (acc >> 32);
	while (acc >> 16) {
		acc = (acc & 0xffff) + (acc >> 16);
	}

	// If the data began at an odd byte address
	// then reverse the byte order to compensate.
	if (offset & 1) {
		acc = ((acc & 0xff00) >> 8) | ((acc & 0x00ff) << 8);
	}

	// Return the checksum in network byte order.
	return htons(~acc);
}
