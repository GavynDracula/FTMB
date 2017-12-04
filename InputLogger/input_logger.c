/*************************************************************************
	> File Name: input_logger.c
	> Author: Dracula
	> Mail: dracula.guanyu.li@gmail.com
	> Created Time: Thursday, November 30, 2017 PM12:04:08 HKT
 ************************************************************************/

#include "input_logger.h"

int replay_trigger = 0;

int main() {
    char err_buf[PCAP_ERRBUF_SIZE];
    int received_num;
    int received_total;
    int connect_status;
    char data;

    int il_sockfd;
    struct sockaddr_in il_addr;

    /*struct bpf_program filter;*/
    pcap_t* src_nic;
    pcap_t* dst_nic;
    pcap_dumper_t* pd;
    struct pcap_loop_arg arg;

    // Create a socket
    il_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    il_addr.sin_family = AF_INET;
    il_addr.sin_addr.s_addr = inet_addr(MASTER_IP);
    il_addr.sin_port = htons(MASTER_IL_PORT);

    // Connect to Master
    if (connect(il_sockfd, (struct sockaddr*)&il_addr, sizeof(il_addr)) == -1) {
        fprintf(stderr, "Error: FTMB-InputLogger: connect(): can't connect to master\n");
        exit(1);
    }

    src_nic = pcap_open_live(SRC_NIC, PACKET_MAX_SIZE, PROMISC_TRIGGER, TO_MS, err_buf);
    dst_nic = pcap_open_live(DST_NIC, PACKET_MAX_SIZE, PROMISC_TRIGGER, TO_MS, err_buf);

    // Open src_nic for capturing packet and open dst_nic for sending packet
    if (src_nic == NULL || dst_nic == NULL) {
        fprintf(stderr, "Error: FTMB-InputLogger: pcap_open_live(): %s\n", err_buf);
        exit(2);
    }
    else
        fprintf(stdout,
                "FTMB-InputLogger: Open src_nic %s and dst_nic %s successfully!\n",
                SRC_NIC, DST_NIC);

    /*
     *if (!pcap_compile(src_nic, &filter, "dst host 192.168.1.2", OPTIMIZE_TRIGGER, 0)) {
     *    fprintf(stderr, "Error: FTMB-InputLogger: pcap_compile():"
     *                    " complie filter error\n");
     *    exit(3);
     *}
     *if (!pcap_setfilter(src_nic, &filter)) {
     *    fprintf(stderr, "Error: FTMB-InputLogger: pcap_compile(): set filter error\n");
     *    exit(4);
     *}
     */
    
    received_total = 0;
    arg.dst_nic = dst_nic;

    while (!replay_trigger) {
        if((pd = pcap_dump_open(src_nic, TMP_PACKETS)) == NULL) {
            fprintf(stderr, "Error: FTMB-InputLogger: "
                    "pcap_dump_open(): open file %s failed\n", IN_PACKETS);
            exit(5);
        }
        else
        fprintf(stdout, "FTMB-InputLogger: Connect to "
                "stable storage(pcap now) successfully!\n");
        arg.pd = pd;
        
        received_num = pcap_dispatch(src_nic, PACKET_NUM, get_packet, (u_char*)&arg);
        pcap_dump_close(pd);

        fprintf(stdout, "Debug: InputLogger: Receive %d packets\n", received_num);
        received_total += received_num;
        fprintf(stdout, "Debug: Receive %d packets totally\n", received_total);
        
        if (received_num > 0) {
            data = 's'; // Tell Master to take a 's'napshot
            connect_status = write(il_sockfd, &data, 1);
            fprintf(stdout, "FTMB-InputLogger: Send the request of snapshot to Master\n");
            connect_status = read(il_sockfd, &data, 1);
            
            // Follow if-case is just incomplete, not tested
            if (connect_status <= 0) {
                fprintf(stderr, "Error: FTMB-InputLogger: "
                        "Master is disconnected, now start to recovery\n");
                // Create a socket
                il_sockfd = socket(AF_INET, SOCK_STREAM, 0);
                il_addr.sin_family = AF_INET;
                il_addr.sin_addr.s_addr = inet_addr(BACKUP_IP);
                il_addr.sin_port = htons(BACKUP_IL_PORT);
                
                // Connect to Backup
                if (connect(il_sockfd,(struct sockaddr*)&il_addr,sizeof(il_addr)) == -1) {
                    fprintf(stderr, "Error: FTMB-InputLogger: "
                            "connect(): can't connect to Backup\n");
                    exit(1);
                }
                // Start to replay 
                replay(IN_PACKETS);
                replay(TMP_PACKETS);
                
                data = 's'; // Tell Backup to take a 's'napshot
                connect_status = write(il_sockfd, &data, 1);
                fprintf(stdout, "FTMB-InputLogger: Send the "
                        "request of snapshot to Backup\n");
                connect_status = read(il_sockfd, &data, 1);
                fprintf(stdout, "FTMB-InputLogger: Receive the reply from Backup\n");
                if (data == 't') {
                    // Backup's snapshot is 't'aken
                    fprintf(stdout, "FTMB-InputLogger: Backup's snapshot is taken\n");
                    rename(TMP_PACKETS, IN_PACKETS);
                }
                dst_nic = pcap_open_live(BAC_NIC, PACKET_MAX_SIZE, 
                                         PROMISC_TRIGGER, TO_MS, err_buf);
                if (dst_nic == NULL) {
                    fprintf(stderr, "Error: FTMB-InputLogger: "
                            "pcap_open_live(): %s\n", err_buf);
                    exit(2);
                }
                continue;
            }
            
            fprintf(stdout, "FTMB-InputLogger: Receive the reply from Master\n");
            if (data == 't') {
                // Master's snapshot is 't'aken
                fprintf(stdout, "FTMB-InputLogger: Master's snapshot is taken\n");
                rename(TMP_PACKETS, IN_PACKETS);
            }
        }
    }

    pcap_close(src_nic);
    pcap_close(dst_nic);

    return 0;
}

