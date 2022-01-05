#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;


	//char str_arr[99999];
	// char message[99999];
	int str_len;

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
	
    printf("---------------------------\n");
    printf(" Address Conversion Server\n");
    printf("---------------------------\n");
	

    while(1){
        char message[999];
        char *str_ptr;
        char str_arr[999];
        memset(message,'\0',sizeof(message));

	    str_len = read(clnt_sock, message, sizeof(message));
	    if(str_len == -1){
	    	error_handling("read() error! \n");
	    }
        if(strcmp(message,"quit")==0){
            printf("quit received and exit program!\n");
            break;
        }
	    printf("Received Dotted-Decimal Address: %s \n", message);


        // unsigned long conv_addr = inet_addr(message);
		// if (conv_addr == INADDR_NONE) {
		//     write(clnt_sock, "Address conversion fail: Format error", sizeof("Address conversion fail: Format error"));
		//     printf("Address conversion fail: Format error \n");
        //     continue;
		// }

	    if(inet_aton(message, &serv_addr.sin_addr)) {
	    	str_ptr = inet_ntoa(serv_addr.sin_addr);
    		printf("inet_aton: %s -> %#x \n", message, serv_addr.sin_addr.s_addr);

	    	//strcpy(str_arr, str_ptr);
	    	printf("inet_ntoa: %#x -> %s \n", serv_addr.sin_addr.s_addr, str_ptr);
            printf("Address conversion success\n\n");
		    write(clnt_sock, "Address conversion success", sizeof("Address conversion success"));
    	}
    	else {
		    printf("Address conversion fail: Format error \n\n");
	    	write(clnt_sock, "Address conversion fail: Format error", sizeof("Address conversion fail: Format error"));
	    }

        // if(inet_aton(message, &serv_addr.sin_addr)) {
		//     write(clnt_sock, "Address conversion success", sizeof("Address conversion success"));
    	// 	printf("inet_aton: %s => %#x \n", message, serv_addr.sin_addr.s_addr);

	    // 	str_ptr = inet_ntoa(serv_addr.sin_addr);
	    // 	strcpy(str_arr, str_ptr);
	    // 	printf("inet_ntoa: %#x => %s \n", serv_addr.sin_addr.s_addr, str_arr);
        //     printf("Address conversion success\n\n");

        // }
        
    }
	close(clnt_sock);	
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
