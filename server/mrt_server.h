#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <iostream>
#ifndef __SERVER_H__
#define __SERVER_H__
class Server{
    public:
        Server(unsigned int port, long buf);
        int accept();
        int close();
        int recv();
        int creactSocket();
    private:
        unsigned server_port;
        long buff_size;
        struct sockaddr_in serverAddr;
        int sockServer ;
        void die(const char* s) {perror(s); exit(1);}

};
#endif