# Chapter 6

## What is the Link Layer?

- Links connect hosts/routers to each other.
    - Could be by physical wire, or a bunch of interconnected switches, or LAN

- The link layer is responsible for transferring datagram from one node to a physically adjacent node over a link. 
    - A nice analogy is travelling from Princeton to Lausanne:
        - Limo: Princeton to JFK
        - Plane: JFK to Geneva
        - Train: Geneva to Lausanne
        - The tourist is the datagram.
        - The transport segment is the communication link.
        - The transportation mode is the link-layer protocol.


## Link Layer services

- The link layer implements an encapsulation service.
    - The link layer takes network layer datagram, wrap header stuff around it, and prepare it for the physical layer.
- MAC (Media Access Control) is a network protocol that defines how devices share access