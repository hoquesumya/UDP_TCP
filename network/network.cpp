#include "network.h"
#include <errno.h>

Network::Network(unsigned int network_port,std::string serverAddr, unsigned int server_port, std::string lossfile){

            this -> network_port = htons(network_port);
            this -> serverAddr = serverAddr;
            this -> server_port = htons(server_port);
            this -> lossfile = lossfile;
           
}
//for acting as a server
int Network::createSocket(){
    if ((this -> sockServer = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                     die("socket failed");
    memset(&(this-> servaddr), 0 , sizeof(this->servaddr));
    this -> servaddr.sin_family = AF_INET;
    this -> servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    this -> servaddr.sin_port = this -> server_port;
    if (bind(this -> sockServer, (struct sockaddr *) &(this-> servaddr), sizeof(this -> servaddr)) < 0){
        int err = errno;
        std::cerr << "Bind failed: " << strerror(err) << " (errno: " << err << ")" << std::endl;
         die("bind failed");
    }
    return this->sockServer;

}

/*
send to the client  as a server:
 memset(&cliaddr, 0, sizeof(cliaddr));

 n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                &len); 
    buffer[n] = '\0'; 
    printf("Client : %s\n", buffer); 
    sendto(sockfd, (const char *)hello, strlen(hello),  
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
            len); 

send to the main_server as a client


memset(&servaddr, 0, sizeof(servaddr)); 
       
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

    sendto(sockfd, (const char *)hello, strlen(hello), 
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
    std::cout<<"Hello message sent."<<std::endl; 
           
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len); 
*/
Network::~Network(){
    std::cout<<"destroying"<<std::endl;
}