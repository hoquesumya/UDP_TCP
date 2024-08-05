#include <iostream>
#include "mrt_client.h"
#include <errno.h>
#include <fstream>

std::string read_data(const char *);

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
   const char * file = "data.txt";
   std::string res = read_data(file);
   Client cl(client_port, server_port, server_addr, segment_size);
   std::cout << res.length() <<std::endl;
   cl.send(res, res.length());

   return 0; 


}
std::string read_data(const char* filename){
    std::ifstream ifs {filename};
    if(!ifs){
        if (filename)
            throw std::runtime_error{"can't open file: "};
        else
            throw std::runtime_error{"no file name"};
    }
    std::string str;
    std::string res;
    while(getline(ifs, str)){
        res += str;
    }
    return res;


}
