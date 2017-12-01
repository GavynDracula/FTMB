/*************************************************************************
	> File Name: input_logger.c
	> Author: 
	> Mail: 
	> Created Time: Thursday, November 30, 2017 PM12:04:08 HKT
 ************************************************************************/

#include "input_logger.h"

int main() {
    char err_buf[PCAP_ERRBUF_SIZE];

    int il_sockfd;
    struct sockaddr_in il_addr;

    /*struct bpf_program filter;*/
    pcap_t *src_nic;
    pcap_t *dst_nic;
    pcap_dumper_t *pd;
    struct pcap_loop_arg arg;

    // Create a socket
    il_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    il_addr.sin_family = AF_INET;
    il_addr.sin_addr.s_addr = inet_addr(MASTER_IP);
    il_addr.sin_port = htons(MASTER_IL_PORT);

    if (connect(il_sockfd, (struct sockaddr *)&il_addr, sizeof(il_addr)) == -1) {
        fprintf(stderr, "Error: connect(): can't connect to master\n");
        exit(1);
    }

    src_nic = pcap_open_live(SRC_NIC, PACKET_MAX_SIZE, PROMISC_TRIGGER, TO_MS, err_buf);
    dst_nic = pcap_open_live(DST_NIC, PACKET_MAX_SIZE, PROMISC_TRIGGER, TO_MS, err_buf);

    // Open src_nic for capturing packet and open dst_nic for sending packet
    if (src_nic == NULL || dst_nic == NULL) {
        fprintf(stderr, "Error: pcap_open_live(): %s\n", err_buf);
        exit(2);
    }
    else
        fprintf(stdout, 
                "Open src_nic %s and dst_nic %s successfully!\n", 
                SRC_NIC, DST_NIC);

    /*
     *if (!pcap_compile(src_nic, &filter, "dst host 192.168.1.2", OPTIMIZE_TRIGGER, 0)) {
     *    fprintf(stderr, "Error: pcap_compile(): complie filter error\n");
     *    exit(3);
     *}
     *if (!pcap_setfilter(src_nic, &filter)) {
     *    fprintf(stderr, "Error: pcap_compile(): set filter error\n");
     *    exit(4);
     *}
     */
    
    if((pd = pcap_dump_open(src_nic, IN_PACKETS)) == NULL) {
        fprintf(stderr, "Error: pcap_dump_open(): open file %s failed\n", IN_PACKETS);
        exit(5);
    }
    else
        fprintf(stdout, "Connect to stable storage successfully!\n");

    arg.dst_nic = dst_nic;
    arg.pd = pd;
    pcap_loop(src_nic, PACKET_NUM, get_packet, (u_char*)&arg);

    pcap_dump_close(pd);
    pcap_close(src_nic);
    pcap_close(dst_nic);

    return 0;
}

void get_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    pcap_dumper_t *pd = ((struct pcap_loop_arg *)arg)->pd;
    pcap_t *dst_nic = ((struct pcap_loop_arg *)arg)->dst_nic;

    pcap_dump((u_char *)pd, pkthdr, packet);

    if (pcap_sendpacket(dst_nic, packet, pkthdr->caplen) != 0) {
        fprintf(stderr, "pcap_sendpacket(): send packet error\n");
    }

    print_packet(pkthdr, packet);
}

void print_packet(const struct pcap_pkthdr* pkthdr, const u_char* packet) {
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
