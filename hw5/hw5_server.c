// 2020110007 박희중

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define BUF_SIZE 50
void error_handling(char *message);

typedef struct
{
    int pro_lev;
    int opt_int;
    char opt_name[20];
    int optval;
    int result;
} SO_PACKET;

int main(int argc, char *argv[])
{
    //udp
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t clnt_adr_sz;
    struct sockaddr_in serv_adr, clnt_adr;

    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock=socket(PF_INET,SOCK_DGRAM,0);

    if(serv_sock==-1)
        error_handling("UDP socket creation error");
    
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");

    //tcp
    int tcp_sock;
    int sock_type;
    socklen_t optlen;
    int state;

    optlen=sizeof(sock_type);
    tcp_sock=socket(PF_INET, SOCK_STREAM, 0);


    while(1)
    {
        SO_PACKET packet;
        clnt_adr_sz=sizeof(clnt_adr);
        str_len=recvfrom(serv_sock, &packet, sizeof(packet), 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        printf(">>> Received Socket option: %s\n",packet.opt_name);

        //tcp option
        state=getsockopt(tcp_sock, packet.pro_lev, packet.opt_int, (void*)&sock_type, &optlen);
        
        packet.optval=sock_type;
        packet.result=state;


        sendto(serv_sock, &packet, sizeof(packet), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
        printf("<<< Send option: %s: %d, result: %d\n\n", packet.opt_name, packet.optval, packet.result);
    }
    
}
void error_handling(char *message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}