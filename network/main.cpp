#include <iostream>
#include "network.h"

namespace handleMsg{
   void handleMessage(int netSocket){

   }
}

int main(int argc, char* argv[]){

 if (argc != 5){
        std::cerr << "Incorrect Usage:" <<argv[0]<<" <Network Port> <server_addr> <server_port> <loss File>" << std::endl;   
        exit(1);
    }
    unsigned short net_port  = atoi(argv[1]);
    char* sevrer_addr = argv[2];
    unsigned short server_port = atoi(argv[3]);
    std::string lossF = argv[4];

    if (net_port< 49151 || net_port > 65535){
        perror("Port is out of range . Rnage is 49151 - 65535");
        exit(1);
    }
    Network net(net_port, sevrer_addr, server_port, "");
    net.createSocket();
    net.handleMessage();
   

}
