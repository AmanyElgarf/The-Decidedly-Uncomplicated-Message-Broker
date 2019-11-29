
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

int main(int argc, const char * argv[]) {
//    quit            (which causes: E.1 GDBYE)
//    create        (which causes: E.2 CREAT)
//    delete        (which causes: E.6 DELBX)
//    open        (which causes: E.3 OPNBX)
//    close        (which causes: E.7 CLSBX)
//    next            (which causes: E.4 NXTMG)
//    put            (which causes: E.5 PUTMG)
//    On 'help' the client should list the commands above.
    
    char* hostname = argv[1];
    int port_num = atoi(argv[2]);
    
    printf("%s\n", hostname);
    
    printf("%d\n", port_num);
    

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");   //handle error
        return -1;
    }
    
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL){ //TODO error
        printf("hostname error\n");
        return 0;
        
    }
    
    
    ////https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);
    serv_addr.sin_addr = *(struct in_addr* )* host->h_addr_list;

    
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    else printf("connecting...\n");
    
    
    
//    int num;
//    printf("Enter the integer: ");
//    scanf("%d", &num);
    return 0;
}
