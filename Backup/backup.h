/*************************************************************************
	> File Name: backup.h
	> Author: Dracula
	> Mail: dracula.guanyu.li@gmail.com
	> Created Time: Tuesday, December 12, 2017 PM01:19:28 HKT
 ************************************************************************/

#ifndef _BACKUP_H
#define _BACKUP_H

#define _GNU_SOURCE

#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sched.h>

#include "packet_counter.h"

#define BACKUP_IP "192.168.2.2"
#define BACKUP_IL_PORT 8082

#endif
