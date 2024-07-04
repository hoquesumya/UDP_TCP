#include "mrt_server.h"

Server::Server(unsigned int port, long buff){

    this-> server_port = port;
    this -> buff_size = buff;
    

}
int Server::creactSocket (){
    if ((this -> sockServer = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                     die("socket failed");
    memset(&(this-> serverAddr), 0 , sizeof(this->serverAddr));
    this -> serverAddr.sin_family = AF_INET;
    this -> serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    this -> serverAddr.sin_port = this -> server_port;
    if (bind(this -> sockServer, (struct sockaddr *) &(this-> serverAddr), sizeof(this -> serverAddr)) < 0){
        int err = errno;
        std::cerr << "Bind failed: " << strerror(err) << " (errno: " << err << ")" << std::endl;
         die("bind failed");
    }
    return this->sockServer;

}
int Server::accept(){

}
int Server::close(){

}
int Server::recv(){
    

}