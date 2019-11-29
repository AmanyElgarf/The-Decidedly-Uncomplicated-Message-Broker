#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <signal.h>


int main(int argc, char* argv[]){
    
    int port_num = atoi(argv[1]);
    
    int sockfd, new_socket, len;
    struct sockaddr_in serv_addr, cli;
    
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        return 0;
    }
    else
        printf("Socket successfully created..\n");
    bzero((char*)&serv_addr, sizeof(serv_addr));
    
    // assign IP, PORT
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);
    
    // Binding newly created socket to given IP and verification
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("socket bind failed...\n");
        return 0;
    }
    else
        printf("Socket successfully binded..\n");
    
    // Now server is ready to listen and verification
    listen(sockfd, 5);
    
    len = sizeof(cli);
    
    // Accept the data packet from client and verification
    new_socket = accept(sockfd, (struct sockaddr*)&cli, &len);
    if (new_socket < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");
    
    int  valread;
    char buffer[1024] = {0};
    char *hello = "HELLO DUMBv0 ready!";
    
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    
    
    
    
}
