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
#include<thread>
#include <fstream>
#include <stdexcept>
#include<unordered_map>
#include<random>
#ifndef __NETWORK_H__
#define __NETWORK_H__
class Network{
    private:
        std::string lossfile;
        struct sockaddr_in servaddr, cliaddr; 
        struct sockaddr_in main_server;
        
        void die(const char* s) {perror(s); exit(1);}
        unsigned int network_port, clnt_sock;
        int sockServer ;
        char* server_ip;
        unsigned int server_port;
        std::unordered_map<int,std::vector<float>>loss;
       
        struct Playload {
            uint16_t cl_port;
            uint16_t des_port;
            int seq_;
            int _ack;
            uint16_t rwnd_;
            uint8_t header_field_ : 4;
            uint8_t flag_field_ : 6;
            uint16_t checksum_;
            char data_[4096];
            long mss;
            
    };
      std::pair<float, float> getLossFile(std::chrono::time_point<std::chrono::high_resolution_clock> start_time) const;

    public:
        Network(unsigned int network_port,char* serverAddr, unsigned int server_port, std::string lossfile );
        ~Network();
        void createSocket();
        bool setLossflie();
        std::vector<int> getLossFile(std::time_t st);
        void handleMessage();
        void handle_client(const std::chrono::time_point
 <std::chrono::high_resolution_clock> start_time);
        void handle_server(const std::chrono::time_point
 <std::chrono::high_resolution_clock> start_time);
        void set_up_loss_file(const char *);

};
#endif
