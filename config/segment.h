#include <iostream>
#include <cstdint>
#include <bitset>

#ifndef __SEGMENT_H__
#define __SEGMENT_H__

 struct Playload {
        uint16_t cl_port;
        uint16_t des_port;
        int seq_;
        int _ack;
        uint16_t rwnd_;
        uint8_t header_field_ : 4;
        uint8_t flag_field_ : 6;
        uint16_t checksum_;
        char data_[4096];
        long mss_;
    };
class Segment{
    public:
         Segment(uint16_t cl_port, uint16_t des_port, int seq, int a, uint16_t rwn, 
          uint8_t h_f,  uint8_t f_f, uint16_t ch, std::string, long mss);
        ~Segment();
        void create_segment(Playload *pt);
        bool extract_segment(Playload *pt);
        
    private:
        uint16_t client_port;
        uint16_t destination_port;
        int seq;
        int _ack_;
        uint16_t rwnd;
        uint8_t header_field : 4;
        uint8_t flag_field : 6;
        uint16_t checksum;
        char* data = nullptr;
        uint16_t create_checksum(uint16_t seq_, uint16_t a, 
            uint16_t header,  uint16_t fl, uint16_t cl_port, uint16_t des_port, 
            uint16_t check,uint16_t r, char * buf, uint16_t mss);
        long mss;
};

#endif