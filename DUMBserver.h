#ifndef _DUMBserver_H
/* Queue functions */
struct Queue* createQueue();
int isEmpty(struct Queue* q);
void enqueue(struct Queue* q, char* str);
struct Node* dequeue(struct Queue* q);
void freeQueue(struct Queue* q);

/* Message box functions */
struct Messages* initMessageSys();
int isEmptyMessages(struct Messages* m);
struct MessagesNode* addMessageBox(struct Messages* m, char* str);
struct MessagesNode* deleteMessageBox(struct Messages* m, char* str);
struct MessagesNode* getMessageBox(struct Messages* m, char* str);
void freeMessages(struct Messages* m);

/* report errors */
void errorMsg(char* str, char* err);
void error(char* msg);

/* initializations */
void initServer(int port);
void* initClient(void* params);

/* formatting */
int isValidName(char* name);
char* intToString(int num, char* ptr);
int stringToInt(char* num);
void printToServer(int success, int id, char* ip, char* msg);

/* clean for termination */
void freeAllMemory();
void exitServer(int sig_num);

#define _DUMBserver_H
