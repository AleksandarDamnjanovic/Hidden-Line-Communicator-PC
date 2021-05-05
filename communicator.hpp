#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#ifdef __cplusplus
    extern "C"{
#endif

bool requestConnection(char* address, int port, char* page, char* id, char* code);
void getClientList(char* address, int port, char* page, char* id, char* code, int size, char* r[]);
void getMessages(char* address, int port, char* page, char* userName, char* password, char* chatWith, int reference, char* sender[], char* message[]);
void sendMessages(char* address, int port, char* page, char* userName, char* password, char* chatWith, char* message);
int getNumOfClients(char* address, int port,char* page, char* userName, char* password);
int getNumOfMessages(char* address, int port, char* page, char* userName, char* password, char* chatWith, int reference);

#ifdef __cplusplus
    }
#endif

#endif