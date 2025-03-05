# Chapter 3

## What is the Transport layer?

- The transport layer provides **logical communication** between application process running on different hosts.
- The transport protocol works in the end systems.
- There are two protocols: UDP and TCP
- The transport layer is the logical communication between processes.
    - The layers below do the actual transportation.

## What is logical communication?

- From the transport view, two devices are logically connected, despite being separated by routers, links, etc.
    - The medium through which they're connected may lose, flip bits or affect order.
        - Under logical communication, we ignore these mishaps.

## What is demultiplexing, how does it work?

![alt text](image-16.png)

- When a host receives an IP datagram, it has:
    - each datagram has source IP and destination IP address.
    - there are two interesting fields in the head:
        - source port.
        - destination port.
- The multiplexer uses the IP and the port to figure out which socket to use.



## What is multiplexing?

![alt text](image-17.png)

## What does demultiplexing solve for us?

![alt text](image-18.png)

- We'll have many clients that use the transport layer. When we receive data from the HTTP server, how do we know which application the data belongs to?
- Another example, when we send a request to an HTTP server, which process should handle it?

