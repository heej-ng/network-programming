#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define STR_SIZE 1000000
#define SEQ_START 1000

typedef struct{
    int seq;
    int ack;
    int bu_len;
    char buf[BUF_SIZE];
}Packet;

void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[STR_SIZE];
    char fexist[STR_SIZE];
	int str_len;
    char fname[STR_SIZE];
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
		
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");
    else
        puts("Connected..\n");

    printf("Input file name : ");
    scanf("%s", fname);
    printf("[Client] request %s\n\n", fname);

    write(sock, fname, strlen(fname)+1);

    /*read(sock, fexist, sizeof(fexist));
    if(strcmp(fexist, "nf")==0){
        printf("File Not Found");
        close(sock);
        return 0;
    }*/

    int recv_len = 0;
    int recv_cnt = 0;
    int k = 0;

    Packet clnt;
    clnt.seq = SEQ_START;

    while(1){

        recv_cnt=read(sock, &message[recv_len], BUF_SIZE);
        if(recv_cnt==-1){
            error_handling("read() error!");
        }
        //printf("%s\n", message);
        recv_len += recv_cnt;

        printf("[Client] Rx SEQ: %d, len : %d bytes\n", clnt.seq, recv_cnt);
        

        if(recv_cnt==BUF_SIZE){
            write(sock, "continue", sizeof("continue"));
            clnt.ack = clnt.seq + 100 + 1;
            printf("[Client] Tx ACK: %d\n\n", clnt.ack);
            clnt.seq = clnt.ack;
        }
        else{
            write(sock, "done", sizeof("done"));
            printf("%s recevied (%d Bytes)\n", fname, recv_len);
            break;
        }
    }

    message[recv_len]=0;
    //printf("Last Message : %s\n", message);

    FILE *fp = fopen(fname, "w");
    fwrite(message, strlen(message), 1, fp);
    
	close(sock);
    fclose(fp);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
