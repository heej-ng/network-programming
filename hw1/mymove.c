#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 3000000

void error_handling1(char* message);

int main(int argc, char* argv[])
{
    int input;
    int output;

    char buf[BUF_SIZE];
    if (argc != 3) 
        error_handling1("[Error] mymove Usage: ./mymove src_file dest_file");
    

    input=open(argv[1], O_RDONLY);

    if(input==-1)
        error_handling1("open() error!");
    if (read(input, buf, sizeof(buf)) == -1)
        error_handling1("read() error!");
    
    output=open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, 0644);

    if(output==-1)
        error_handling1("open() error!");
    

    int len = strlen(buf);

    if (write(output, buf, len) == -1)
        error_handling1("write() error!");

    printf("move from %s to %s (bytes: %d) finished.\n", argv[1], argv[2], len);
    remove(argv[1]);
    close(input);
    close(output);
}


void error_handling1(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}