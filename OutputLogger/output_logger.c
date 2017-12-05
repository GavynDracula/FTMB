/*************************************************************************
	> File Name: output_logger.c
	> Author: Dracula
	> Mail: dracula.guanyu.li@gmail.com
	> Created Time: Tuesday, December 05, 2017 PM05:59:45 HKT
 ************************************************************************/

#include "output_logger.h"

int main(void) {
    char err_buf[PCAP_ERRBUF_SIZE];

    struct bpf_program filter;
    pcap_t* src_nic;
    pcap_t* dst_nic;

    src_nic = pcap_open_live(SRC_NIC, PACKET_MAX_SIZE, PROMISC_TRIGGER, TO_MS, err_buf);
    dst_nic = pcap_open_live(DST_NIC, PACKET_MAX_SIZE, PROMISC_TRIGGER, TO_MS, err_buf);

    /* Open src_nic for capturing packet and open dst_nic for sending packet */
    if (src_nic == NULL || dst_nic == NULL) {
        fprintf(stderr, "Error: FTMB-OutputLogger: pcap_open_live(): %s\n", err_buf);
        exit(2);
    }
    else
        fprintf(stdout,
                "FTMB-OutputLogger: Open src_nic %s and dst_nic %s successfully!\n",
                SRC_NIC, DST_NIC);

    if (pcap_compile(src_nic, &filter, "dst host 192.168.2.2", OPTIMIZE_TRIGGER, 0)) {
       fprintf(stderr, "Error: FTMB-OutputLogger: pcap_compile():"
                       " complie filter error\n");
       exit(3);
    }
    if (pcap_setfilter(src_nic, &filter)) {
       fprintf(stderr, "Error: FTMB-OutputLogger: pcap_compile(): set filter error\n");
       exit(4);
    }
    
    pcap_loop(src_nic, PACKET_NUM, get_packet, (u_char*)dst_nic);

    pcap_close(src_nic);
    pcap_close(dst_nic);

    return 0;
}

void get_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    pcap_t* dst_nic = (pcap_t*)arg;

    /* Send the packet to Master */
    if (pcap_sendpacket(dst_nic, packet, pkthdr->caplen) != 0) {
       fprintf(stderr, "Error: Packet Counter: pcap_sendpacket(): send packet error\n");
    }
}
