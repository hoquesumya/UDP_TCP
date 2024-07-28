#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <iostream>
#include "../config/segment.h"
#ifndef __SERVER_H__
#define __SERVER_H__
class Server{
    public:
        Server(unsigned int port, long buf);
        int accept();
        int close();
        void recv();
        ~Server();
    private:
        unsigned int server_port;
        long buff_size;
        struct sockaddr_in serverAddr, clnaddr;
        int sock ;
        void die(const char* s) {perror(s); exit(1);}

};
#endif