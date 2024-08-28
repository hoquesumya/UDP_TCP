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
    //get the receive window
    this->rwnd = buff_size;


    if (bind(this->sock, (struct sockaddr*)&(this->serverAddr), sizeof(this->serverAddr)) < 0) {
            perror("bind failed");

    }
    if (pthread_cond_init(&cond_recv, NULL) != 0)
        die("Cannot initialize condition variable");

    if (pthread_mutex_init(&mutex_recv, NULL) != 0) {
        pthread_cond_destroy(&cond_recv);
        die("Cannot initialize queue mutex");
    }
      if (pthread_cond_init(&cond_data, NULL) != 0)
        die("Cannot initialize condition variable");
     
    if (pthread_mutex_init(&mutex_data, NULL) != 0) {
        pthread_cond_destroy(&cond_data);
        die("Cannot initialize queue mutex");
    }
    if (pthread_cond_init(&cond_established, NULL) != 0)
        die("Cannot initialize condition variable");


    if (pthread_mutex_init(&mutex_established, NULL) != 0) {
        pthread_cond_destroy(&cond_established);
        die("Cannot initialize queue mutex");
    }
    int rval = pthread_attr_init(&m_attr_recv);
    if (rval){
        die("attriute initialization has been falied");

    }
     int rval_send = pthread_attr_init(&m_attr_send);
    if (rval_send){
        die("attriute initialization has been falied");

    }
    SpawnThred();
}
/*the below three functions will handle the threadRoutines*/
void Server::SpawnThred(){
    int rCode = pthread_create(&recv_id, &m_attr_recv, thread_recv_main, this);
    if (rCode)
        {
            die("thrad-recv creation fail");
        }
    int rCode_send = pthread_create(&send_id, &m_attr_send, thread_send_main, this);
    if (rCode_send)
        {
            die("thrad creation fail");
        }
   // pthread_join(recv_id, nullptr);
  //  pthread_join(send_id, nullptr);
    

}
void* thread_recv_main(void * arg){
    std::cout<<"x"<<std::endl;
    return thread_recv_buffer(arg);

}
void* thread_recv_buffer(void* arg){
    /* this function will handle the socket recv and put into the buffer
    aka will act as the producer
    */
     if (arg == nullptr){
        std::cerr<<"Error in agumnent"<<std::endl;   
    }
    std::cout<<"strating recv thread"<<std::endl;
    Server &serv = *reinterpret_cast<Server *>(arg);
   
    Playload buffer;
    socklen_t len = sizeof(serv.clnaddr);

    while(true){
      
        int n = recvfrom(serv.sock, &buffer, sizeof(Playload) , MSG_WAITALL,
                        (struct sockaddr*)&(serv.clnaddr), &len);
        if (n < 0) {
            perror("recvfrom failed");
        }

        pthread_mutex_lock(&serv.mutex_recv);
        
        bool is_empty = serv.shared_recv.empty();
        /*lock the recv buffer */
       
        Playload temp_buffer = buffer;
        serv.shared_recv.push_back(temp_buffer);
        std::cout<<"from receive: "<< serv.shared_recv.size()<<std::endl;
        
        if(is_empty){
            pthread_cond_signal(&serv.cond_recv);
        }
        pthread_mutex_unlock(&serv.mutex_recv);
         
    }

}
void* thread_send_main(void * arg){
    return thread_send_data(arg);
}
void* thread_send_data(void* arg){
    Server &serv = *reinterpret_cast<Server *>(arg);
    Playload buf;
    bool established = false;
    bool partial_establised = false;
    auto it = serv.shared_recv.end();
    while(1){
        pthread_mutex_lock(&serv.mutex_recv);
        while (serv.shared_recv.size() ==0){
             it = serv.shared_recv.begin();
            pthread_cond_wait(&serv.cond_recv,&serv.mutex_recv);
        } 
    
       if (!partial_establised && !established){
            std::cout<<"reciving connection"<<std::endl;
         // server_port, des_port, seq, ack, rwnd, head_flg, flag, checksum, data
            buf = serv.shared_recv.front();
            serv.shared_recv.pop_front();
            it =  serv.shared_recv.begin();

            pthread_mutex_unlock(&serv.mutex_recv);
            serv.mss = buf.mss_;
            serv.max_len_data_buf = serv.data_buffer / serv.mss;
            serv.max_recv_size = serv.buff_size / serv.mss;
           // std::cout<<"data len"<< serv.max_len_data_buf <<std::endl;
            Segment seg((uint16_t)serv.server_port, buf.des_port, 1, 
            buf.seq_ + 1, serv.max_recv_size, 5 , 0b010010, 0, "", 0);
            /*
            to - do here we will examine the buf then create the segment
            */
           seg.extract_segment(&buf);
           Playload buf_;
            seg.create_segment(&buf_);
            if(sendto(serv.sock, &buf_, sizeof(Playload),
                    0, (const struct sockaddr *) &serv.clnaddr,  
                        sizeof(sockaddr_in)) == -1)
            {
                 std::cout<<"error"<<std::endl;
            }
    
            partial_establised = true;

            continue;
        }
        else if(partial_establised && !established){
           // std::cout<<"almost_done"<<std::endl;
            buf = serv.shared_recv.front();
            serv.shared_recv.pop_front();
             it =  serv.shared_recv.begin();
            pthread_mutex_unlock(&serv.mutex_recv);
         
            Segment seg((uint16_t)serv.server_port, buf.des_port, 1, 
            buf.seq_ + 1, 0, 5 , 0b010000, 0, "", 0);
            seg.extract_segment(&buf);
            if(buf.flag_field_ == 0b010000){
                established = true;
                partial_establised = false;
                pthread_mutex_lock(&serv.mutex_established);
                serv.established = true;
                pthread_cond_signal(&serv.cond_established);
                pthread_mutex_unlock(&serv.mutex_established); 
           }
           continue;
             
        }
       // std::cout<<"applying main data section "<<std::endl;
       
        pthread_mutex_unlock(&serv.mutex_recv);
        /*getting the total reamining size of the data buffer*/
        int n = 0;
        pthread_mutex_lock(&serv.mutex_data);
        n = serv.max_len_data_buf - serv.shared_data.size();
       
      //  std::cout<< "data buffer size is "<< n <<std::endl;
        
       pthread_mutex_unlock(&serv.mutex_data);
        /* 
        we can skip putting the data to the recv buffer if thereis no space 
        in the data buffer. then recieve the window size will shrink.
        then when is avaiable the recive window size will increase,
        othewise the window size be same

        */
        Playload _recv_buf;
        pthread_mutex_lock(&serv.mutex_recv);
            if (n > 0){
               // std::cout<<"only one data"<<std::endl;
                it = serv.shared_recv.begin();
            }
             buf = serv.shared_recv.front();
             std::cout<<buf.data_<<std::endl;

            //if (it != serv.shared_recv.end())
         //   std::cout<<"dereferencing: "<<serv.shared_recv.size() <<std::endl;
            _recv_buf = *it;
            std::cout<<_recv_buf.data_<<std::endl;

            if(n > 0){
                serv.shared_recv.pop_front();
              //  std::cout<<"current size is: "<<serv.shared_recv.size();
                it = serv.shared_recv.begin();

            }
            //send the  ack but with the recv window less tha
        
            
            int remain = serv.max_recv_size - serv.shared_recv.size();
          //  std::cout<<"window size is: " << remain<<std::endl;

       // pthread_mutex_unlock(&serv.mutex_recv);
        
        long latest_ack = _recv_buf.seq_ + strlen(_recv_buf.data_) + 1;
        std::cout<<"the lastest ack is: "<< latest_ack<<std::endl;
        Segment seg((uint16_t)serv.server_port, _recv_buf.des_port, 0, 
            latest_ack, remain, 5 , 0b001000, 0, "", 0);
        seg.extract_segment(&_recv_buf);
        Playload temp_buf;
        seg.create_segment(&temp_buf);
        /*
        send to the client the buf
        */
        if(sendto(serv.sock, &temp_buf, sizeof(Playload),
                    0, (const struct sockaddr *) &serv.clnaddr,  
                        sizeof(sockaddr_in)) == -1)
                    std::cout<<"error"<<std::endl;
        
        
           

        /*this part of the code will execute if there is there 
        is space otherwise continue*/
        if(n <= 0){
           // std::cout<<"wait the data buffer is full"<<std::endl;
             
           // std::cout<<"more than one data"<<std::endl;

            auto last = serv.shared_recv.end() - 1;
            if (it != last){
                std::cout<<"incrementing it"<<std::endl;
                ++it;
            }
        
           // std::cout<<"END data!!"<< (*(serv.shared_recv.end()-1)).data_<<std::endl;
            pthread_mutex_unlock(&serv.mutex_recv);
            continue;

        }
        pthread_mutex_unlock(&serv.mutex_recv);

        pthread_mutex_lock(&serv.mutex_data);

    //    std::cout<<"main data into the buffer"<< std::endl;
        bool is_emp = serv.shared_data.empty();
        serv.shared_data.push_back(std::string(buf.data_));
       // std::cout<<serv.shared_data.front()<<std::endl;
        if(is_emp){
            pthread_cond_signal(&serv.cond_data);
        }
        pthread_mutex_unlock(&serv.mutex_data);
 
    }
}

