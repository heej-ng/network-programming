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
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz;
    struct sockaddr_in serv_adr, from_adr;
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    int flag = 0;

    while (1)
    {
        SO_PACKET packet;
        int num;
        // start print
        if (flag == 0)
        {
            printf("---------------------------\n");
            printf("1: SO_SNDBUF\n");
            printf("2: SO_RCVBUF\n");
            printf("3: SO_REUSEADDR\n");
            printf("4: SO_KEEPALIVE\n");
            printf("5: SO_BROADCAST\n");
            printf("6: IP_TOS\n");
            printf("7: IP_TTL\n");
            printf("8: TCP_NODELAY\n");
            printf("9: TCP_MAXSEG\n");
            printf("10: Quit\n");
            printf("---------------------------\n");
        }
        fputs("Input option number: ", stdout);
        scanf("%d", &num);
        if (num < 1 || num > 10)
        {
            printf("Wrong number. type again!\n");
            flag = 1;
            continue;
        }
        flag = 0;
        if (num == 10)
        {
            printf("Client quit.\n");
            break;
        }
        switch (num)
        {
        case 1:
            packet.opt_int = SO_SNDBUF;
            packet.pro_lev = SOL_SOCKET;
            strcpy(packet.opt_name, "SO_SNDBUF");
            break;
        case 2:
            packet.opt_int = SO_RCVBUF;
            packet.pro_lev = SOL_SOCKET;
            strcpy(packet.opt_name, "SO_RCVBUF");
            break;
        case 3:
            packet.opt_int = SO_REUSEADDR;
            packet.pro_lev = SOL_SOCKET;
            strcpy(packet.opt_name, "SO_REUSEADDR");
            break;
        case 4:
            packet.opt_int = SO_KEEPALIVE;
            packet.pro_lev = SOL_SOCKET;
            strcpy(packet.opt_name, "SO_KEEPALIVE");
            break;
        case 5:
            packet.opt_int = SO_BROADCAST;
            packet.pro_lev = SOL_SOCKET;
            strcpy(packet.opt_name, "SO_BROADCAST");
            break;
        case 6:
            packet.opt_int = IP_TOS;
            packet.pro_lev = IPPROTO_IP;
            strcpy(packet.opt_name, "IP_TOS");
            break;
        case 7:
            packet.opt_int = IP_TTL;
            packet.pro_lev = IPPROTO_IP;
            strcpy(packet.opt_name, "IP_TTL");
            break;
        case 8:
            packet.opt_int = TCP_NODELAY;
            packet.pro_lev = IPPROTO_TCP;
            strcpy(packet.opt_name, "TCP_NODELAY");
            break;
        case 9:
            packet.opt_int = TCP_MAXSEG;
            packet.pro_lev = IPPROTO_TCP;
            strcpy(packet.opt_name, "TCP_MAXSEG");
            break;
        }
        sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
        adr_sz = sizeof(from_adr);
        str_len = recvfrom(sock, &packet, sizeof(packet), 0, (struct sockaddr *)&from_adr, &adr_sz);

        printf(">>> Server result: %s: value: %d, result: %d\n\n", packet.opt_name, packet.optval, packet.result);
    }
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}