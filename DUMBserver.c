#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
#include <signal.h>

#include "DUMBserver.h"

//vars to set up server socket
int server_sock;
int port_num;
struct sockaddr_in server_address;

pthread_t* threads;
pthread_mutex_t lock;

struct clientParams
{
	int client_id;
	char ip[INET_ADDRSTRLEN];
};

//mailbox system
struct Messages* m;

/* Data Structures */
struct MessagesNode
{
	char msgbox_name[40]; //a message box name must be between 5 - 25 chars, 26 including null terminator
	struct Queue* msgbox;
	struct MessagesNode* next;
	int opened;
};

struct Messages
{
	struct MessagesNode* front;
};

//Node data structure
struct Node
{
	char* msg;
	struct Node* next;
};

//queue data structure built from linked list
struct Queue
{
	struct Node* front;
};


/* QUEUE (MSGBOX) FUNCTIONS */
//create a queue
struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*) malloc(sizeof(struct Queue));
    q -> front = NULL;
    return q;
}

//checks if queue is empty
int isEmpty(struct Queue* q)
{
	return (q -> front == NULL);
}

//add to front of LL
void enqueue(struct Queue* q, char* str)
{
	int n = strlen(str) + 1;
	struct Node* cur = (struct Node*) malloc(1*sizeof(struct Node));
	cur -> msg = (char*) malloc(n*sizeof(char));

	//no current Nodes in LL
	if (isEmpty(q))
	{
		strcpy(cur -> msg, str);
		cur -> next = NULL;
		q -> front = cur;
		return;
	}

	strcpy(cur -> msg, str);
	cur -> next = q -> front;
	q -> front = cur;

}

//remove last element
struct Node* dequeue(struct Queue* q)
{
	if (isEmpty(q))
	{
		printf("Error: cannot dequeue from an empty queue\n");
		return NULL;
	}

	//find last element in linked list
	struct Node* ptr = NULL;
	struct Node* prev = NULL;
	for (ptr = q -> front; ptr -> next != NULL; ptr = ptr -> next)
	{
		prev = ptr;
	}

	//case 1: LL only has 1 element
	if (prev == NULL) {
		q -> front = NULL;
		return ptr;
	}

	//case 2: LL has more than 1 element
	prev -> next = NULL;
	return ptr;
}

void freeQueue(struct Queue* q)
{
	free(q);
}


/* MESSAGE MAILBOX FUNCTIONS */
//initialize mailbox
struct Messages* initMessageSys()
{
	struct Messages* m = (struct Messages*) malloc(sizeof(struct Messages));
	m -> front = NULL;
	return m;
}

//checks if Messages mailbox is empty
int isEmptyMessages(struct Messages* m)
{
	return (m -> front == NULL);
}

//add to front of Messages
struct MessagesNode* addMessageBox(struct Messages* m, char* str)
{
	struct MessagesNode* cur = (struct MessagesNode*) malloc(1*sizeof(struct MessagesNode));

	//no current Message boxes in Messages
	if (isEmptyMessages(m))
	{
		strcpy(cur -> msgbox_name, str);
		cur -> msgbox = createQueue();
		cur -> next = NULL;
		m -> front = cur;
		return cur;
	}

	strcpy(cur -> msgbox_name, str);
	cur -> msgbox = createQueue();
	cur -> next = m -> front;
	m -> front = cur;

	return cur;
}

//remove message box
struct MessagesNode* deleteMessageBox(struct Messages* m, char* str)
{
	if (isEmptyMessages(m))
	{
		//Error: cannot delete message box from empty messages mailbox
		return NULL;
	}

	//find target with mailbox name
	struct MessagesNode* ptr = NULL;
	struct MessagesNode* prev = NULL;
	for (ptr = m -> front; ptr != NULL; ptr = ptr -> next)
	{
		if (strcmp(ptr -> msgbox_name, str) == 0)
		{
			if (prev == NULL)
			{
				//first element in LL
				m -> front = ptr -> next;
				return ptr;
			}

			prev -> next = ptr -> next;
			return ptr;
		}
		prev = ptr;
	}

	//Error: did not find message box to delete
	return NULL;
}

//find message box
struct MessagesNode* getMessageBox(struct Messages* m, char* str)
{
	if (isEmptyMessages(m))
	{
		return NULL;
	}

