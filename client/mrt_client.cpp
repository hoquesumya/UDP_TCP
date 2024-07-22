#include <cstring>
#include "mrt_client.h"
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

int Client::send(void* data, size_t len){

    pthread_mutex_lock(&mutex);

    bool was_empty = q.empty();

    if (len <= data_segment){
        q.push(data);
    }
    if (was_empty){
        pthread_cond_broadcast(&cond);
    }
   
  pthread_mutex_unlock(&mutex);
    //q.push(data)
    sendto(sockFD, data, len, 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
    
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
   void* data = thread_cl.q.front();
   Segment s(thread_cl.client_port, thread_cl.server_port, 1, 1, 0, 5, 0b101111, 0, data);
   Playload pt;
   s.create_segment(&pt);
   std::cout << pt.checksum_<<std::endl;
   thread_cl.q.pop();

     pthread_mutex_unlock(&thread_cl.mutex);
    return nullptr;

}
void* thread_recv(void * arg){
    return thread_recv_cpp(arg);

}


        
