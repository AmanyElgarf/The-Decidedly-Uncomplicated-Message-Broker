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

///TODO --> EDIT NUMBERS AND CAPACITIES LATER AFTER TESTING THAT ALL COMMANDS WORK


int isValidCommand(char * name){
    if(strlen(name) < 5 || strlen(name) > 25 || !isalpha(name[0])){
        printf("Error. The name must be 5 to 25 characters long and start with an alphabetic character\n");
        return 0;
    }
    return 1;
}


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

int quit(int sock){
    int valread;
    char buffer[9] = {0};
    char send_to_server[6] = "GDBYE";
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 9);
    if(valread == 0){
        printf("Connection closed\n");
        return 1;
    }
    else{
        printf("Error. Connection was not closed\n");
        return 0;
    }
}


void create(int sock, char *name){
    int valread;
    char buffer[9] = {0};
    char send_to_server[100] = "CREAT ";
    strcat(send_to_server, name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 9);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully created\n");
    }
    else if(strcmp(buffer, "ER:EXIST") ==0){
        printf("Error. A box with this name already exists\n");
    }
    else if(strcmp(buffer, "ER:WHAT?") ==0){
        if(isValidCommand(name) == 0){
            return;
        }
        else{
            printf("Error. Your message is in some way broken or malformed\n");
        }
    }
}

void openBox(int sock, char *name){
    int valread;
    char buffer[9] = {0};
    char send_to_server[35] = "OPNBX ";
    strcat(send_to_server, name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 9);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully opened\n");
    }
    else if(strcmp(buffer, "ER:NEXST") ==0){
        printf("Error. This name doesn't exist\n");
    }
    else if(strcmp(buffer, "ER:OPEND") ==0){
        printf("Error. This box is already opened by another user\n");
    }
    else if(strcmp(buffer, "ER:WHAT?") ==0){
        if(isValidCommand(name) == 0){
            return;
        }
        else{
            printf("Error. Your message is in some way broken or malformed\n");
        }
    }
    
}

void next(int sock){
    int valread;
    char buffer[100] = {0};
    char* send_to_server = "NXTMG";
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 100);
    if(strcmp(buffer, "ER:NOOPN") ==0){
        printf("Error. Either the message box not open or doesn't exist\n");
    }
    else if(strcmp(buffer, "ER:EMPTY") ==0){
        printf("Error. No messages left in this message box\n");
    }
    else if(strcmp(buffer, "ER:WHAT?") ==0){
        printf("Error. Please enter a valid command\n");
    }
    else{
        char *last = strrchr(buffer, '!');
        if(last != NULL){
            printf("%s\n", last+1);
        }
        
    }
}

void put(int sock, char* msg){
    int valread;
    char buffer[9] = {0};
    int len = strlen(msg);
    char str[12];
    sprintf(str, "%d", len);
    char buff[30];
    char send_to_server[200] = "PUTMG!";
    strcat(send_to_server, str);
    strcat(send_to_server, "!");
    strcat(send_to_server, msg);
    //printf("%s\n", send_to_server); ////////
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 9);
    //printf("%s\n", buffer);  //////////
    if(strcmp(buffer, "ER:NOOPN") ==0){
        printf("Error. You have no message box open\n");
    }
    else if(strcmp(buffer, "ER:WHAT?") ==0){
        printf("Error. Please enter a valid command\n");
    }
    else{
        printf("Success. Message was put in the message box\n");
    }
}

void delete(int sock, char* name){
    int valread;
    char buffer[9] = {0};
    char send_to_server[35] = "DELBX ";
    strcat(send_to_server, name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 9);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully deleted\n");
    }
    else if(strcmp(buffer, "ER:NEXST") ==0){
        printf("Error. Box doesn't exist\n");
    }
    else if(strcmp(buffer, "ER:OPEND") ==0){
        printf("Error. This box is currently open\n");
    }
    else if(strcmp(buffer, "ER:NOTMT") ==0){
        printf("Error. This box is not empty\n");
    }
    else if(strcmp(buffer, "ER:WHAT?") ==0){
        if(isValidCommand(name) == 0){
            return;
        }
        else{
            printf("Error. Your message is in some way broken or malformed\n");
        }
    }
    
}

