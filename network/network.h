#include <iostream>
#include <string.h>
#include <ctime>
#include <chrono>
#include <map>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <vector>
#ifndef __NETWORK_H__
#define __NETWORK_H__
class Network{
    private:
        std::string lossfile;
        std::map<int, std::vector<int>> loss;
        struct sockaddr_in servaddr, cliaddr; 
        struct sockaddr_in main_server;
        
        void die(const char* s) {perror(s); exit(1);}
        unsigned int network_port, clnt_sock;
        int sockServer ;
        char* server_ip;
        unsigned int server_port;


    public:
        Network(unsigned int network_port,char* serverAddr, unsigned int server_port, std::string lossfile );
        ~Network();
        void createSocket();
        bool setLossflie();
        std::vector<int> getLossFile(std::time_t st);
        void handleMessage();





};
#endif
