#include "network.h"
#include <errno.h>

Network::Network(unsigned int network_port,char* serverAddr, unsigned int server_port, std::string lossfile){

            this -> network_port = htons(network_port);
            this -> server_ip = serverAddr;
            this -> server_port = htons(server_port);
            this -> lossfile = lossfile;
            memset(&(this-> servaddr), 0 , sizeof(this->servaddr));
            memset(&(this -> cliaddr), 0, sizeof(this->cliaddr));
            memset(&(this -> main_server), 0, sizeof(this->main_server));
           
}

void Network::createSocket(){
    /*first part will act as a server*/

    if ((this -> sockServer = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
                     die("socket failed");
   
    this -> servaddr.sin_family = AF_INET;
    this -> servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    this -> servaddr.sin_port = this -> network_port;
    if (bind(this -> sockServer, (struct sockaddr *) &(this-> servaddr), sizeof(this -> servaddr)) < 0){
        int err = errno;
        std::cerr << "Bind failed: " << strerror(err) << " (errno: " << err << ")" << std::endl;
         die("bind failed");
    }

    //start filing up main server struct
    if((this->clnt_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        die("client - scoket failed");
    this->main_server.sin_family = AF_INET; 
    this->main_server.sin_port = server_port;
    this->main_server.sin_addr.s_addr = inet_addr(this->server_ip);

    std::cout<< "printing"<<main_server.sin_addr.s_addr<<std::endl;
   
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
void Network::handle_client(){
    while(true){
        
        Playload buffer; 
    
        socklen_t len = sizeof(cliaddr);
        //recv from client
        int n = recvfrom(sockServer, &buffer, sizeof(Playload), MSG_WAITALL, 
        ( struct sockaddr *)&(this -> cliaddr),&len);
        std::cout<<"data is"<<buffer.data_<<std::endl;
        //send to server
        if(sendto(this->clnt_sock, &buffer, sizeof(Playload),
            0, (const struct sockaddr *) &main_server,  
                sizeof(sockaddr_in)) == -1){
                    std::cout<<"error"<<std::endl;
                };
    }

}
void Network::handle_server(){
    Playload buffer;
        socklen_t len_server = sizeof(main_server);
    while(true){
        int n_server = recvfrom(clnt_sock, &buffer, sizeof(Playload), MSG_WAITALL, 
        ( struct sockaddr *) &main_server, &len_server);
        //send to client
        if(sendto(this->sockServer, &buffer, sizeof(Playload),
            0, (const struct sockaddr *) &cliaddr,  
                sizeof(sockaddr_in)) == -1){
                    std::cout<<"error"<<std::endl;
                };
    }


}

void Network::handleMessage(){
    /**we need the lembda*/
    std::thread t1{[this](){handle_client();}};
    std::thread t2{[this](){handle_server();}};
   
    t1.join();
    t2.join();

}
Network::~Network(){
    std::cout<<"destroying"<<std::endl;
}