void closeBox(int sock, char* name){
    int valread;
    char buffer[9] = {0};
    char send_to_server[35] = "CLSBX ";
    strcat(send_to_server, name);
    send(sock, send_to_server ,strlen(send_to_server) , 0);
    valread = read(sock, buffer, 9);
    if(strcmp(buffer, "OK!") ==0){
        printf("Successfully closed\n");
    }
    else if(strcmp(buffer, "ER:NOOPN") ==0){
        printf("Error. This box is not currently open\n");
    }
    else if(strcmp(buffer, "ER:WHAT?") ==0){
        if(isValidCommand(name) == 0){
            return;
        }
        else{
            printf("Error. Your message is in some way broken or malformed\n");
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printf("Please enter a host and a port number.\n");
        return -1;
    }
    
    char* hostname = argv[1];
    int PORT = atoi(argv[2]);
    int sock, valread;
    struct sockaddr_in serv_addr;
    
    
    struct hostent *host = gethostbyname(hostname);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    bzero(&serv_addr, sizeof(serv_addr));
    
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
    
    
    char buffer[9] = {0};
    char *hello = "HELLO";
	int i;
    for(i=0; i<3; ++i){ //try to connect for three times
        send(sock, hello ,strlen(hello) , 0);
        valread = read(sock, buffer, 1024);
        if(strcmp(buffer, "HELLO DUMBv0 ready!") ==0){printf("Success. You are connected.\n"); break;}
        else{if(i==2){printf("Connection Failed\n"); return -1;}else{continue;}}
    }
    
    commands();
    char input[26];
    while(1){
        bzero(input, sizeof(input));
        fflush(stdin);
        fgets(input, 26, stdin);
        input[strcspn(input, "\n")] = '\0';
        if(strcmp(input, "quit") == 0){
            int i = quit(sock);
            if(i==1){
                close(sock);
                return 0;
            }
        }
        else if(strcmp(input, "create") == 0){
            printf("%s\n", "Okay, enter the name of the box");
            printf("%s", "create:> ");
            fflush(stdin);
            char name[26];
            bzero(name, sizeof(name));
            fgets(name, 26, stdin);
            name[strcspn(name, "\n")] = '\0';
            fflush(stdin);
            create(sock, name);
        }
        else if(strcmp(input, "delete") == 0){
            printf("Okay, enter the name of the box\n");
            char name[26];
            fgets(name, 26, stdin);
            name[strcspn(name, "\n")] = '\0';
            delete(sock, name);
        }
        else if(strcmp(input, "open") == 0){
            printf("Okay, open which message box?\n");
            char name[26];
            printf("%s", "open:> ");
            fflush(stdin);
            fgets(name, 26, stdin);
            name[strcspn(name, "\n")] = '\0';
            openBox(sock, name);
        }
        else if(strcmp(input, "close") == 0){
            printf("Okay, close which message box?\n");
            char name[26];
            fgets(name, 26, stdin);
            name[strcspn(name, "\n")] = '\0';
            closeBox(sock, name);
        }
        else if(strcmp(input, "next") == 0){
            next(sock);
        }
        else if(strcmp(input, "put") == 0){
            printf("Okay, enter the message you want to put in the box.\n");
            char msg[26];
            printf("%s", "put:> ");
            fflush(stdin);
            fgets(msg, 26, stdin);
            msg[strcspn(msg, "\n")] = '\0';
            put(sock, msg);
        }
        else if(strcmp(input, "help") == 0){
            commands();
        }
        else{
            fflush(stdout);
            printf("That is not a command, for a command list enter 'help'.\n");
            fflush(stdout);
        }
    }
}
