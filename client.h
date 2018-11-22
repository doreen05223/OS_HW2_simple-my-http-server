#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

void saveContent(char *ffile, char *address, char *rootaddress);

#endif
