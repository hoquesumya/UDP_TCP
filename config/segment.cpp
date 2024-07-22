#include "segment.h"
#include <bit>



Segment::Segment(uint16_t cl_port, uint16_t des_port, int seq, int a, uint16_t rwn, 
          uint8_t h_f,  uint8_t f_f, uint16_t ch, void * data){
            client_port = cl_port;
            destination_port = des_port;
            this->seq = seq;
            _ack_ = a;
            this -> rwnd = rwn;
            header_field = h_f;
            flag_field = f_f;
            checksum = ch;
            this -> data = (char*)data;
            std::cout<< "hello\n"<<client_port << std::endl;
          }
void Segment::create_segment(Playload *pt){
  
    create_checksum();
    
    pt -> cl_port = client_port;
    pt -> des_port = destination_port;
    pt->seq_ = seq;
    pt -> data_ = data;
    pt ->_ack = _ack_;
    pt ->rwnd_ = rwnd;
    pt->header_field_= header_field;
    pt -> checksum_ = checksum;
    pt->flag_field_ = flag_field;
}
void Segment::extract_segment(){
    
}
Segment::~Segment(){

}
void Segment::create_checksum(){
   uint16_t seq_ = (uint16_t)seq;
   uint16_t a = (uint16_t)_ack_;
   uint16_t header =(u_int16_t) header_field;
   uint16_t fl = (uint16_t) flag_field;


   uint16_t sum = 0;
   sum += seq_ + a + header + fl + checksum + client_port + destination_port + rwnd;

   while (*data){
      
      if (*(data + 1)){
        uint16_t word = (*data << 8) + *(data + 1);
        sum += word;
      }
      else{
         uint16_t word = (*data << 8);
         sum += word;
         data += 1;
         continue;
      }
      data += 2;
   }
   std::cout<<"sum"<<sum<<std::endl;
   sum = (sum & 0xFFFF) + (sum >> 16);
   sum = (sum & 0xFFFF) + (sum >> 16);
   checksum = ~sum;

}