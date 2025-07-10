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


## sysfs

- sysfs provides a treelike representation of kernel objects.
    - A kernel object represents a directory and an attribute is a file.

- There are three main directories:
    - devices
    - class
    - block

## sysfs: /sys/device

- This lists all the devices discovered since boot, and how they're connected with each other.
- They're broken into three groups:
    - system: Includes heart of system, like CPU and clocks
    - virtual: devices in memory, like `/dev/null`
    - `platform`: devices not connected via the conventional hardware bus.

##  sysfs - `/sys/class`

- This directory provides a view of device drivers based on the type.
    - for example, `input` folder contains devices like the keyboard

## sysfs - `/sys/block`

- you can view all the block drivers with this directory.
- each subdirectory is for each block device.

## Drivers - Controlling drivers

- How drivers are controlled are put into two classes:
    - files in sysfs (like gpio, LEDs, etc.)
    - serial buses that exposes generic interface through a device node, like i2c.

## GPIO, adding/removing from linux

- GPIO is General Input/Output
- GPIO pins are grouped together with GPIO registers, usually 32 bits per register.
- gpio is handled via the gpiolib subsystem, which is a GPIO driver to expose I/O in a consistent way.

- you can interact with gpiolib via sysfs: `/sys/class/gpio`

- you can add gpio using `/sys/class/gpio/export`

```bash
$ echo 53 > /sys/class/gpio/export
```

- You can remove via unexport

```bash
$ echo 53 > /sys/class/gpio/unexport
```

## GPIO - Handling Interrupts

- There's an `edge` file in the gpio directory, which will invoke an interrupt depending on what `edge` contains:
    - `rising`: Interrupt on rising edge
    - `falling`: Interrupt on falling edge
    - `both`: interrupt on rising and falling edge
    - `none`: no interrupt.


- Once you set that, you can use `poll` to wait for the interrupt to happen in your C code:

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
int main(int argc, char *argv[])
{
    int f;
    struct pollfd poll_fds[1];
    int ret;
    char value[4];
    int n;
    f = open("/sys/class/gpio/gpio48/value", O_RDONLY);
    if (f == -1) {
        perror("Can't open gpio48");
        return 1;
    }
    n = read(f, &value, sizeof(value));
    if (n > 0) {
        printf("Initial value=%c\n",
        value[0]);
        lseek(f, 0, SEEK_SET);
    }
    poll_fds[0].fd = f;
    poll_fds[0].events = POLLPRI | POLLERR;
    while (1) {
        printf("Waiting\n");
        ret = poll(poll_fds, 1, -1);
        if (ret > 0) {
            n = read(f, &value, sizeof(value));
            printf("Button pressed: value=%c\n",
            value[0]);
            lseek(f, 0, SEEK_SET);
        }
    }
    return 0;
}
```

## i2c

- i2c is an interface with two wires.
- A subset of i22c is the SMBus, which is the system management bus found on pc.
- i2c hosts are represented on linux with `/dev/i2c*`

- The device interface contains a bunch of ioctl commands that queries the host controller.
- you can use read/write to read/write data from the slave address


```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#define I2C_ADDRESS 0x50
int main(void){
    int f;
    int n;
    char buf[10];
    f = open("/dev/i2c-0", O_RDWR);
    /* Set the address of the i2c slave device */
    ioctl(f, I2C_SLAVE, I2C_ADDRESS);
    /* Set the 16-bit address to read from to 0 */
    buf[0] = 0; /* address byte 1 */
    buf[1] = 0; /* address byte 2 */
    n = write(f, buf, 2);
    /* Now read 4 bytes from that address */
    n = read(f, buf, 4);
    printf("0x%x 0x%x0 0x%x 0x%x\n",
    buf[0], buf[1], buf[2], buf[3]);
    close(f);
    return 0;
}
```

## SPI

- SPI is similar to i2c, but is faster and uses 4 wires.
- linux has a generic SPI driver.
    - this driver creates a node for each SPI controller, formatted as `spidev[dev].[chip select]`.

## Designing character driver interface

- to commicate with devices besides read/write:
    - `ioctl`: pass two arguments into driver
    - `sysfs`, `debugfs`, `proc`: file system
    - `mmap`: direct access to kernel buffers and hardware registers
    - `sigio`: driver can send signal from driver
    - `netlink`: not important

## device drivers - discovering hardware configuration

- there are two main approaches to do this:
    - the device will register the device on a discoverable bus, sending the requirements.
        - the kernel will match the device with the driver, and call the driver's probe function.
    - read the device tree


## linking hardware with device drivers

```cs
net@10010000 {
    compatible = "smsc,lan91c111";
    reg = <0x10010000 0x10000>;
    interrupts = <25>;
};
```

```c
static const struct of_device_id smc91x_match[] = {
    { .compatible = "smsc,lan91c94", },
    { .compatible = "smsc,lan91c111", },
    {},
};
MODULE_DEVICE_TABLE(of, smc91x_match);
static struct platform_driver smc_driver = {
    .probe = smc_drv_probe,
    .remove = smc_drv_remove,
    .driver = {
    .name = "smc91x",
    .of_match_table = of_match_ptr(smc91x_match),
},
};
static int __init smc_driver_init(void){
    return platform_driver_register(&smc_driver);
}
static void __exit smc_driver_exit(void){
    platform_driver_unregister(&smc_driver);
}

module_init(smc_driver_init);
module_exit(smc_driver_exit);

```

- When we initialize the driver, we call `platform_driver_register`, which takes a `platform_driver`, which contains a probe function.
    - the kernel will look at the driver property of platform_driver, which contains the compatible devices.
    - The compatible device should match the device in the tre.