void get_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    pcap_dumper_t* pd = ((struct pcap_loop_arg*)arg)->pd;
    pcap_t* dst_nic = ((struct pcap_loop_arg*)arg)->dst_nic;

    // Log the packet
    pcap_dump((u_char *)pd, pkthdr, packet);

    // Send the packet to Master
    if (pcap_sendpacket(dst_nic, packet, pkthdr->caplen) != 0) {
        fprintf(stderr, "Error: FTMB-InputLogger: "
                "pcap_sendpacket(): send packet error\n");
    }

    /*print_packet(pkthdr, packet);*/
}

void print_packet(const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    printf("Packet length: %d\n", pkthdr->len);
    printf("Number of bytes: %d\n", pkthdr->caplen);
    printf("Received time: %s\n", ctime((const time_t*)&pkthdr->ts.tv_sec));

    for (int i = 0; i < pkthdr->len; i++) {
        printf(" %02x", packet[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    printf("\n\n");
}

void replay_packet(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
    pcap_t* bac_nic = (pcap_t*)arg;

    // Send the packet to Backup
    if (pcap_sendpacket(bac_nic, packet, pkthdr->caplen) != 0) {
        fprintf(stderr, "Error: FTMB-InputLogger: "
                "pcap_sendpacket(): send packet error\n");
    }
}

void replay(char* packets) {
    char err_buf[PCAP_ERRBUF_SIZE];
    pcap_t* bac_nic;
    pcap_t* in_packets;

    // Open Backup NIC to send packets to Backup
    bac_nic = pcap_open_live(BAC_NIC, PACKET_MAX_SIZE, PROMISC_TRIGGER, TO_MS, err_buf);
    // Open logged in_packets for replay
    in_packets = pcap_open_offline(packets, err_buf);

    if (bac_nic == NULL)  {
        fprintf(stderr, "Error: FTMB-InputLogger: pcap_open_live(): %s\n", err_buf);
        exit(6);
    }
    else
        fprintf(stdout, "FTMB-InputLogger: Open bac_nic %s successfully!\n", BAC_NIC);

    if (in_packets == NULL) {
        fprintf(stderr, "Error: FTMB-InputLogger: pcap_open_offline(): %s\n", err_buf);
        exit(7);
    }
    else
        fprintf(stdout, "FTMB-InputLogger: Open in_packets %s successfully!\n", packets);

    // Replay packets to Backup
    pcap_loop(in_packets, -1, replay_packet, (u_char*)bac_nic);

    pcap_close(in_packets);
    pcap_close(bac_nic);
}
