// 2020110007 박희중
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct{
	int command;
	int seatno;
	int seats[20];
	int result;
}BUS_INFO;

BUS_INFO bus;

void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);
void printSeat();

static sem_t sem_one;
static sem_t sem_two;

int main(int argc, char *argv[])
{
	int sock;
	sem_init(&sem_one, 0, 0);
	sem_init(&sem_two, 0, 1);
	void * thread_return;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	 }
	sock=socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");

	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);

	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	sem_destroy(&sem_one);
	sem_destroy(&sem_two);

	close(sock);  
	return 0;
}
void printSeat()
{
	printf("\n-----------------------------------------\n");
	for(int i=1; i<11; i++)
		printf("%2d ", i);
	printf("\n");
	for(int i=0; i<10; i++)
		printf("%2d ", bus.seats[i]);
	printf("\n-----------------------------------------\n");
	for(int i=11; i<21; i++)
		printf("%2d ", i);
	printf("\n");
	for(int i=10; i<20; i++)
		printf("%2d ", bus.seats[i]);
	printf("\n-----------------------------------------\n");
}
	
void * send_msg(void * arg)   // send thread
{
	int sock=*((int*)arg);
	int menu;
	while(1) 
	{
		sem_wait(&sem_two); // two == 1
		memset(&bus, 0, sizeof(BUS_INFO));

		printf("1: inquiry, 2: reservation, 3: cancellation, 4: quit: ");
		scanf("%d", &menu);
		bus.command = menu;
	
		if(menu == 2)
		{
			int seatno;
			printf("Input seat number: ");
			scanf("%d", &seatno);
			bus.seatno = seatno;
		}
		else if(menu == 3)
		{
			int cancel_seatno;
			printf("Input seat number of cancellation: ");
			scanf("%d", &cancel_seatno);
			bus.seatno = cancel_seatno;
		}
		else if(menu == 4) // 4) 종료 메뉴 
		{
			printf("Quit.\n");
			close(sock);
			exit(0);
		}
		
		write(sock, (void*)&bus, sizeof(BUS_INFO));
		sem_post(&sem_one);
	}
	return NULL;
}
	
void * recv_msg(void * arg)   // recv thread
{
	int sock=*((int*)arg);
	int result;
	while(1)
	{
		sem_wait(&sem_one);
		read(sock, (void*)&bus, sizeof(BUS_INFO));
		result = bus.result;

		if(result == 0) // success
		{
			printSeat();
			printf("Operation success.\n");
		}
		else if(result == -1) // 잘못된 좌석번호 입력
		{
			printSeat();
			printf("Wrong seat number.\n");
		}
		else if(result == -2) // 예약 실패
		{
			printSeat();
			printf("Reservation failed. (The seat was already reserved.)\n");
		}
		else if(result == -3) // 예약 취소 실패(예약 좌석 x)
		{
			printSeat();
			printf("Cancellation failed. (The seat was not reserved.)\n");
		}
		else if(result == -4) // 예약 취소 실패(예약자 자신 x)
		{
			printSeat();
			printf("Cancellation failed. (The seat was reserved by another person.)\n");
		}
		sem_post(&sem_two);
	}
	return NULL;
}
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
