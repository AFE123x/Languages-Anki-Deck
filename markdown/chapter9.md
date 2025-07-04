# 9. Interfacing with Device Drivers

## What is the role of a kernel driver?

- The kernel driver encapsulates the hardware interfaces of a computer system, and presents them in a consistent manner to user space programs.

## types of device drivers

- character: meant for unbuffered I/O
- Block: tailored for block I/O
- network: meant for transmitting and receiving network packets.
- virtual: `/dev/random` for example.

## Character Devices