	//find target with mailbox name
	struct MessagesNode* ptr = NULL;
	for (ptr = m -> front; ptr != NULL; ptr = ptr -> next)
	{
		if (strcmp(ptr -> msgbox_name, str) == 0)
		{
			return ptr;
		}
	}

	//couldn't find message box with specified name
	return NULL;
}

void freeMessages(struct Messages* m)
{
	free(m);
}

/*
void printMessageSys(struct Messages* m)
{
	struct MessagesNode* ptr;
	for (ptr = m -> front; ptr != NULL; ptr = ptr -> next)
	{
		struct Node* ptr2;
		struct Node* tmp;
		for (ptr2 = ptr -> msgbox -> front; ptr2 != NULL; ptr2 = ptr2 -> next)
		{
			freeQueue(ptr2)
		}
	}
}
*/



/* Server functions */
void errorMsg(char* str, char* err)
{
	char res[256] = "";
	strcat(res, "ER:");
	strcat(res, err);
	strcpy(str, res);
}

void error(char* msg)
{
    perror(msg);
    exit(1);
}

void initServer(int port)
{
	 //AF_INET -> Internet protocol v4 (IP), SOCK_STREAM -> TCP connection-based protocol
     server_sock = socket(AF_INET, SOCK_STREAM, 0);

     if (server_sock < 0)
	 {
        error("Error opening socket\n");
	 }

	 //initialize server address memory
     bzero((char *) &server_address, sizeof(server_address));

	 //get the port number from argument
     port_num = port;

	 //setup the server struct
     server_address.sin_family = AF_INET;
     server_address.sin_addr.s_addr = INADDR_ANY;
     server_address.sin_port = htons(port_num);

	 //bind socket
     if (bind(server_sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
	 {
		error("Error binding socket\n");
	 }

	 //listen for connections, max connections = 1000
	 listen(server_sock, 1000);
}

int isValidName(char* name)
{
	int n = strlen(name);
	if (n < 5 || n > 25)
	{
		//length is not between 5 and 25, not valid
		return 0;
	}

	if (!isalpha(name[0]))
	{
		//does not start with a letter, not valid
		return 0;
	}

	//else, the name is valid
	return 1;
}

char* intToString(int num, char* ptr)
{
	if(ptr == NULL) return NULL;
	sprintf(ptr, "%d", num);
	return ptr;
}

int stringToInt(char* num)
{
	if (num == NULL) return -1;

	int i;
	for (i = 0; i < strlen(num); i++) {
		if (!isdigit(num[i])) return -1;
	}

	return atoi(num);
}


void printToServer(int success, int id, char* ip, char* msg)
{
	//date info
	time_t t = time(NULL);
	struct tm* date = localtime(&t);
	int day = date -> tm_mday;
	char month[25];

	switch (date -> tm_mon)
	{
		case 0:
			strcpy(month, "Jan");
			break;
		case 1:
			strcpy(month, "Feb");
			break;
		case 2:
			strcpy(month, "Mar");
			break;
		case 3:
			strcpy(month, "Apr");
			break;
		case 4:
			strcpy(month, "May");
			break;
		case 5:
			strcpy(month, "Jun");
			break;
		case 6:
			strcpy(month, "Jul");
			break;
		case 7:
			strcpy(month, "Aug");
			break;
		case 8:
			strcpy(month, "Sep");
			break;
		case 9:
			strcpy(month, "Oct");
			break;
		case 10:
			strcpy(month, "Nov");
			break;
		case 11:
			strcpy(month, "Dec");
			break;
		default:
			break;
	}

	if (success)
	{
		fprintf(stdout, "%d %d %s %s %s\n", id, day, month, ip, msg);
	}
	else
	{
		fprintf(stderr, "%d %d %s %s %s\n", id, day, month, ip, msg);
	}
}


void* initClient(void* params)
{
	struct clientParams* par = params;
	int client_sock = par -> client_id;
	char ip[INET_ADDRSTRLEN] = "";
	strcpy(ip, par -> ip);

	char openmsg[25] = "connected";
	printToServer(1, client_sock, ip, openmsg);
	//vars
	char buffer[5000];
	int n;

	//keeps track of if we currently have a box opened
	struct MessagesNode* openBox = NULL;

	char errmsg[500] = "";
	char sucmsg[500] = "OK!";

	while (1)
	{
		 if (client_sock < 0)
		 {
			printf("Error while accepting client socket.\n");
			if (openBox != NULL) openBox -> opened = 0;
			pthread_exit(NULL);
		 }

		 bzero(buffer,5000);
		 n = read(client_sock,buffer,4999);

		 //check for read error
		 if (n <= 0)
		 {
			printf("Error reading from socket %s\n", ip);
			if (openBox != NULL) openBox -> opened = 0;
			pthread_exit(NULL);
		 }

		 //check for no empty input
		 if (strcmp(buffer, "\n") == 0)
		 {
			errorMsg(errmsg, "WHAT?");
			printToServer(0, client_sock, ip, errmsg);
			n = write(client_sock, errmsg, 9);
			continue;
		 }

		 //get command and argument
		 char cmd[5000] = "\0";
		 char arg[5000] = "\0";

		 //get name of command and first arg
		 if (strncmp(buffer, "PUTMG", 5) == 0)
		 {
			 char* cur = strtok(buffer, "\n");
			 strcpy(cmd, cur);
		 }
		 else
		 {
			 int z = 0;
			 int y = 0;
			 int found_space = 0;
			 while (buffer[z] != '\0' && buffer[z] != '\n')
			 {
				 if (!found_space && buffer[z] == ' ') {
					 found_space = 1;
					 z++;
					 continue;
				 }
				 if (!found_space)
				 {
					 cmd[z] = buffer[z];
					 z++;
				 }
				 else {
					 arg[y] = buffer[z];
					 y++;
					 z++;
				 }
			 	}
		 }

		 //start the client-server connection
		 if (strcmp(cmd, "HELLO") == 0)
		 {
			 char msgForHello[256] = "HELLO DUMBv0 ready!";
			 n = write(client_sock, msgForHello, strlen(msgForHello));
			 printToServer(1, client_sock, ip, cmd);
		 }
		 //check to see which command to run
		 else if (strcmp(cmd, "GDBYE") == 0)
		 {
			//check if expression is well formed and only has 1 parameter
			if (strcmp(arg, "") != 0)
			{
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
				continue;
			}

			//if user forgot to close message box, close it for them
			if (openBox != NULL)
			{
				openBox -> opened = 0;
				openBox = NULL;
			}

			//report success
			printToServer(1, client_sock, ip, cmd);

			//close client socket
			close(client_sock);
			if (openBox != NULL) openBox -> opened = 0;
			pthread_exit(NULL);
		 }
		 else if (strcmp(cmd, "CREAT") == 0)
		 {
			 //get the current message box
			 struct MessagesNode* cur_box = NULL;
			 pthread_mutex_lock(&lock);
			 cur_box = getMessageBox(m, arg);
			 pthread_mutex_unlock(&lock);

			//check if name of message box is valid
			if (!isValidName(arg))
			{
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if message box name already exists
			else if (cur_box != NULL)
			{
				errorMsg(errmsg, "EXIST");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//else, create message box
			else
			{
				pthread_mutex_lock(&lock);
				addMessageBox(m, arg);
				pthread_mutex_unlock(&lock);
				n = write(client_sock, sucmsg, 3);

				//report success
				printToServer(1, client_sock, ip, cmd);
			}
		 }
		 else if (strcmp(cmd, "OPNBX") == 0)
		 {
		 	//get the current message box
			struct MessagesNode* cur_box = NULL;
			pthread_mutex_lock(&lock);
			cur_box = getMessageBox(m, arg);

			//check if name of message box is valid
			if (!isValidName(arg))
			{
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if there is no existing message box with this name
			else if (cur_box == NULL)
			{
				errorMsg(errmsg, "NEXST");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			else if (openBox != NULL)
			{
				errorMsg(errmsg, "CUROP");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if the message box is already opened OR you already have one opened
			else if (cur_box -> opened == 1)
			{
				errorMsg(errmsg, "OPEND");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//open the message box
			else
			{
				cur_box -> opened = 1;
				openBox = cur_box;
				n = write(client_sock, sucmsg, 3);
				printToServer(1, client_sock, ip, cmd);
			}
			pthread_mutex_unlock(&lock);
		 }
		 else if (strcmp(cmd, "NXTMG") == 0)
		 {
			struct Node* cur_msg_node;
			char cur_msg[3+4+256+1] = ""; //3 for ok!, 4 for char byte length and !, 256 for max message size, 1 for null terminator
			int cur_msg_len = 0;

			//check if expression is well formed and only has 1 parameter
			if (strcmp(arg, "") != 0)
			{
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if we have a valid message box opened
			else if (openBox == NULL)
			{
				errorMsg(errmsg, "NOOPN");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if there are no messages left
			else if (isEmpty(openBox -> msgbox))
			{
				errorMsg(errmsg, "EMPTY");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//get the next message from the msgbox queue
			else
			{
				pthread_mutex_lock(&lock);
				cur_msg_node = dequeue(openBox -> msgbox);
				pthread_mutex_unlock(&lock);
				strcat(cur_msg, "OK!");
				cur_msg_len += 3;

				pthread_mutex_lock(&lock);
				int curlen = strlen(cur_msg_node -> msg);
				pthread_mutex_unlock(&lock);
				char curnum[50];
				intToString(curlen, curnum);

				strcat(cur_msg, curnum);
				cur_msg_len += strlen(curnum);

				strcat(cur_msg, "!");
				cur_msg_len += 1;

				pthread_mutex_lock(&lock);
				strcat(cur_msg, cur_msg_node -> msg);
				pthread_mutex_unlock(&lock);
				cur_msg_len += curlen;

				//free the dequeue'd node and its message array
				free(cur_msg_node -> msg);
				free(cur_msg_node);

				//printf("we sent the client %d bytes", cur_msg_len);
				n = write(client_sock, cur_msg, cur_msg_len);

				//report success
				printToServer(1, client_sock, ip, cmd);
			}
		 }
		 else if (strstr(cmd, "PUTMG"))
		 {
			/*
		 	//check if expression is well formed and only has 1 parameter
			if (strcmp(arg, "") != 0)
			{
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
				continue;
			}
			*/
			//break up the command into 3 parts, make sure it has atleast 2 '!' to be valid
			int num_exclamations = 0;
			int i;
			for (i = 0; i < strlen(cmd); i++) {
				if (cmd[i] == '!') num_exclamations++;
			}

			//if less than 2 ! marks, this is not well formed. OR if we had more than 1 parameter
			if (num_exclamations < 2) {
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
				continue;
			}

			 //number of bytes and string message
			 char num_bytes[50] = "";
			 char cur_msg[5000] = "";
			 int num_bytes_int = 0;

			 /*
			 char* tmp_line = strtok(cmd, "!");
			 tmp_line = strtok(NULL, "!");
			 strcpy(num_bytes, tmp_line);
			 tmp_line = strtok(NULL, "!");
			 strcpy(cur_msg, tmp_line);
			 */

			 //STRING PARSING OF FIRST TWO ! DELIMITER
			 int exc_seen = 0;
			 i = 0;
			 int j = 0;
			 while (exc_seen < 2)
			 {
				if (cmd[i] == '!')
				{
					exc_seen++;
					j = i+1;
				}
				else
				{
					if (exc_seen == 1)
					{
						num_bytes[i-j] = cmd[i];
					}
				}
				i++;
			 }

			 for (; i < strlen(cmd); i++)
			 {
				cur_msg[i-j] = cmd[i];
			 }

			 num_bytes_int = stringToInt(num_bytes);
			 //if negative value, the number of bytes read was not a pure int. OR if the num bytes doesn't equal the message bytes, the message is not well formed.
			 if (num_bytes_int < 0 || num_bytes_int != strlen(cur_msg)) {
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
				continue;
			 }

			//check if we have a valid message box opened
			if (openBox == NULL)
			{
				errorMsg(errmsg, "NOOPN");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//else, let us add the message
			else
			{
				pthread_mutex_lock(&lock);
				enqueue(openBox -> msgbox, cur_msg);
				pthread_mutex_unlock(&lock);
				char success_msg[500] = "";
				strcat(success_msg, "OK!");
				strcat(success_msg, num_bytes);
				n = write(client_sock, success_msg, strlen(success_msg));

				char tmp[10] = "";
				strcpy(tmp, "PUTMG");
				//print success
				printToServer(1, client_sock, ip, tmp);
			}
		 }
		 else if (strcmp(cmd, "DELBX") == 0)
		 {
		 	//get the current message box
			struct MessagesNode* cur_box = NULL;
			pthread_mutex_lock(&lock);
			cur_box = getMessageBox(m, arg);

			//check if name of message box is valid
			if (!isValidName(arg))
			{
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if message box exists
			else if (cur_box == NULL)
			{
				errorMsg(errmsg, "NEXST");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if box is currently opened
			else if (cur_box -> opened == 1)
			{
				errorMsg(errmsg, "OPEND");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if message box is empty or not
			else if (!isEmpty(cur_box -> msgbox))
			{
				errorMsg(errmsg, "NOTMT");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//else, delete the message box since it is not open and empty
			else
			{
				//free the queue in the message box and then the message box itself
				cur_box = deleteMessageBox(m, cur_box -> msgbox_name);
				freeQueue(cur_box -> msgbox);
				free(cur_box);
				n = write(client_sock, sucmsg, 3);

				//report success
				printToServer(1, client_sock, ip, cmd);
			}
			pthread_mutex_unlock(&lock);
		 }
		 else if (strcmp(cmd, "CLSBX") == 0)
		 {
		 	//get the current message box
			struct MessagesNode* cur_box = NULL;
			pthread_mutex_lock(&lock);
			cur_box = getMessageBox(m, arg);
			pthread_mutex_unlock(&lock);

			//check if name of message box is valid
			if (!isValidName(arg))
			{
				errorMsg(errmsg, "WHAT?");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if there is no existing message box with this name
			else if (cur_box == NULL)
			{
				errorMsg(errmsg, "NOOPN");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//check if the message box was opened by you
			else if (openBox != cur_box)
			{
				errorMsg(errmsg, "NOOPN");
				printToServer(0, client_sock, ip, errmsg);
				n = write(client_sock, errmsg, 8);
			}
			//close the message box
			else
			{
				pthread_mutex_lock(&lock);
				cur_box -> opened = 0;
				pthread_mutex_unlock(&lock);
				openBox = NULL;
				n = write(client_sock, sucmsg, 3);

				//report success
				printToServer(1, client_sock, ip, cmd);
			}
		 }
		 else
		 {
			errorMsg(errmsg, "WHAT?");
			printToServer(0, client_sock, ip, errmsg);
			n = write(client_sock, errmsg, 9);
		 }

		 //check for write error
		 if (n < 0)
		 {
			printf("Error writing to socket\n");
			if (openBox != NULL) openBox -> opened = 0;
			pthread_exit(NULL);
		 }

	 }

	 pthread_exit(NULL);
}


void freeAllMemory()
{
	pthread_mutex_lock(&lock);
	struct MessagesNode* ptr;
	struct MessagesNode* next;
	for (ptr = m -> front; ptr != NULL; ptr = next)
	{
		next = ptr -> next;

		while (!isEmpty(ptr -> msgbox))
		{
			struct Node* tmp = dequeue(ptr -> msgbox);
			free(tmp -> msg);
			free(tmp);
		}

		freeQueue(ptr -> msgbox);
		free(ptr);
	}

	freeMessages(m);

	pthread_mutex_unlock(&lock);

	pthread_mutex_destroy(&lock);

	free(threads);
}

void exitServer(int sig_num)
{
		printf("\nExiting server... freeing all allocated memory\n");
    freeAllMemory();
		printf("Successfully ended server\n");
		close(server_sock);
		exit(0);
}

int main(int argc, char** argv)
{
   if (argc < 2)
 	 {
       printf("Please enter a host and a port number.\n");
       exit(1);
   }

	//initialize message mailbox system
	m = initMessageSys();


	 signal(SIGINT, exitServer);

	 initServer(atoi(argv[1]));

	 threads = (pthread_t*) malloc(sizeof(pthread_t)*1000);
	 int num_threads = 0;

	 while (1)
	 {
		 //vars to set up client socket
		 int client_sock;
		 socklen_t client_len;
		 struct sockaddr_in client_address;

		 //get client length
		 client_len = sizeof(client_address);

		 //block until a client tries to connect and then get the client socket
		 client_sock = accept(server_sock, (struct sockaddr *) &client_address, &client_len);

		//getting ip address in text form
		struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_address;
		struct in_addr ipAddr = pV4Addr->sin_addr;
		char str[INET_ADDRSTRLEN];
		inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

		//creating parameter list
		struct clientParams* p = (struct clientParams*)malloc(sizeof(struct clientParams));
		p -> client_id = client_sock;
		strcpy(p -> ip, str);

		 pthread_create(&threads[num_threads++], NULL, initClient, (void*)p);
	 }

	 	//printMessageSys(m);
    close(server_sock);

    return 0;
}
