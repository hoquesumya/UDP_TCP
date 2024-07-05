#include<iostream>
#include "mrt_server.h"


int main(int argc, char* argv[]){

 if (argc != 3){
        std::cerr << "Incorrect Usage:" <<argv[0]<<" <server_port> buffer size" << std::endl;   
        exit(1);
    }

    unsigned short server_port = atoi(argv[1]);
    long buff_size = atoi(argv[2]);

    if (server_port< 49151 || server_port > 65535){
        perror("Port is out of range . Rnage is 49151 - 65535");
        exit(1);
    }
    Server s(server_port, buff_size);
 
    s.recv();
    
} 
