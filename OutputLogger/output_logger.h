/*************************************************************************
	> File Name: output_logger.h
	> Author: Dracula
	> Mail: dracula.guanyu.li@gmail.com
	> Created Time: Tuesday, December 05, 2017 PM05:59:50 HKT
 ************************************************************************/

#ifndef _OUTPUT_LOGGER_H
#define _OUTPUT_LOGGER_H

#include<stdio.h>
#include<stdlib.h>
#include<pcap.h>

// Source NIC should be set as promisc as follow
// $ sudo ifconfig eno3 promisc
#define SRC_NIC "eno3"
#define DST_NIC "eno4"
#define PACKET_MAX_SIZE 65535
#define PACKET_NUM -1
#define PROMISC_TRIGGER 0
#define OPTIMIZE_TRIGGER  1
#define TO_MS -1

void get_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet);

#endif
