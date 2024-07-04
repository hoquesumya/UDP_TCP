#include<iostream>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#ifndef __MRT_CLIENT_H__
#define __MRT_CLIENT_H__
template <typename T>
class Client{
    public:
        //contructor
        Client(unsigned short c_port, unsigned short s_port, char* s_addr,long s_size);
        
        //destructor
        ~Client();
        
        //Copy constructor
        Client(const Client& orig);

        int connect();

        int send(T* data);

        int close();

    private:
        unsigned short client_port;
        unsigned short server_port;
        char* server_ip;
        long segment_size; //maximum segment size including the TCP headers
        struct  sockaddr_in     servaddr;
        int sockFD;
        void die(const char* s) {perror(s); exit(1);}
};

#endif