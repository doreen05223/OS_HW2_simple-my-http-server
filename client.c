#include "client.h"

int main(int argc, char *argv[])
{
    int sock = 0;
    long valread;
    struct sockaddr_in serv_addr;
    char *ffile = argv[2];
    char *Localhost = argv[4];
    char *port = argv[6];
    char hello[0xfff];
    char *requestline = "GET ";
    strcpy(hello,requestline);
    strcat(hello,ffile);
    strcat(hello," HTTP/1.x\r\nHost: ");
    strcat(hello,Localhost);
    strcat(hello,":");
    strcat(hello,port);
    strcat(hello,"\r\n\r\n");
    //int PORT = atoi(argv[6]);
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock, hello, strlen(hello), 0 );

    int ii;
    char *addr=NULL;
    char *address=NULL;
    char *rootaddress=NULL;
    char *testaddress=NULL;
    char out[1000]= {0};
    char test[1000]= {0};
    rootaddress = getcwd(NULL,0);
    mkdir("output",0777);
    strcpy(out,rootaddress);
    strcat(out,"/output");
    ii = chdir(out);
    address = getcwd(NULL,0);
    mkdir("secfolder",0777);
    strcat(out,"/secfolder");
    ii = chdir(out);
    addr = getcwd(NULL,0);
    mkdir("trifolder",0777);
    strcpy(test,rootaddress);
    strcat(test,"/testdir");
    ii=chdir(test);
    testaddress = getcwd(NULL,0);

    valread = read( sock, buffer, 1024);
    printf("%s\n",buffer );

    for(int i=0; i<strlen(ffile); i++) {
        ffile[i]=ffile[i+1];
    }

    saveContent(ffile,testaddress,address);
    ii=chdir(rootaddress);
    addr=getcwd(NULL,0);
    return 0;
}

void saveContent(char *ffile, char *testaddress, char *outaddress)
{
    FILE *file;
    struct stat buf;
    int result, ii;
    char filename[100][100];
    struct dirent *dent;
    DIR *dir;
    char add[1000]= {0};
    char *addr=NULL;
    char location[1000]= {0};
    FILE *infile;
    char *buffer;
    long numbytes;

    for(int i=0; i<30; i++) {
        for(int j=0; j<30; j++) {
            filename[i][j]='0';
        }
    }

    dir = opendir(testaddress);
    while((dent=readdir(dir))!=NULL) {
        result = stat(dent->d_name, &buf);
        if(result==0) {
            if(strcmp(dent->d_name,".")!=0 && strcmp(dent->d_name,"..")!=0 && dent->d_type==4) {
                strcpy(filename[0], dent->d_name);
            }
        }
        if(strcmp(dent->d_name,ffile)==0) {
            if(dent->d_type==8) {
                infile = fopen(ffile, "r");
                fseek(infile, 0L, SEEK_END);
                numbytes = ftell(infile);
                fseek(infile, 0L, SEEK_SET);
                buffer = (char*)calloc(numbytes, sizeof(char));
                fread(buffer, sizeof(char), numbytes, infile);
                fclose(infile);
                ii = chdir(outaddress);
                addr = getcwd(NULL,0);
                file = fopen(ffile,"w");
                fprintf(file,"%s",buffer);
                fclose(file);
                free(buffer);
                return;
            }
        }
    }
    if(filename[0][0]!='0') {
        strcpy(add,testaddress);
        strcat(add,"/");
        strcat(add,filename[0]);
        strcpy(location,outaddress);
        if(strcmp(filename[0],"testdir")!=0) {
            strcat(location,"/");
            strcat(location,filename[0]);
        }
        ii = chdir(add);
        addr=getcwd(NULL,0);
        saveContent(ffile, addr, location);
    } else return;
}
