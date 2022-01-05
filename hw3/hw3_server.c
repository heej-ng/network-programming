#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define STR_SIZE 10000
#define SEQ_START 1000

typedef struct{
    int seq;
    int ack;
    int bu_len;
    char buf[BUF_SIZE];
}Packet;

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;

    FILE *fp;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[STR_SIZE];
    char getmessage[STR_SIZE];
    char fname[STR_SIZE];
    int str_len, i;
	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
		error_handling("bind() error"); 
	
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	clnt_addr_size=sizeof(clnt_addr);  

    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
	if(clnt_sock==-1)
		error_handling("accept() error"); 
    else{
        printf("---------------------------------\n");
        printf("    File Transmission Server\n");
        printf("---------------------------------\n");
    }
    //if((str_len=read(clnt_sock, fname, sizeof(fname)))!=0){
      //  error_handling("file read error");
    //}
    read(clnt_sock, fname, sizeof(fname));

    Packet Ser;
    Ser.seq = SEQ_START;
    char *fnotexist = "nf";
    char *fexist = "ef";

    fp = fopen(fname, "r");

    if(fp == NULL){
        printf("%s File Not Found\n", fname);
        return 0;
    }
    

    while(feof(fp) == 0){
        memset(message, 0, sizeof(message));
        memset(getmessage, 0, sizeof(getmessage));
        fread(message, sizeof(char), 100, fp);
        int len = strlen(message);
        printf("[Server] Tx: SEQ: %d,  %d byte data\n", Ser.seq, len);
        write(clnt_sock, message, strlen(message));
        Ser.bu_len += len;

        //printf("%s\n", message);

        read(clnt_sock, getmessage, sizeof(getmessage));

        if(strcmp(getmessage, "continue")==0){
            Ser.ack = Ser.seq + 100 + 1;
            printf("[Server] Rx ACK: %d\n\n", Ser.ack);
            Ser.seq = Ser.ack;
        }
        else{
            printf("%s sent (%d Bytes)\n", fname, Ser.bu_len);
        }

    }

    close(clnt_sock);

    
    close(serv_sock);
    fclose(fp);
    return 0;
	 

}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
