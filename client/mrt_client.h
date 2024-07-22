

#ifndef __MRT_CLIENT_H__
#define __MRT_CLIENT_H__


#include<iostream>
#include <pthread.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "../config/segment.h"
#include <queue>
extern "C"{
    void* thread_recv(void* p);
   // void * theard_send(void * p);
}
void * thread_recv_cpp(void * p);

class Client{
    public:
        //contructor
        Client(unsigned short c_port, unsigned short s_port, char* s_addr,long s_size);
        
        //destructor
        ~Client();

        int connect();

        int send(void* data, size_t len);

        int close();

        

    private:
        unsigned short client_port;
        unsigned short server_port;

        std::queue<void *>q;
        char* server_ip;
        long segment_size; //maximum segment size including the TCP headers
        int data_segment;
        struct  sockaddr_in     servaddr;
        int sockFD;

        void die(const char* s) {perror(s); exit(1);};
        friend void* thread_recv_cpp(void* p);
        pthread_attr_t m_attr;
        void SpawnThred();
        pthread_t id;
        pthread_mutex_t mutex; // A mutex used to protect the queue itself
        pthread_cond_t cond;   // A condition variable for threads to sleep on
    
};

#endif