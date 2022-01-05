//2020110007 박희중
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLNT 256

typedef struct{
	int command;
	int seatno; 
	int seats[20]; 
	int result; 
}BUS_INFO;

BUS_INFO bus;
int seat_save[20]={0,};

void * handle_clnt(void * arg);
void send_info(int clnt_num);
void error_handling(char* msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	memset(&bus, 0, sizeof(BUS_INFO));

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP: %s, clnt_sock = %d\n", inet_ntoa(clnt_adr.sin_addr), clnt_sock);
	}
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock = *((int*)arg);
	int str_len=0, i;
	
	while(read(clnt_sock, (void*)&bus, sizeof(BUS_INFO))!=0)
		send_info(clnt_sock);
	
    pthread_mutex_lock(&mutx);
    for(i=0; i < clnt_cnt; i++) {
        //printf("clnt_sock: %d, clnt_cnt: %d\n", clnt_sock, clnt_cnt);
        if(clnt_sock==clnt_socks[i])
        {
            while(i<clnt_cnt) {
                clnt_socks[i]=clnt_socks[i+1];
                i++;
            }
            break;
        }
    }
    clnt_cnt--;
	printf("clnt_sock = %d closed\n", clnt_sock);
    pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}

void send_info(int clnt_num)   // send to clnt_num
{
    pthread_mutex_lock(&mutx);
	memcpy(bus.seats, seat_save, sizeof(seat_save));
	if(bus.command == 1){ // 조회
		bus.result = 0;
    }
	else if(bus.command == 2) // 예약
	{
		//
		int snum = bus.seatno;
		if(snum <= 0 || snum >= 21)
			bus.result = -1;
		else
		{
			if(bus.seats[snum-1] == 0) // 빈 좌석
			{
				bus.seats[snum-1] = clnt_num; // 예약
				memcpy(seat_save, bus.seats, sizeof(seat_save));
				bus.result = 0; // 예약 성공
			}
			else
				bus.result = -2; // 예약 실패
		}
		//
	}
	else if(bus.command == 3) // 예약 취소
	{
		//
		int snum = bus.seatno;
		if(snum <1 || snum > 20) // 잘못된 좌석 번호
			bus.result = -1;
		else
		{
			if(bus.seats[snum-1] == 0) // 예약 취소 실패(예약된 좌석이 아님)
			{
				bus.result = -3;
			}
			else if(bus.seats[snum-1] != clnt_num) // 예약 취소 실패(예약자 아님)
			{
				bus.result = -4;
			}
			else // 예약 취소 가능
			{
				bus.seats[snum-1] = 0;
				memcpy(seat_save, bus.seats, sizeof(seat_save));	
				bus.result = 0;
			}
		}
		//
	}
	write(clnt_num, (void*)&bus, sizeof(BUS_INFO));
	pthread_mutex_unlock(&mutx);
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
