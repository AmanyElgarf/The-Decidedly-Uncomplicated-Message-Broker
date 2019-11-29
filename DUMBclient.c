#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <signal.h>

void commands(){
    printf("%s\n", "quit");
    printf("%s\n", "create");
    printf("%s\n", "delete");
    printf("%s\n", "open");
    printf("%s\n", "close");
    printf("%s\n", "next");
    printf("%s\n", "put");
}


int main(int argc, char const *argv[])
{
    char* hostname = argv[1];
    int PORT = atoi(argv[2]);
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    
    char buffer[1024] = {0};
    struct hostent *host = gethostbyname(hostname);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    serv_addr.sin_addr = *(struct in_addr* )* host->h_addr_list;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    char *hello = "HELLO";
    for(int i=0; i<3; ++i){ //try to connect for three times
        send(sock , hello , strlen(hello) , 0 );
        valread = read(sock , buffer, 1024);
        if(strcmp(buffer, "HELLO DUMBv0 ready!") ==0){break;}
        else{if(i==2){printf("Connection Failed\n"); return -1;}else{continue;}}
    }
    
    printf("Please choose a command from the command list below: \n");
    commands();
    char input[20];
    scanf("%s\n", input);
    printf("%s\n",input);
    return 0;
} 
