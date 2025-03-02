# Chapter 1

## What are network edges

- Network edges are like your computers, smartphones, etc.


## What are broadband residential access?

- high-speed internet service provided to homes, enabling users to access the internet with fast and reliable connections
    - Examples: dial up, digital subscriber line, cable.


## What is the network core?

- The network core is the mesh of packet switches and links that interconnect the internet's end system.

## Packets and packet switching

- In networks, we need to exchange messages.
    - These messages can be too big, so we can break the message down into smaller chunks of data. These are **packets**.

- We transmit these packets through communication links and **packet switches**.

## Store and Forward transmission

- This approach of transmitting packages entails that the packet switch must receive the entire packet before it can begin transmitting. 

## Queueing delay and Packet loss

- Messages are stored in multiple packets.
    - Packet switches have a output queue (also known as output buffer).
    - If the packet switch is busy transmitting data, it'll store the packet on a queue.

- The time our packet spends in the queue is known as **queueing delay**.

- If the packet becomes full, the switch will ignore the packet, which is known as **packet loss**.


## Packet forwarding, forwarding table

- Routers are responsible for getting and sending out packets. 
    - Question is, how does the router know where to send the packet.

- All end systems have a unique IP address.
- Routers will have a **forwarding table** that maps destination addresses to the routers outbound links. 
    - IP addresses are heirarchial, similar to real addresses (state -> county -> street -> house number).


## Routing protocols

- Routing Protocols are responsible for setting the forwarding table.
    - It'll find the shortest path from each router to destination, and store the resilt into the forwarding table.


## Circuit Switching

- Circuit switching, unlike packet switching, reserves the entire path from both endpoints throughout the duration of communication.
    - For example, telephone networks use circuit-switching networks.
- Circuit switching also reserves a constant transmission rate.

## Packet switching vs. circuit switching

- Packet switching goes link by link
- circuit switching reserves the entire path.

## ISPs

- Internet Service Provides provide Internet Access to end systems.
- To access the internet, end systems need an access ISP.

## Content Provider networks

- These are networks created by content providers to control how their services are delivered to end users.
    - Google is an example of a content-provider network. 

## 4 important network delays, total nodal delay

- Processing delay.
- Queueing delay
- Transmission delay
- Propogation delay.
- total nodal delay is the sum of the 4 delays. 

## Processing Delay

- Processing delay is the time required for the router to examine:
    - the packet's header
    - decide where to direct the router packet.
    - Can include other factors, like checking for bit errors in the packet.

## Queuing Delay

- Queuing delay is the time the packet waits in the queue.
    - If the packet is empty, the queuing delay is 0.

## Transmission delay

- Transmission delay is the time it takes to push one packet onto the link.
    - To get this, we divide the size of the packet in bits by the transmission rate of the link from one router to another.
- Analogy, how long does it take for the toll door to open.

## Propagation Delay

- This is the time it takes for a packet to travel from one router to another.
    - This delay depends on the link, and it's speed.

- Propogation speed in the (distance between router A and B / propagation speed of the link.)

## What's the difference between Transmission and Propagation Delay

- Transmission delay is the time for the router to push out the packet.
- Propagation delay is the time it takes to transport packets to the next router.

## Traffic Intensity

- Traffic Intensity is the ratio of the average rate at which packets arrive at a queue (La) to the transmission rate (R).
    - We can say it's La/R.
        - La is the average rate at which packets arrive at the queue.
        - R is the transmission rate
    - When La/R > 1, then the rate at which packets arrive at the queue exceeds tho rate bits are transmitted by the queue.
    - If La/R is less than one, that means the nth packet will have a queueing delay of L/R
    - If La/R is zero, there's no queueing delay. 

## Packet Loss

- The router queue is limited in number of packets it can hold. If the queue is full, the router drops the packet.

## Throughput

- Throuput is the rate that bits are being sent from the sender to receiver.
    - Can be instantaneous, or average overtime

![alt text](image.png)

- If the link capacities differ, we call the smaller link a bottleneck link. 

## Layering

- When we send data over a network, we need to wrap it into layers, then, when the data reaches the destination, the destination unwraps the data. 

- Why do we need layering?
    - Explicit structure allows identification and distinctions of system pieces.
    - Modularization eases maintenance and updating of systems.

## Layered Internet Protocol Stack

```
+-------------+
| application | <- application: HTTP, IMAP
+-------------+
|  transport  | <- transport: TCP, UDP
+-------------+
|   network   | <- network: IP, Routing protocols
+-------------+
|    link     | <- link: Ethernet, 802.11, Wifi
+-------------+
|  physical   | <- The physical hardware. 
+-------------+
```

## Encapsulation

- When we send data, we encapsulate the data into it's physical representation. Some links don't need to fully unwrap the data, may just need the link.
- The destination will fully unwrap, and get the message for the application.

## IP Sniffing

- IP Sniffing are network interfaces that can read a packet sent from another computer

![](image-1.png)

- Wire shark is an example of a wire sniffer

## IP Spoofing

- A spoofer sends a fake packet, with the illusion that it's sent from another source. 

![alt text](image-2.png)

## DoS Attacks

- These are Denial of Service Attacks
    - Intentionally send requests to a server overwhelming it. 