int Server::accept(){
   
    pthread_mutex_lock(&mutex_established);
    while(established == false){
       pthread_cond_wait(&cond_established, &mutex_established);
    }
    pthread_mutex_unlock(&mutex_established);
    return 1;
}
int Server::close(){
 /*needs to be implemented
 similar as accept
 
 */
}
int Server::recv(int size){
    size_t len = 0;
    int i = 0;
    while(1){
        pthread_mutex_lock(&mutex_data);
        while(shared_data.size() == 0){
            pthread_cond_wait(&cond_data, &mutex_data);
        }
       // sleep(10);
       /*if (i == 0){
            pthread_mutex_unlock(&mutex_data);
            std::cout<<"sleeping"<<std::endl;
            sleep(4);
            pthread_mutex_lock(&mutex_data);
            std::cout<<" done sleeping"<<std::endl;
        }*/
        std::string data = shared_data.front();
        shared_data.pop_front();
        
        pthread_mutex_unlock(&mutex_data);
        len += data.length();
        std::cout<<data<<" ..... " <<len <<std::endl;
        i ++;
        if(len == size){
            std::cout<<"len is : "<<len<<std::endl;
            break;
        }
    }
    //pthread_mutex_unlock(&mutex_data);
    return len;

}
Server::~Server(){
    /*needs to be implemented*/
    pthread_cond_destroy(&cond_recv);
    pthread_cond_destroy(&cond_data);
    pthread_cond_destroy(&cond_established);

    pthread_mutex_destroy(&mutex_data);
    pthread_mutex_destroy(&mutex_recv);
    pthread_mutex_destroy(&mutex_established);
}
