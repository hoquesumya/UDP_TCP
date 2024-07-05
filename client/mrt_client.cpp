#include <cstring>
#include <iostream>

#include "mrt_client.h"
template <typename T>
Client<T>::Client(unsigned short c_port, unsigned short s_port, char* s_addr, long s_size){
    client_port = htons(client_port);
    server_port = htons(s_port);
    server_ip = s_addr;
    segment_size = s_size;

    if ((sockFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        die("socket failed");
    
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = server_port;
    servaddr.sin_addr.s_addr = inet_addr(server_ip);

}
template <typename T>
Client<T>::~Client(){

}
template <typename T>
int Client<T>::connect(){
    return 1;
}
template <typename T>
int Client<T>::send(T* data){

    sendto(sockFD, data->c_str(), data->length(), 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
    
    int n = 0;
    return  n;
}
template <typename T>
int Client<T>::close(){
    /*
        request to close the connection with the server
        blocking until the connection is closed
    */
   return 1;
}

        
