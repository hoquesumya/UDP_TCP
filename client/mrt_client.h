#ifndef __MRT_CLIENT_H__
#define __MRT_CLIENT_H__

class Client{
    public:
        //contructor
        Client(unsigned short c_port, unsigned short s_port, char* s_addr,long s_size);
        
        //destructor
        ~Client();
        
        //Copy constructor
        Client(const Client& orig);

        int connect();

        int send(char *data);

        int close();

    private:
        unsigned short client_port;
        unsigned short server_port;
        char* server_ip;
        long segment_size;
};

#endif;