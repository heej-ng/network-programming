// 2020110007 박희중

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BUF_SIZE 1023
#define TTL 1

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int recv_sock;
    int send_sock;
    int n;
    int len;
    pid_t pid;
    unsigned int yes = 1;
    struct sockaddr_in mul_adr;
    struct ip_mreq join_adr;
    int time_live = TTL;
    char name[10];

    if(argc!=4){
        printf("Usage : %s <GroupIP> <PORT> <Name>",argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);
    //send_sock
    memset(&mul_adr, 0, sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    //mul_adr.sin_addr.s_addr = inet_addr(argv[1]);
    mul_adr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &mul_adr.sin_addr);

    //recv_sock //mul_adr==adr
    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (recv_sock < 0)
    {
        printf("error : Can't create receive socket\n");
        exit(0);
    }
    // multicast group in
    //join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_adr.imr_multiaddr = mul_adr.sin_addr;
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_adr, sizeof(join_adr));

    // 소켓 재사용 옵션 지정
    if (setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        printf("error : reuse setsockopt\n");
        exit(0);
    }

    if(bind(recv_sock,(struct sockaddr*)&mul_adr, sizeof(mul_adr))==-1)
        error_handling("bind() error");

    if((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("error : Can't create send socket\n");
        exit(0);
    }
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));
    

    if ((pid = fork()) < 0)
    {
        printf("error : fork\n");
        exit(0);
    }

    // child process : 채팅 메시지 수신 담당
    if (pid == 0)
    {
        char message[BUF_SIZE + 1];
        for (;;)
        {
            if ((n = recvfrom(recv_sock, message, BUF_SIZE, 0, NULL, 0)) < 0)
            {
                printf("error : recvfrom\n");
                exit(0);
            }

            message[n] = 0;
            printf("Receiced Message: %s", message); // Receiced Message
        }
        //close(recv_sock);
    }
    // parent process : 키보드 입력 및 메시지 송신 담당
    else
    {
        char message[BUF_SIZE + 1], line[BUF_SIZE + 1];
        while (fgets(message, BUF_SIZE, stdin) != NULL)
        {
            sprintf(line, "%s %s", name, message);
            int len = strlen(line);
            if (sendto(send_sock, line, strlen(line), 0, (struct sockaddr *)&mul_adr, sizeof(mul_adr)) < len)
            {
                printf("error : sendto\n");
                exit(0);
            }
        }
        //close(send_sock);
    }
    close(recv_sock);
    close(send_sock);
    return 0;
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}