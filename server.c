#include "server.h"
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
#define PORT 8080
#define MAX_QUEUE 50

char* printContent(char *findfile);
char *checkFile(char *files, char *filetype, char *address, char *rootaddress);
char hello[0xfff];
char Queue[MAX_QUEUE][500];
int front = -1, rear = -1;
bool flag = 0;

int isFull()
{
    return (rear % MAX_QUEUE == front);
}

int isEmpty()
{
    return front == rear;
}

void Add(char** Queue, char *item)
{
    if(isFull() && flag == 1 || rear == MAX_QUEUE - 1 && front == -1) {
//		printf("Circular Queue is full!\n");
        return;
    }
//	printf("Circular Queue add: %s\n", item);
    rear = (rear + 1) % MAX_QUEUE;
    Queue[rear] = item;
//	printf("%d\n",rear);
    if (front == rear) flag = 1;
}

void Delete(char** Queue)
{
    if (isEmpty() && flag == 0) {
//		printf("Circular Queue is empty!\n");
        return;
    }
    front = (front + 1) % MAX_QUEUE;
//	printf("%s is deleted.\n", Queue[front]);
    if (front == rear) flag = 0;
}

char *getQueue(char** Queue)
{
    if (isEmpty() && flag==0) {
//                printf("Queue is empty!\n");
        return;
    }
    for (int i = 0; i < MAX_QUEUE; i++) {
        return Queue[i];
    }
}

int main(int argc, char *argv[])
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[30000] = {0};
        valread = read( new_socket, buffer, 30000);

        pthread_t t;
        Add(Queue,buffer);
        printf("%s\n",buffer);
        pthread_create(&t,NULL,getQueue,Queue);
        bool lock =0;
        char request[0xfff];
        while(1) {
            if(!isEmpty()) {
                lock=1;
                strcpy(request,getQueue(Queue));
                Delete(Queue);
                lock=0;
                printContent(request);
                write(new_socket, hello, strlen(hello));
            } else break;
        }
        pthread_join(t,NULL);
        close(new_socket);
    }
    return 0;
}

char* printContent(char *files)
{
    //get request file or directory
    char findfile[4][128]= {0};
    int i, j=0, ctr=0;
    for(i=0; i<(strlen(files)); i++) {
        if(files[i]==' ') {
            ctr++;
            j=0;
        } else {
            findfile[ctr][j]=files[i];
            j++;
        }
    }

    char *hell;
    if(findfile[1][0]!='/') {
        hell = "HTTP/1.x 400 BAD_REQUEST\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n";
        strcpy(hello,hell);
        return hello;
    } else if(strcmp(findfile[0],"GET")!=0) {
        hell = "HTTP/1.x 405 METHOD_NOT_ALLOW\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n";
        strcpy(hello,hell);
        return hello;
    }

    struct stat buf;
    int result;
    char filetype[2][10]= {0};
    int cctr=0, jj=0;

    //delete '/'
    for(int i=0; i<strlen(findfile[1]); i++) {
        findfile[1][i]=findfile[1][i+1];
    }

    char *address=NULL;
    address = getcwd(NULL,0);
//    printf("%s\n",address);
    result = stat( findfile[1], &buf );
    //if(__S_IFDIR & buf.st_mode) {
    if(result==0) {
        checkFile(findfile[1],filetype[1],address,address);
    } else { /* if(!(__S_IFDIR & buf.st_mode)){*/
        //get file type
        for(i=0; i<(strlen(findfile[1])); i++) {
            if(findfile[1][i]=='.') {
                cctr++;
                jj=0;
            } else {
                filetype[cctr][jj]=findfile[1][i];
                jj++;
            }
        }
        printf("filetype: %s\n",filetype[1]);
        if(strcmp(filetype[1],"htm")!=0 && strcmp(filetype[1],"html")!=0 && strcmp(filetype[1],"css")!=0 && strcmp(filetype[1],"h")!=0 && strcmp(filetype[1],"hh")!=0 && strcmp(filetype[1],"c")!=0 && strcmp(filetype[1],"cc")!=0 && strcmp(filetype[1],"json")!=0) {
            hell = "HTTP/1.x 415 UNSUPPORT_MEDIA_TYPE\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n";
            strcpy(hello,hell);
            return hello;
        } else checkFile(findfile[1],filetype[1],address,address);
    }

}

