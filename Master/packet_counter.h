/*************************************************************************
	> File Name: packet_counter.h
	> Author: Dracula
	> Mail: dracula.guanyu.li@gmail.com
	> Created Time: Saturday, December 02, 2017 PM05:54:44 HKT
 ************************************************************************/

#ifndef _PACKET_COUNTER_H
#define _PACKET_COUNTER_H

#include<stdio.h>
#include<stdlib.h>
#include<pcap.h>
#include<netinet/in.h>
#include<netinet/if_ether.h>
#include<pthread.h>

#define SRC_NIC "eno3"
#define DST_NIC "eno3"

#define PRIVATE_PORT 8081

#define PACKET_MAX_SIZE 65535
#define PACKET_NUM -1
#define PROMISC_TRIGGER 0
#define OPTIMIZE_TRIGGER  1
#define TO_MS 5000

#define ETHER_HEADER_LENGTH 14

typedef struct packet_counter_arg {
    int* counter;
    pthread_mutex_t* mutex;
    int* nf_process_flag;
} packet_counter_arg;

typedef struct pcap_loop_arg {
    packet_counter_arg* pc_arg;
    pcap_t* dst_nic;
} pcap_loop_arg;
    
void* packet_counter(void* argv);
void get_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet);

#endif
