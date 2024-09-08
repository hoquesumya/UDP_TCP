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
void Network::handle_client(const std::chrono::time_point
 <std::chrono::high_resolution_clock> start_time){
    
    Playload buffer; 
        
    socklen_t len = sizeof(cliaddr);
    
    while(true){
        //recv from client
        int n = recvfrom(sockServer, &buffer, sizeof(Playload), MSG_WAITALL, 
        ( struct sockaddr *)&(this -> cliaddr),&len);

        auto res = getLossFile(start_time);
        double lastPktLoss = res.first;
        double lastBitError = res.second;

        std::random_device rand;  
         std::mt19937 gen(rand());  
         std::uniform_int_distribution<>dis(0.0, 1.0); 
         float temp = dis(gen);
         if (temp <= lastPktLoss){
            std::cout<<"packet loss occurrs client and not sending: "<<buffer.seq_<<std::endl;
            continue;
         }
        //send to server
        std::cout<<"sending seq: "<<buffer.seq_<<std::endl;
        if(sendto(this->clnt_sock, &buffer, sizeof(Playload),
            0, (const struct sockaddr *) &main_server,  
                sizeof(sockaddr_in)) == -1){
                    std::cout<<"error"<<std::endl;
                };
    }

}
void Network::handle_server(const std::chrono::time_point
 <std::chrono::high_resolution_clock> start_time){

 
    Playload buffer;
    socklen_t len_server = sizeof(main_server);
    
    while(true){

        int n_server = recvfrom(clnt_sock, &buffer, sizeof(Playload), MSG_WAITALL, 
        ( struct sockaddr *) &main_server, &len_server);

        /*auto res = getLossFile(start_time);
        double lastPktLoss = res.first;
        double lastBitError = res.second;

        std::random_device rand;  
        std::mt19937 gen(rand());  
        std::uniform_real_distribution<>dis(0.0, 1.0); 
        float temp = dis(gen);
       
        if (temp <= lastPktLoss){
            std::cout<<"packet loss occurrs server"<<std::endl;
            std::cout<<temp<<","<<lastPktLoss<<std::endl;
            continue;
        }*/
        //send to client
        if(sendto(this->sockServer, &buffer, sizeof(Playload),
            0, (const struct sockaddr *) &cliaddr,  
                sizeof(sockaddr_in)) == -1){
                    std::cout<<"error"<<std::endl;
                };
    }


}
void Network::set_up_loss_file(const char * file){
    std::ifstream ifs { file };
    if (!ifs) {
        if (file)
            throw std::runtime_error{"can't open file: " + std::string(file)};
        else
            throw std::runtime_error{"no file name"};
    } 
    int integer;
    float value1, value2;  
     while (ifs >> integer >> value1 >> value2) {
        // Process or display the data
        loss[integer].push_back(value1);
        loss[integer].push_back(value2);
        std::cout << "Integer: " << integer << ", Value1: " << value1 << ", Value2: " << value2 << std::endl;
    }

    ifs.close(); 
}
 std::pair<float, float> Network::getLossFile(std::chrono::time_point
 <std::chrono::high_resolution_clock> start_time) const{
    sleep(1);
      auto current_time = std::chrono::high_resolution_clock::now();
    // Calculate elapsed time in seconds
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

    double lastPktLoss = 0;
    double lastBitError = 0;

    // Find the loss data based on elapsed time''
    std::cout<<"elapsed time:"<<elapsed_seconds<<std::endl;
    for (const auto& entry : loss) {
        if (elapsed_seconds > entry.first) {
            lastPktLoss = entry.second[0];
           // std::cout<<"pakcet loss get"<<lastPktLoss<<std::endl;
            lastBitError = entry.second[1];
        }
    }

    return {lastPktLoss, lastBitError};


 }

void Network::handleMessage(){
    /**we need the lembda*/
    auto start = std::chrono::high_resolution_clock::now();
       std::thread t1([this, start]() {
            handle_client(start);
        });
     std::thread t2([this, start]() {
            handle_server(start);
        });

   
    t1.join();
    t2.join();

}
Network::~Network(){
    std::cout<<"destroying"<<std::endl;
}