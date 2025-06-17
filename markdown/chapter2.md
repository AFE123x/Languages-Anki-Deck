# 2. The Linux Device and Driver Model

## What is the linux device and driver model? Definitions of device, driver and bus

- This model provides a universal way to organize devices and drivers into buses.

- **device**: a physical or virtual object that attaches to bus
- **driver**: software entity which may probe for and be bound to devices, which can perform certain management functions.
- **bus**: a device that serves as an attachment point for other devices.

## Device model - structures

- `struct bus_type`, which represents a type of bus (USB, PIC, I2C)
- `struct device_driver` structure, represnts one driver of handling a certain device on a certain bus.
- `struct device`, which represents one device connected to a bus.

## What's a bus

- A bus is a channel between the processor and one or more devices.

## Bus Core Drivers

- Each bus supported by the kernel will have a generic bus core driver.
- The device model assumes all devices are connected to a bus, including virtual devices.
- The kernel will initialize the bus using the `bus_type` structure.
    - After, `bus_register()` will register the bus.
- The bus core driver will setup the buses available (i2c, usb, etc.)

- To register a device, you use `device_register()`
- to register a driver, you use `driver_register()`

## `device_register()`

- To register a device on the bus, you'd use the 'device_register()' function.
    - The `bus_type` has a member called `subsys_private`, which contains a list of devices in the system that resides on a particular bus. `device_register()` adds the device.
    - `device_register`  is called when you initialize or when a device is hot plugged in.

## `driver_register()`

- Within the `bus_type`, there's a `subsys_private`, which contains a list of drivers that can handle devices on the bus.
- You update the list using the `driver_register()` function.

## When a device is plugged in

- When you plug in a device:
    - The bus controller driver will detect device, calling `device_register()`.
    - The bus controller will iterate over to find if there's any driver that can handle a device.
        - There's a `match()` function that handles this.
    - When a driver is found, the driver will be assigned to handle the device.

- There's a `struct device` type, which contains a pointer to it's bus controller device. 
- within the `struct device`, there's a `struct device_driver * driver`, which is the device driver the bus controller uses to assign the driver. 

## Bus Controller Drivers

- Bus Controller Driver Registration: Bus controller drivers register themselves to their bus using the driver_register() function.
- Bus Controller Device Discovery: Bus controller devices are typically discovered during kernel initialization by of_platform_populate().
- Platform Controller Device Registration: Discovered platform controller devices are registered to the platform bus at runtime.

## device drivers

 -All device drivers register themselves with the bus core driver using `driver_register()`.
- when a device on the bus that can be handled by a particular driver is detected, the `probe()` member of the driver is called, and the device configuration data can be retrieved from the device tree.

## Device Trees

- The **Open Firmware Device Tree**, or **Device Tree**, is a data structure that lets you describe hardware. 
- A DT is a tree with **nodes**
    - Each node can have an arbitrary number of properties.
    - Node Connections: Nodes can be linked to each other outside the natural tree structure.

- We, within the nodes, we have bindings, which define how data should appear in the tree to described hardware characteristics.
    - These include: data busses, interrupt lines, GPIO connections, etc.

- There are two file extensions:
    - `.dtsi`: device tree source include file
    - `.dts`: device tree source file

## Device Tree - purposes

- Linux uses Device Tree data for three major purposes:
    - **platform identification**:
        - Here, the kernel uses data in the DT to identify the specific machine.
        - The kernel selects setup code based on the machine's core CPU.
        - The kernel determines the best setup code by comparing the compatible property in the root device tree node with the compatible list, `compatible = "atmel,sama5d2"`.
    - **runtime configuration**:
        - The runtime configuration is communicated via the Device Tree.
            - Configuration includes things like kernel parameters, and the initrd image location.
        - The data will be in the `chosen node`.
            - `bootargs` are the kernel arguments
            - `initrd-*` has the address and size of initrd blob.

```cs
chosen {
    bootargs = "console=ttyS0,115200 loglevel=8";
    initrd-start = <0xc8000000>;
    initrd-end = <0xc8200000>;
};
```
    - **device population**:
        - Once the board's been identified, the kernel initialization can proceed.
        - During the kernel initialization, it'll convert the device tree to a more efficient runtime representation.
        - With this conversion, irq handlers will be enabled, etc.