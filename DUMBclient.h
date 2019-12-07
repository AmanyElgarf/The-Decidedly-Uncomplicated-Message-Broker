#ifndef DUMBclient_h
#define DUMBclient_h

//print commands
void commands();

//close connection
int quit(int sock);

//create a new box
void create(int sock, char *name);

//open a box
void openBox(int sock, char *name);

//print nnext message
void next(int sock);

//split a string --> helper function
int  getSubString(char *source, char *target,int from, int to);

//put  a message in a message box
void put(int sock, char* msg);

//delete a message from a message box
void deletee(int sock, char* name);

//close a box
void closeBox(int sock, char* name);


#endif
