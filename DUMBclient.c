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

#include "DUMBclient.h"

static int open = 0; //static varia

//function to check if the box name is correct
int isValidCommand(char * name){
    if(strlen(name) < 5 || strlen(name) > 25 || !isalpha(name[0])){
        printf("Error. The name must be 5 to 25 characters long and start with an alphabetic character\n");
        return 0;
    }
    return 1;
}

//help function to print all the available command to the user
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


//function to close the connection between the client and the server
int quit(int sock){
    int valread;
    char buffer[256] = {0};
    char send_to_server[6] = "GDBYE";
    write(sock, send_to_server ,strlen(send_to_server));
    valread = read(sock, buffer, 255);
    if(valread == 0){
        printf("Connection closed\n");
        return 1;
    }
    else{
        printf("Error. Connection was not closed\n");
        return 0;
    }
}

//function to create a new box
void create(int sock, char *name){
    int valread;
    char buffer[256] = {0};
    char send_to_server[1000] = "CREAT ";
    strcat(send_to_server, name);
    write(sock, send_to_server ,strlen(send_to_server));
    valread = read(sock, buffer, 255);
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

//function to open an existing box;
void openBox(int sock, char *name){

    int valread;
    char buffer[256] = {0};
    char send_to_server[1000] = "OPNBX ";
    strcat(send_to_server, name);
    write(sock, send_to_server ,strlen(send_to_server));
    valread = read(sock, buffer, 255);
    if(strcmp(buffer, "OK!") ==0){
		open = 1;
        printf("Successfully opened\n");
    }
    else if(strcmp(buffer, "ER:NEXST") ==0){
        printf("Error. This name doesn't exist\n");
    }
    else if(strcmp(buffer, "ER:OPEND") ==0){
			printf("Error. This box is already opened by another user \n");
    }
    else if (strcmp(buffer, "ER:CUROP") ==0){
      printf("Error. You currently have a box opened\n");
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

//function to print out the next message in the box
void next(int sock){
    int valread;
    char buffer[10000] = {0};
    char* send_to_server = "NXTMG";
    write(sock, send_to_server ,strlen(send_to_server));
    valread = read(sock, buffer, 9999);
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
        char *first = strstr(buffer, "!");
		char *second = strstr(first+1, "!");
		printf("%s\n", second+1);


    }
}

//function to get a substring from a main string
int  getSubString(char *source, char *target,int from, int to)
{
	int length=0;
	int i=0,j=0;

	//get length
	while(source[i++]!='\0')
		length++;

	if(from<0 || from>length){
		return 1;
	}
	if(to>length){
		return 1;
	}

	for(i=from,j=0;i<=to;i++,j++){
		target[j]=source[i];
	}

	//assign NULL at the end of string
	target[j]='\0';

	return 0;
}

//function to put a message inside a box
void put(int sock, char* msg){


    int valread;
    char buffer[256] = {0};

    char send_to_server[10000] = "PUTMG!";

	if(strlen(msg) > 4000){
        printf("Your message was trimmed\n");
		char newMsg[5000] = {0};
		if(getSubString(msg, newMsg, 0, 4000)==0){

			int len = strlen(newMsg);
   			char str[5000];
   			sprintf(str, "%d", len);
			strcat(send_to_server, str);
   			strcat(send_to_server, "!");
			strcat(send_to_server, newMsg);
		}
		else{printf("Command failed\n");}

    }
    else {
		int len = strlen(msg);
    	char str[5000];
   		sprintf(str, "%d", len);
		strcat(send_to_server, str);
    	strcat(send_to_server, "!");
		strcat(send_to_server, msg);
	}

    //printf("%s\n", send_to_server);
	fflush(stdout);
    write(sock, send_to_server ,strlen(send_to_server));
    valread = read(sock, buffer, 255);
    //printf("%s\n", buffer);
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

//function to delete an existing box
void deletee(int sock, char* name){
    int valread;
    char buffer[256] = {0};
    char send_to_server[1000] = "DELBX ";
    strcat(send_to_server, name);
    write(sock, send_to_server ,strlen(send_to_server));
    valread = read(sock, buffer, 255);
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

//function to close a bix that is opened
void closeBox(int sock, char* name){

    int valread;
    char buffer[1000] = {0};
    char send_to_server[999] = "CLSBX ";
    strcat(send_to_server, name);
    write(sock, send_to_server ,strlen(send_to_server));
    valread = read(sock, buffer, 999);
    if(strcmp(buffer, "OK!") ==0){
		open = 0;
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

    char* hostname = argv[1];  //retrieve the hostname
    int PORT = atoi(argv[2]); //retrieve the port number
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

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }


    char buffer[256] = {0};
    char *hello = "HELLO";
	int i;
    for(i=0; i<3; ++i){ //try to connect for three times
		bzero(buffer, 256);
        write(sock, hello ,strlen(hello));
        valread = read(sock, buffer, 255);
        if(strcmp(buffer, "HELLO DUMBv0 ready!") ==0){printf("Success. You are connected.\n"); break;}
        else{if(i==2){printf("Connection Failed\n"); return -1;}else{continue;}}
    }

    commands();
    char input[1000];
    while(1){
        bzero(input, sizeof(input));
        fflush(stdin);
        fgets(input, 1000, stdin);
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
            char name[1000];
            bzero(name, sizeof(name));
            fgets(name, 1000, stdin);
            name[strcspn(name, "\n")] = '\0';
            fflush(stdin);
            create(sock, name);
        }
        else if(strcmp(input, "delete") == 0){
            printf("Okay, enter the name of the box\n");
            char name[1000];
            fgets(name, 1000, stdin);
            name[strcspn(name, "\n")] = '\0';
            deletee(sock, name);
        }
        else if(strcmp(input, "open") == 0){
            printf("Okay, open which message box?\n");
            char name[1000];
            printf("%s", "open:> ");
            fflush(stdin);
            fgets(name, 1000, stdin);
            name[strcspn(name, "\n")] = '\0';
            openBox(sock, name);
        }
        else if(strcmp(input, "close") == 0){
            printf("Okay, close which message box?\n");
            char name[1000];
            fgets(name, 1000, stdin);
            name[strcspn(name, "\n")] = '\0';
            closeBox(sock, name);
        }
        else if(strcmp(input, "next") == 0){
            next(sock);
        }
        else if(strcmp(input, "put") == 0){
            printf("Okay, enter the message you want to put in the box.\n");
            char msg[10000];
            printf("%s", "put:> ");
            fflush(stdin);
            fgets(msg, 10000, stdin);
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
