/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: Thursday, November 30, 2017 PM12:04:08 HKT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<pcap.h>
#include<time.h>

void get_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    pcap_dump(arg, pkthdr, packet);

    printf("Packet length: %d\n", pkthdr->len);
    printf("Number of bytes: %d\n", pkthdr->caplen);
    printf("Recieved time: %s\n", ctime((const time_t*)&pkthdr->ts.tv_sec));

    for (int i = 0; i < pkthdr->len; i++) {
        printf(" %02x", packet[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    printf("\n\n");
}

int main() {
    char* nic_str = "eno4";
    char* filename = "/home/dracula/ftmb/stable_storage/packets.record";
    int pkt_max = 65535;
    int pkt_num = 6;
    int promisc = 0;
    int optimize = 1;
    int to_ms = 1000;
    char err_buf[PCAP_ERRBUF_SIZE];

    //struct bpf_program filter;
    pcap_t *nic;
    pcap_dumper_t *pd;

    nic = pcap_open_live(nic_str, pkt_max, promisc, to_ms, err_buf);

    if (nic == NULL) {
        fprintf(stderr, "Error: pcap_open_live(): %s\n", err_buf);
        exit(1);
    }

    /*
     *if (!pcap_compile(nic, &filter, "dst host 192.168.1.2", optimize, 0)) {
     *    fprintf(stderr, "Error: pcap_compile(): complie filter error\n");
     *    exit(2);
     *}
     *if (!pcap_setfilter(nic, &filter)) {
     *    fprintf(stderr, "Error: pcap_compile(): set filter error\n");
     *    exit(3);
     *}
     */
    
    if((pd = pcap_dump_open(nic, filename)) == NULL) {
        fprintf(stderr, "Error: pcap_dump_open(): open file %s failed\n", filename);
        exit(4);
    }

    pcap_loop(nic, pkt_num, get_packet, (u_char*)pd);

    pcap_dump_close(pd);
    pcap_close(nic);

    return 0;
}
