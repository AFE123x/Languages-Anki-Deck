# Chapter 3.2

## TCP Header bits

- URG = Urgent Pointer Field in use
- ACK = acknowledge
- PSH: Data has been pushed
- RST: Indicate connection should be reset
- SYN: Used to establish connection
- FIN: Used to release a connection.

## OSI Model

- Application
- Presentation
- Session
- Transport
- Network
- Data Link
- Physical

## Three Access Networks

- Home access networks
- Institutional networks
- Mobile networks

## Active vs passive SFTP

- Passive is when the client initiates the data connection to the server
- Active mode means the server initiates the data connection to the client.

## Principles of congestion control

- Congestion: Too many sources sending too much data too fast for the network to handle.
- Our goal is to achieve self-clocking state.
    - You don't know the bandwidth of a bottleneck.

## TCP Slow Start

- You send 1 window initially.
    - Every RTT, you double the window
    - It'll grow exponential.
    - If a timeout occurs, cut the window size to half.

## Flow Control vs. Congestion Control

- Flow control:
    - avoids overwhelming the receiving application.
    - sender manages the receiver socket buffer.
- Congestion Control
    - Avoid overwhelming the bottleneck network link.
    - Sender is managing the bottleneck link capacity and bottleneck router buffers.