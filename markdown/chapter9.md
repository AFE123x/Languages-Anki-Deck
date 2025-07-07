# 9. Interfacing with Device Drivers

## What is the role of a kernel driver?

- The kernel driver encapsulates the hardware interfaces of a computer system, and presents them in a consistent manner to user space programs.

## types of device drivers

- character: meant for unbuffered I/O
- Block: tailored for block I/O
- network: meant for transmitting and receiving network packets.
- virtual: `/dev/random` for example.

## Device Nodes

- Character devices are identified in user space via a **device node**
- Device nodes are uniquely identified with a major and minor number.
    - major maps the device node to particular driver.
    - minor number tells driver what interface is being accessed.

## Block devices major/minor numbers

- major numbers identify the device driver
- minor numbers identify the partition.