#include <cstring>
#include <iostream>

#include "mrt_client.h"

Client::Client(unsigned short c_port, unsigned short s_port, char* s_addr,long s_size){
    client_port = c_port;
    server_port = s_port;
    server_ip = s_addr;
    segment_size = s_size;

}

Client::~Client(){

}

int Client::connect(){
    return 1;
}
int Client::send(char* data){
    int n = 0;
    return  n;
}

int Client::close(){
    /*
        request to close the connection with the server
        blocking until the connection is closed
    */
   return 1;

}

        