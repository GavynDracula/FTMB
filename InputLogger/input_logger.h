/*************************************************************************
	> File Name: input_logger.h
	> Author: 
	> Mail: 
	> Created Time: Friday, December 01, 2017 PM02:42:46 HKT
 ************************************************************************/

#ifndef _INPUT_LOGGER_H
#define _INPUT_LOGGER_H

#include<stdio.h>
#include<stdlib.h>
#include<pcap.h>
#include<time.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MASTER_IP "192.168.1.2"
#define MASTER_IL_PORT 8081

#define SRC_NIC "eno4"
#define DST_NIC "eno3"
#define IN_PACKETS "/home/dracula/FTMB/StableStorage/packets.pcap"
#define PACKET_MAX_SIZE 65535
#define PACKET_NUM 6
#define PROMISC_TRIGGER 0
#define OPTIMIZE_TRIGGER  1
#define TO_MS 1000

struct pcap_loop_arg {
    pcap_t *dst_nic;
    pcap_dumper_t *pd;
};

void get_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet);
void print_packet(const struct pcap_pkthdr* pkthdr, const u_char* packet);

#endif
