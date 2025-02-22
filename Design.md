Flow Control: 
- Receiver Side Rev window is calculated by total_buffer_size - #of total present segments. If recv_window == 0, then sender will send an special seq -1 to keep the connection alive and get the window > 1. If the window size > 0, then the sender sender based in the recvd rwndmby not overwhelimg the system.
The property has been tested by slowing the data processing of the receiver process such that the data  buffer gets eventually full

Packet Loss / Timeout : 
3 scenrios:
1. Cientside Packet Loss
    --> Recvr side implementation: If the current #seq > expected_seq: then resend the prec sequece
    --> Client Side implimentation: 
    Matain a data structure vector to hold the segements and the seqence list
2. Server Side packet Loss
3. Timeout


 ./client 49996 127.0.0.1 50000 900
 ./server 50001 5000
 ./main 50000 127.0.0.1 50001 ""