#include "mrt_server.h"

Server::Server(unsigned int port, long buff){

    this-> server_port = htons(port);
    this -> buff_size = buff;
    memset(&(this-> serverAddr), 0 , sizeof(this->serverAddr));
    memset(&(this-> clnaddr), 0 , sizeof(this->clnaddr));

    if ((sock =socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        die("socket failed");
    this -> serverAddr.sin_family = AF_INET;
    this -> serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    this->serverAddr.sin_port = this->server_port;

    if (bind(this->sock, (struct sockaddr*)&(this->serverAddr), sizeof(this->serverAddr)) < 0) {
            perror("bind failed");

    }
}
int Server::accept(){

}
int Server::close(){

}
void Server::recv(){

    while(true){
        Playload buffer; 
        socklen_t len = sizeof(this->clnaddr);
        int n = recvfrom(this->sock, &buffer, sizeof(Playload) , MSG_WAITALL,
                        (struct sockaddr*)&(this->clnaddr), &len);
        if (n < 0) {
            perror("recvfrom failed");
        }
        std::cout<<buffer.data_<< " "<< ntohs(buffer.checksum_)<<std::endl;
        Segment server_seg((uint16_t) 0, (uint16_t)server_port, 1, 1, 0, 5, 0b101111, 0, nullptr);
        server_seg.extract_segment(&buffer);

    }

}
Server::~Server(){
    
}
