#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <iostream>
#include<deque>
#include<thread>
#include<mutex>
#include <pthread.h>
#include "../config/segment.h"
#ifndef __SERVER_H__
#define __SERVER_H__
extern "C"{
    void* thread_recv_main(void* p);
    void* thread_send_main(void* p);
   // void * theard_send(void * p);
}
void* thread_recv_buffer(void* arg);
void* thread_send_data(void* arg);
class Server{
    public:
        Server(unsigned int port, long buf);
        int accept();
        int close();
        int recv(int size);
        ~Server();
    private:
        unsigned int server_port;
        long buff_size;
        bool established = false;
        struct sockaddr_in serverAddr, clnaddr;
        int sock ;
        long mss = 0;
        long data_buffer = 4000L;
        int max_len_data_buf = 0;
        long rwnd; //maximum segment size
        int max_recv_size;
        std::deque<Playload>shared_recv;
        std::deque<std::string>shared_data;
        pthread_mutex_t mutex_recv; // A mutex used to protect the queue itself
        pthread_mutex_t mutex_data;
        pthread_mutex_t mutex_established;
        /*condition varaible for recv and data buffer*/
        pthread_cond_t cond_recv; 
        pthread_cond_t cond_data; 
        pthread_cond_t cond_established;
        pthread_t recv_id;
        pthread_t send_id;
        pthread_attr_t m_attr_recv;
        pthread_attr_t m_attr_send;
       
        
        void die(const char* s) {perror(s); exit(1);}
        void SpawnThred();
        friend void* thread_recv_buffer(void* arg);
        friend void* thread_send_data(void * arg);

};
#endif