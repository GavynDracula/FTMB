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

    return 0;
}
