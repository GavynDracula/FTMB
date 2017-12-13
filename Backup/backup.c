/*************************************************************************
	> File Name: backup.c
	> Author: Dracula
	> Mail: dracula.guanyu.li@gmail.com
	> Created Time: Tuesday, December 12, 2017 PM01:19:40 HKT
 ************************************************************************/

#include "backup.h"

int main(void) {
    int backup_sockfd;
    int il_sockfd;
    struct sockaddr_in backup_addr;
    struct sockaddr_in il_addr;
    socklen_t len = sizeof(il_addr);

    char data;

    cpu_set_t set;

    pthread_t nf_thread;
    packet_counter_arg pc_arg;
    pthread_mutex_t mutex;
    int counter;
    int nf_process_flag;

    /* Start Network Function Threand(Here a simple packet counter) */
    pc_arg.mutex = &mutex;
    pc_arg.counter = &counter;
    pc_arg.nf_process_flag = &nf_process_flag;
    nf_process_flag = 1;
    pthread_mutex_init(pc_arg.mutex, NULL);
    if (pthread_create(&nf_thread, NULL, &packet_counter, (void*)&pc_arg) != 0) {
        fprintf(stderr, "Error: FTMB-Backup: can't create "
                "network function(packet counter) thread!");
    }

    /* Multi thread cpu affinity */
    CPU_ZERO(&set);
    CPU_SET(6, &set);
    if(pthread_setaffinity_np(pthread_self(), sizeof(set), &set) != 0) {
        fprintf(stderr, "Error: FTMB-Backup: can't set backup's cpu-affinity\n");
        exit(1);
    }
    CPU_ZERO(&set);
    CPU_SET(8, &set);
    if(pthread_setaffinity_np(nf_thread, sizeof(set), &set) != 0) {
        fprintf(stderr, "Error: FTMB-Backup: can't set packet counter's cpu-affinity\n");
        exit(1);
    }

    /* Create a socket */
    backup_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    backup_addr.sin_family = AF_INET;
    backup_addr.sin_addr.s_addr = inet_addr(BACKUP_IP);
    backup_addr.sin_port = htons(BACKUP_IL_PORT);

    /* Bind */
    bind(backup_sockfd, (struct sockaddr *)&backup_addr, sizeof(backup_addr));
    /* Listen */
    listen(backup_sockfd, 1);
    fprintf(stdout, "FTMB-Backup: Backup is waiting for InputLogger to connect...\n");
    /* Accept a connection */
    il_sockfd = accept(backup_sockfd, (struct sockaddr *)&il_addr, &len);
    fprintf(stdout, "FTMB-Backup: Connection with InputLogger is established\n");

    /* Accept snapshot state */
    pthread_mutex_lock(pc_arg.mutex);
    read(il_sockfd, pc_arg.counter, 4);
    fprintf(stdout, 
            "FTMB-Backup: Receive the snapshot state(counter) %d from InputLogger\n", 
            *(pc_arg.counter));
    pthread_mutex_unlock(pc_arg.mutex);

    while (1) {
        read(il_sockfd, &data, 1);
        if (data == 's') {
            fprintf(stdout, "FTMB-Backup: Receive the "
                    "request of snapshot from InputLogger\n");
            while(*(pc_arg.nf_process_flag));
            *(pc_arg.nf_process_flag) = 1;
            pthread_mutex_lock(pc_arg.mutex);
            fprintf(stdout, 
                    "FTMB-Backup: Send the snapshot state(counter) %d to InputLogger\n", 
                    *(pc_arg.counter));
            write(il_sockfd, pc_arg.counter, 4);
            pthread_mutex_unlock(pc_arg.mutex);
        }
    }

    close(il_sockfd);

    return 0;
}
