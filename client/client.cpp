#include <iostream>
#include "mrt_client.h"
#include <errno.h>


int main(int argc, char** argv){
    if (argc != 5){
        std::cerr << "Incorrect Usage:" <<argv[0]<<" <client_port> <server_addr> <server_port> <segment_size>" << std::endl;   
        exit(1);
    }
    unsigned short client_port  = atoi(argv[1]);
    char* server_addr = argv[2];
    unsigned short server_port = atoi(argv[3]);
    long int segment_size = atoi(argv[4]);

    if (client_port < 49151 || client_port > 65535){
        perror("Port is out of range . Rnage is 49151 - 65535");
        exit(1);
    }
   Client cl(client_port, server_port, server_addr, segment_size);
   std::string h = "hello";
   sleep(10);
   cl.send(&h, h.length());

   return 0; 


}