char *checkFile(char *findfile, char *filetype, char *address, char *rootaddress)
{
    FILE *infile;
    char *buffer;
    long numbytes;
    struct stat buf;
    int result;
    struct dirent *dent;
    char *hell;
    char filename[30][30];
    char add[1000]= {0};
    char *addr=NULL;
    DIR *dir;

    for(int i=0; i<30; i++) {
        for(int j=0; j<30; j++) {
            filename[i][j]='0';
        }
    }

    dir = opendir(address);
    while((dent=readdir(dir))!=NULL) {
        result = stat(dent->d_name, &buf);
        if(result==0) {
            if(strcmp(dent->d_name,".")!=0 && strcmp(dent->d_name,"..")!=0 && dent->d_type==4) {
                printf("file: %s\n",dent->d_name);
                strcpy(filename[0], dent->d_name);
            }
        }

        if(strcmp(dent->d_name,findfile)==0) {
            if(dent->d_type==4) {
                //if(__S_IFDIR & buf.st_mode) {
                dir = opendir(findfile);
                hell = "HTTP/1.x 200 OK\r\nContent-Type: directory\r\nServer: httpserver/1.x\r\n\r\n";
                strcpy(hello,hell);
                while((dent=readdir(dir))!=NULL) {
                    if(strcmp(dent->d_name,".")!=0 && strcmp(dent->d_name,"..")!=0) {
                        strcat(hello,dent->d_name);
                        strcat(hello," ");
                    }
                }
                int ii = chdir(rootaddress);
                addr=getcwd(NULL,0);
                return hello;
            } else if(dent->d_type==8) {
                infile = fopen(findfile, "r");
                // Get the number of bytes
                fseek(infile, 0L, SEEK_END);
                numbytes = ftell(infile);
                // reset the file position indicator to the beginning of the file
                fseek(infile, 0L, SEEK_SET);
                // grab sufficient memory for the buffer to hold the text
                buffer = (char*)calloc(numbytes, sizeof(char));
                // copy all the text into the buffer
                fread(buffer, sizeof(char), numbytes, infile);
                fclose(infile);

                if(strcmp(filetype,"htm")==0 || strcmp(filetype,"html")==0) {
                    hell = "HTTP/1.x 200 OK\r\nContent-Type: text/html\r\nServer: httpserver/1.x\r\n\r\n";
                } else if(strcmp(filetype,"css")==0) {
                    hell = "HTTP/1.x 200 OK\r\nContent-Type: text/css\r\nServer: httpserver/1.x\r\n\r\n";
                } else if(strcmp(filetype,"h")==0 || strcmp(filetype,"hh")==0) {
                    hell = "HTTP/1.x 200 OK\r\nContent-Type: text/x-h\r\nServer: httpserver/1.x\r\n\r\n";
                } else if(strcmp(filetype,"c")==0 || strcmp(filetype,"cc")==0) {
                    hell = "HTTP/1.x 200 OK\r\nContent-Type: text/x-c\r\nServer: httpserver/1.x\r\n\r\n";
                } else if(strcmp(filetype,"json")==0) {
                    hell = "HTTP/1.x 200 OK\r\nContent-Type: application/json\r\nServer: httpserver/1.x\r\n\r\n";
                }
                strcpy(hello,hell);
                strcat(hello,buffer);
                free(buffer);
                int ii = chdir(rootaddress);
                addr=getcwd(NULL,0);
                return hello;
            }
        }

    }
    if(filename[0][0]!='0') {
        strcpy(add,address);
        strcat(add,"/");
        strcat(add,filename[0]);
        int ii = chdir(add);
        addr=getcwd(NULL,0);
//    		printf("%s\n",addr);
        checkFile(findfile,filetype,addr,rootaddress);
    } else {
        hell = "HTTP/1.x 404 NOT_FOUND\r\nContent-Type: \r\nServer: httpserver/1.x\r\n\r\n";
        strcpy(hello,hell);
        int ii = chdir(rootaddress);
        addr=getcwd(NULL,0);
        return hello;
    }
}
