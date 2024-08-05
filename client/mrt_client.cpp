#include <cstring>
#include "mrt_client.h"
#include <algorithm>
Client::Client(unsigned short c_port, unsigned short s_port, char* s_addr, long s_size){
    client_port = htons(client_port);
    server_port = htons(s_port);
    server_ip = s_addr;
    segment_size = s_size;
    data_segment = segment_size - 20;

    if ((sockFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        die("socket failed");
    
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = server_port;
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    
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
            /*char temp_data[data_segment];
            
            if (i < data_segment){
                max_len = data_segment;
            }
            else if (i % data_segment == 0 and len - i > data_segment){
                max_len = i + data_segment;
            }
            else{
                max_len = len;
            }
            std::cout<<"max_len"<<max_len<<" "<<i<<std::endl;
            size_t j = 0;

            for(; i < max_len; i++){
                temp_data[j++] = data[i];
            }*/
            max_len = std::min(len - i, static_cast<size_t>(data_segment));
            std::string temp_data = data.substr(i, max_len);
            q.push(temp_data);
            //std::cout<<temp_data<<" "<<std::endl;
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

int Client::close(){
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
   
    pthread_mutex_lock(&thread_cl.mutex);
    std::cout<<thread_cl.q.size()<<std::endl;
    
    while (thread_cl.q.size() == 0){
        std::cout<<"waiting"<<std::endl;
        pthread_cond_wait(&thread_cl.cond, &thread_cl.mutex);
    }
    while (thread_cl.q.size()> 0){

    
        std::string data = thread_cl.q.front();
        Segment s(thread_cl.client_port, thread_cl.server_port, 1, 1, 0, 5, 0b101111, 0, data);
        Playload pt;
        s.create_segment(&pt);
        //std::cout<<pt.data_<<std::endl;
        //std::cout<<pt.flag_field_<<std::endl;
        sendto(thread_cl.sockFD, &pt, sizeof(Playload), 
                0, (const struct sockaddr *) &thread_cl.servaddr,  
                    sizeof(thread_cl.servaddr)); 
        //std::cout << pt.checksum_<<std::endl;
        thread_cl.q.pop();
        pthread_mutex_unlock(&thread_cl.mutex);
    }
    return nullptr;

}
void* thread_recv(void * arg){
    return thread_recv_cpp(arg);

}


        
