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
            int len = strlen((char*) data);
            this -> data = new char[len + 1];
            strcpy(this->data, (char *) data);
          }
void Segment::create_segment(Playload *pt){
  
    create_checksum();
    
    pt -> cl_port = client_port;
    pt -> des_port = destination_port;
    pt->seq_ = seq;
    std::strcpy(pt->data_, data);
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
   char temp [strlen(data) + 1];
   strcpy(temp, data);
   int i = 0;
   int len = strlen(temp);
   while (i < len){
      std::cout<<temp[i]<<std::endl;
      
      if ( i + 1 < len){
        uint16_t word = (temp[i] << 8) + temp[i + 1]; //make a space for 16 bit 
        sum += word;
      }
      else{
         uint16_t word = temp[i]<< 8;
         sum += word;
         i += 1;
         continue;
      }
      i += 2;
      
   }
   std::cout<<"sum"<<sum<<std::endl;
   sum = (sum & 0xFFFF) + (sum >> 16);
   sum = (sum & 0xFFFF) + (sum >> 16);
   checksum = ~sum;


}