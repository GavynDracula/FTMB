/*************************************************************************
	> File Name: master.c
	> Author: 
	> Mail: 
	> Created Time: Friday, December 01, 2017 PM04:02:43 HKT
 ************************************************************************/

#include "master.h"

int main() {
    int master_sockfd;
    int il_sockfd;
    struct sockaddr_in master_addr;
    struct sockaddr_in il_addr;
    socklen_t len = sizeof(il_addr);

    char command;

    // Creat a socket
    master_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    master_addr.sin_family = AF_INET;
    master_addr.sin_addr.s_addr = inet_addr(MASTER_IP);
    master_addr.sin_port = htons(MASTER_IL_PORT);

    // Bind
    bind(master_sockfd, (struct sockaddr *)&master_addr, sizeof(master_addr));
    // Listen
    listen(master_sockfd, 1);
    fprintf(stdout, "Master is waiting for InputLogger to connect...\n");
    // Accept a connection
    il_sockfd = accept(master_sockfd, (struct sockaddr *)&il_addr, &len);
    fprintf(stdout, "Receive connection request from InputLogger\n");

    while (1) {
        read(il_sockfd, &command, 1);
    }

    close(il_sockfd);

    return 0;
}
