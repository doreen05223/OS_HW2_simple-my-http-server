#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#define PORT 8080
#define MAX_QUEUE 50

char* printContent(char *findfile);
char *checkType(char *findfile, char *filetype, int det, char *address, char *rootaddress);
char *checkFile(char *files, char *filetype, char *address, char *rootaddress);

#endif
