#include <iostream>


int main(int argc, char** argv){
    if (argc != 4){
        std::cerr << "Incorrect Usage:" <<argv[0]<<" <client_port> <server_addr> <server_port> <segment_size>" << std::endl;   
        exit(1);
    }
    unsigned short client_port  = atoi(argv[1]);
    char* sever_addr = argv[2];
    unsigned short server_port = atoi(argv[3]);

    return 0;

}
