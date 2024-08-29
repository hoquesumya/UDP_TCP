#include "segment.h"
#include <bit>



Segment::Segment(uint16_t cl_port, uint16_t des_port, int seq, int a, uint16_t rwn, 
          uint8_t h_f,  uint8_t f_f, uint16_t ch, std::string data, long mss){
            client_port = cl_port;
            destination_port = des_port;
            this->seq = seq;
            _ack_ = a;
            this -> rwnd = rwn;
            header_field = h_f;
            flag_field = f_f;
            checksum = ch;
            if (data.length() > 0){
               int len = data.length();
               this -> data = new char[len + 1];
               strcpy(this->data, (char *) data.c_str());
               
            }
             this-> mss = mss;
}
void Segment::create_segment(Playload *pt){
  
    uint16_t res = create_checksum(seq, _ack_, header_field, flag_field, 
    checksum, client_port, destination_port, rwnd, data, mss);
    
    pt -> cl_port = client_port;
    pt -> des_port = destination_port;
    pt->seq_ = seq;
    if (data)
      std::strcpy(pt->data_, data);
    else{
      std::cout<<"no data"<<std::endl;
      std::strcpy(pt->data_, "");
    }
    pt ->_ack = _ack_;
    pt ->rwnd_ = rwnd;
    pt->header_field_= header_field;
    pt -> checksum_ = res;
    pt->flag_field_ = flag_field;
    pt->mss_ = mss;
    //std::cout<<pt->data_<<std::endl;
}
bool Segment::extract_segment(Playload *pt){
    /*
    in thsi function we will work on checking bit error
    if bit error occurs, then we will return bool
    we need a helper function 
    */

   uint16_t seq_ = (uint16_t)pt->seq_;
   uint16_t a = (uint16_t)pt->_ack;
   uint16_t header =(u_int16_t)pt->header_field_;
   uint16_t fl = (uint16_t)pt->flag_field_;
   uint16_t mss_ =( uint16_t) pt-> mss_;
   uint16_t ch = create_checksum(seq_, a, header, 
   fl, pt->cl_port, pt->des_port, u_int16_t(0), pt->rwnd_, pt->data_, mss_);
   if (ch != pt->checksum_){
      return false;
   }
   return true;
    
}

Segment::~Segment(){

}
uint16_t Segment::create_checksum(uint16_t seq_, uint16_t a, 
 uint16_t header,  uint16_t fl, uint16_t cl_port, uint16_t des_port, 
 uint16_t check, uint16_t r, char* buf, uint16_t mss){
   


   uint16_t sum = 0;
   sum += seq_ + a + header + fl + check + cl_port + des_port + r + mss;
   if (buf){
      char temp [strlen(buf) + 1];
      strcpy(temp, buf);
      int i = 0;
      int len = strlen(temp);
      while (i < len){
         //std::cout<<temp[i]<<std::endl;
         
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

   }
   /*
   we want to get the extra 1 bit and wrap it around the right bits

   */
   //std::cout<<"sum"<<sum<<std::endl;
   sum = (sum & 0xFFFF) + (sum >> 16);
   sum = (sum & 0xFFFF) + (sum >> 16);
   return ~sum;

}