## Description
This project aims at  designing and implementing mini relaiable trasport(MRT) protocol on top of UDP without using UDP's checksum features.
1. This transport layer supports protection against segment losses via segment retransmissions
2. protection against data corruption via checksums 
3. protection against out-of-order delivery of segments on the UDP layer 
4. fast transmission if the latency is high
5. transmission of small or large amounts of data (segmentation is needed for sending a large amount of data);
6. flow control (to avoid overflowing receiver buffer).

## USAGE:
1. Client Program:
    - 
    ```bash
         cd client
         make all
        ./client <client-port> <server-addr> <server-port> <server-address>
    ```
2. Network Program
    ```bash
         cd network
         make all
        ./network <network Port> <server_addr> <server_port> <loss File>
    ```
3. Server Program
    ```bash
         cd server
         make all
        ./server <server port> < buffer size>
    ```




