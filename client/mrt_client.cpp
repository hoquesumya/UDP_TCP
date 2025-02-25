#include <cstring>
#include "mrt_client.h"
#include <algorithm>
#include <random>
#include <chrono>
Client::Client(unsigned short c_port, unsigned short s_port, char* s_addr, long s_size){
    client_port = htons(client_port);
    server_port = htons(s_port);
    server_ip = s_addr;
    segment_size = s_size;
    data_segment = segment_size - 28;

    if ((sockFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        die("socket failed");
    
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = server_port;
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    
    struct timeval timeout;
    timeout.tv_sec = 15;  // 5 seconds timeout
    timeout.tv_usec = 0;
   if (setsockopt(sockFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Error setting timeout\n";
        close(sockFD);
    }
    
    if (pthread_cond_init(&cond, NULL) != 0)
        die("Cannot initialize condition variable");

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        pthread_cond_destroy(&cond);
        die("Cannot initialize queue mutex");
    }
    int rval = pthread_attr_init(&m_attr);
    if (rval){
        die("attriute initialization has been falied");

    }
    
    SpawnThred();
}

Client::~Client(){

    pthread_join(id, nullptr);
}

int Client::connect(){
    std::random_device rand;  
    std::mt19937 gen(rand());  
    std::uniform_int_distribution<>dis(1, 10);   
    client_seq = dis(gen);
    std::cout<<"client seq is"<<client_seq<<std::endl;
    _ack_list.insert(client_seq);
   
    // server_port, des_port, seq, ack, rwnd, head_flg, flag, checksum, data
    Segment s(client_port, server_port, client_seq, 0, 0, 5, 0b000010, 0, "", data_segment);
    Playload pt;
   // std::cout<<" jjkkk "<<data_segment<<std::endl;
    s.create_segment(&pt);
    socklen_t len = sizeof(servaddr);

    
    sendto(sockFD, &pt, sizeof(Playload), 
                0, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr)); 
  
    
    while(true) {
        int n = recvfrom(sockFD, &pt, sizeof(Playload), MSG_WAITALL, 
        ( struct sockaddr *)&(this -> servaddr),&len);
        if (n < 0){
            if(errno == EAGAIN or errno == EWOULDBLOCK){
                /*we will resend the seq */
                Segment s(client_port, server_port, client_seq, 0, 0, 5, 0b000010, 0, "", data_segment);
            // std::cout<<" jjkkk "<<data_segment<<std::endl;
                s.create_segment(&pt);
                socklen_t len = sizeof(servaddr);

                
                sendto(sockFD, &pt, sizeof(Playload), 
                            0, (const struct sockaddr *) &servaddr,  
                                sizeof(servaddr)); 
                std::cout<<"damn packet loss"<<client_seq<<std::endl;
                continue;
            }
        }
        if(pt._ack == client_seq + 1)

        {
            std::cout<<"received data: "<<pt._ack <<std::endl;
             break;

        }
        
    }
    /*int n = recvfrom(sockFD, &pt, sizeof(Playload), MSG_WAITALL, 
        ( struct sockaddr *)&(this -> servaddr),&len);*/
    
    //final send on three way handshake connection
    // server_port, des_port, seq, ack, rwnd, head_flg, flag, checksum, data, data_segment
    client_seq = pt._ack;
    window= pt.rwnd_;
    _ack_list.insert(client_seq);
   
    Segment s1(client_port, server_port, client_seq, pt.seq_ + 1, 0, 5, 0b010000, 0, "", data_segment);
    s1.create_segment(&pt);
    sendto(sockFD, &pt, sizeof(Playload), 
                0, (const struct sockaddr *) &servaddr,  
                    sizeof(servaddr)); 
    std::cout<<"window c0nnection: "<<window<<", "<<(int)pt.flag_field_<<std::endl;
    client_seq += 1;
    _ack_list.insert(client_seq);

    return 1;
}

int Client::send(std::string data, size_t len){

    pthread_mutex_lock(&mutex);

    bool was_empty = q.empty();

    if (len <= data_segment){
        //char *temp = (char*)data.c_str();
        q.push(data);
    }
    else{
        size_t i = 0;
        size_t max_len =0;
        
        while (i < len){
            max_len = std::min(len - i, static_cast<size_t>(data_segment));
            std::string temp_data = data.substr(i, max_len);
            q.push(temp_data);
            i += max_len;
        }

    }
    if (was_empty){
        pthread_cond_broadcast(&cond);
    }
   
  pthread_mutex_unlock(&mutex);
    
    int n = 0;
    return  n;
}

int Client::Close(){
    /*
        request to close the connection with the server
        blocking until the connection is closed
    */
   return 1;
}

void Client::SpawnThred(){
    
    int rCode = pthread_create(&id, &m_attr, thread_recv, this);
    if (rCode)
        {
            die("thrad creation fail");
        }
    
}
void * thread_recv_cpp(void * arg){

    if (arg == nullptr){
        std::cerr<<"Error in agumnent"<<std::endl;
    }
    /*this is for class object that the friend function will use*/
    std::cout<<"thread routinw has been started"<<std::endl;
    Client *cl = (Client *) arg;
    Client &thread_cl = *cl;
    long expected_last_ack_recvd= 0;
    const int timeout = 6000;
   
    pthread_mutex_lock(&thread_cl.mutex);
    std::cout<<thread_cl.q.size()<<std::endl;
    
    while (thread_cl.q.size() == 0){
        std::cout<<"waiting"<<std::endl;
        pthread_cond_wait(&thread_cl.cond, &thread_cl.mutex);
    }
    bool initial_seq = false;
    while (thread_cl.q.size()> 0){
        std::vector<Playload>temp;
        std::cout<<"window"<<thread_cl.window <<std::endl;
       
        while(thread_cl.window == 0){
            Segment s(thread_cl.client_port, thread_cl.server_port, 
            -1, 1, 0, 5, 0b101111, 0, "", thread_cl.data_segment);
            Playload pt;
            s.create_segment(&pt);
            socklen_t len = sizeof(thread_cl.servaddr);
            sendto(thread_cl.sockFD, &pt, sizeof(Playload), 
                    0, (const struct sockaddr *) &thread_cl.servaddr,  
                        sizeof(thread_cl.servaddr)); 
            int n = recvfrom(thread_cl.sockFD, &pt, sizeof(Playload), MSG_WAITALL, 
               (struct sockaddr *)& thread_cl.servaddr,&len);
            thread_cl.window = pt.rwnd_;
            std::cout<<"my current window is: "<< thread_cl.window<<std::endl;

        }

        int num_seg = thread_cl.window;
        if (num_seg > thread_cl.q.size()){
            num_seg = thread_cl.q.size();
        }
        long l = 0;
       
        for (int i = 0; i < num_seg; i ++){
            std::string data = thread_cl.q.front();
           
            Segment s(thread_cl.client_port, thread_cl.server_port,
             thread_cl.client_seq, 1, 0, 5, 0b101111, 0, data, thread_cl.data_segment);
            Playload pt;
            s.create_segment(&pt);
            l += strlen(pt.data_);
            std::cout<<" isseq: "<<thread_cl.client_seq<<std::endl;
            sendto(thread_cl.sockFD, &pt, sizeof(Playload), 
                    0, (const struct sockaddr *) &thread_cl.servaddr,  
                        sizeof(thread_cl.servaddr)); 
            //std::cout << pt.checksum_<<std::endl;
            
            temp.push_back(pt);
            thread_cl.q.pop();
            thread_cl.client_seq += data.size() + 1;
            thread_cl._ack.push_back(thread_cl.client_seq);
        } 
        pthread_mutex_unlock(&thread_cl.mutex);
        expected_last_ack_recvd = thread_cl.client_seq;
        std::cout<<"expected ack will be: "<<expected_last_ack_recvd<<std::endl;
        socklen_t len = sizeof(thread_cl.servaddr);
        Playload buf;
      
       int count = 0;
       int track_seg = 0;
       int i = 0;
        while(true){
             int n = recvfrom(thread_cl.sockFD, &buf, sizeof(Playload), MSG_WAITALL, 
               (struct sockaddr *)& thread_cl.servaddr,&len);
               std::cout<<"server ack is: "<< buf._ack<< ", "<<buf.rwnd_<<std::endl;
                
                if (buf._ack != 0 and thread_cl._ack_list.find(buf._ack) == thread_cl._ack_list.end()){
                    /*condiering ack is missing from server
                    if the current expected ack is less than received ack
                    */
                    if(thread_cl._ack[i] < buf._ack){
                        std::cout<<"ops accumalted ack"<<std::endl;
                        /*here I am updating the ack index to matched with the current ack
                        so that I can advance the track_segment accordingly
                        */
                        while (thread_cl._ack[i] != buf._ack){
                            i++;
                            track_seg++;
                            count++;
                            thread_cl._ack_list.insert(thread_cl._ack[i]);
                        }
                        i++;
                        track_seg++;
                        count++;
                

                    }
                    else{
                        std::cout<<"ops no ack loss has been occurred"<<std::endl;
                        i++;
                        
                        thread_cl._ack_list.insert(buf._ack);
                        std::cout<<"entering the data"<<std::endl;
                        count += 1;
                        track_seg ++;
                    }
               }

                if (n < 0){
                    if(errno == EAGAIN or errno == EWOULDBLOCK){
                         std::cout<<"found packet loss "<<temp.size()<<std::endl;
                        if (count <= temp.size() - 1 &&
                        (thread_cl._ack_list.find(buf._ack) != thread_cl._ack_list.end()) or buf._ack == 0 )
                 /*send the rest to the segment*/
                        {
                            std::cout<<"track seg"<<track_seg<<std::endl;
                            for( int i = track_seg; i< temp.size(); i++){
                                sendto(thread_cl.sockFD, &temp[i], sizeof(Playload), 
                                    0, (const struct sockaddr *) &thread_cl.servaddr,  
                                    sizeof(thread_cl.servaddr)); 
                            }

                        }
                            
                        
                    }
                }
            
               if (buf._ack == expected_last_ack_recvd){
                    break;
               }
               
               

        }
    
        thread_cl.window = buf.rwnd_;
        /*clearing out the buffer*/
        for(const auto &it : temp){
            temp.pop_back();
        }
        for(const auto &it : thread_cl._ack){
            thread_cl._ack.pop_back();
        }
       pthread_mutex_lock(&thread_cl.mutex);
        
    }
    std::cout<<"damn I am done"<<std::endl;
    return nullptr;

}
void* thread_recv(void * arg){
    return thread_recv_cpp(arg);

}


        
