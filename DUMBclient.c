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


//if(strlen(name) < 5 || strlen(name) > 25 || !isalpha(name[0])){
//    printf("Error. The name must be 5 to 25 characters long and start with an alphabetic character");
//    return;
//}

void commands(){
    printf("Please choose a command from the command list below: \n");
    printf("%s\n", "1. quit");
    printf("%s\n", "2. create");
    printf("%s\n", "3. delete");
    printf("%s\n", "4. open");
    printf("%s\n", "5. close");
    printf("%s\n", "6. next");
    printf("%s\n", "7. put");
}

void quit(int sock){
    int valread;
    char buffer[1024] = {0};
    char* send_to_server = "GDBYE";
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 1024);
    if(valread == 0){ //not sure if this should be 0 or -1
        printf("Connection closed\n");
    }
    else{
        printf("Error. Connection was not closed\n");
    }
}


void create(int sock, char *name){
    int valread;
    char buffer[1024] = {0};
    char* send_to_server = strcat("CREAT ", name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 1024);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully created\n");
    }
    else if(strcmp(buffer, "EXIST") ==0){
        printf("Error. This name already exists\n");
    }
    else if(strcmp(buffer, "WHAT?") ==0){
        printf("Error. The name must be 5 to 25 characters long and start with an alphabetic character\n");
    }
    
}

void openBox(int sock, char *name){
    int valread;
    char buffer[1024] = {0};
    char* send_to_server = strcat("OPNBX ", name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 1024);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully opened\n");
    }
    else if(strcmp(buffer, "NEXST") ==0){
        printf("Error. This name doesn't exists\n");
    }
    else if(strcmp(buffer, "OPEND") ==0){
        printf("Error. This box is already opened by another user\n");
    }
    else if(strcmp(buffer, "WHAT?") ==0){
        printf("Error. Please enter a valid command followed by a valid name\n");
    }
    
}

void next(int sock){
    int valread;
    char buffer[1024] = {0};
    char* send_to_server = "NXTMG";
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 1024);
    if(strcmp(buffer, "NOOPN") ==0){
        printf("Error. Either the message box not open or doesn't exist\n");
    }
    else if(strcmp(buffer, "EMPTY?") ==0){
        printf("Error. No messages left in this message box\n");
    }
    else if(strcmp(buffer, "WHAT?") ==0){
        printf("Error. Please enter a valid command\n");
    }
    else{
        char* token = strtok(buffer, "!");
        token = strtok(buffer, "!");
        token = strtok(buffer, "!");
        printf("%s\n", token);
        
    }
}

void put(int sock, char* msg){
    int valread;
    char buffer[1024] = {0};
    int len = strlen(msg);
    
    char str[12];
    sprintf(str, "%d", len);
    char buff[30];
    char* send_to_server1 = strcat("PUTMG!", str);
    char* send_to_server2 = strcat(send_to_server1, "!");
    char* send_to_server = strcat(send_to_server2, msg);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 1024);
    if(strcmp(buffer, "NOOPN") ==0){
        printf("Error. You have no message box open\n");
    }
    else if(strcmp(buffer, "WHAT?") ==0){
        printf("Error. Please enter a valid command\n");
    }
    else{
        printf("Success. Message was put in the message box\n");
    }
}

void delete(int sock, char* name){
    int valread;
    char buffer[1024] = {0};
    char* send_to_server = strcat("DELBX ", name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 1024);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully deleted\n");
    }
    else if(strcmp(buffer, "NEXST") ==0){
        printf("Error. Box doesn't exist\n");
    }
    else if(strcmp(buffer, "OPEND") ==0){
        printf("Error. This box is currently open\n");
    }
    else if(strcmp(buffer, "NOTMT") ==0){
        printf("Error. This box is not empty\n");
    }
    else if(strcmp(buffer, "WHAT?") ==0){
        printf("Error. Please enter a valid command/name\n");
    }
    
}

void closeBox(int sock, char* name){
    int valread;
    char buffer[1024] = {0};
    char* send_to_server = strcat("CLSBX ", name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 1024);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully closed\n");
    }
    else if(strcmp(buffer, "NOOPN") ==0){
        printf("Error. This box is not currently open\n");
    }
    else if(strcmp(buffer, "WHAT?") ==0){
        printf("Error. Please enter a valid command/name\n");
    }
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
        send(sock, hello ,strlen(hello) , 0);
        valread = read(sock, buffer, 1024);
        if(strcmp(buffer, "HELLO DUMBv0 ready!") ==0){break;}
        else{if(i==2){printf("Connection Failed\n"); return -1;}else{continue;}}
    }
    
    commands();
    char input[26];
    while(1){
        bzero(input, 26);
        scanf("%s", input);
        if(strcmp(input, "quit") == 0){
            quit(sock); //
            close(sock);
            return 0;
        }
        else if(strcmp(input, "create") == 0){
            printf("Okay, enter the name of the box");
            char name[26];
            scanf("%s", name);
            create(sock, name);
        }
        else if(strcmp(input, "delete") == 0){
            printf("Okay, enter the name of the box");
            char name[26];
            scanf("%s", name);
            delete(sock, name);
        }
        else if(strcmp(input, "open") == 0){
            printf("Okay, open which message box?");
            char name[26];
            scanf("%s", name);
            openBox(sock, name);
        }
        else if(strcmp(input, "close") == 0){
            printf("Okay, close which message box?");
            char name[26];
            scanf("%s", name);
            closeBox(sock, name);
        }
        else if(strcmp(input, "next") == 0){
            next(sock);
        }
        else if(strcmp(input, "put") == 0){
            printf("Okay, enter the message you want to put");
            char msg[26];
            scanf("%s", msg);
            put(sock, msg);
        }
        else if(strcmp(input, "help") == 0){
            commands();
        }
        else{
            printf("That is not a command, for a command list enter 'help'.");
        }
    }
}


