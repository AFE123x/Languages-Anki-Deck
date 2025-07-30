# 5. Platform Device Drivers

## what are platform devices

- There are plug and play devices, like USB and PCI Express.
    - these are **auto-discovered devices**

- There are devices that are built into the SoC, like I2S, I2C, etc.
    - these are not discoverable, and these are known as **platform devices**

## what is a pseudo platform bus?

- From the SoC point of view, devices are connected internaly through through dedicated buses, which are mostly proprietary and specific to the manufacturer.
- to the kernel, they're root devices connected to nothing.

- the **pseudo platform bus** is a kernel virtual bus for devices not seated on the physical bus known to the kernel.

## how to handle platform devices?

- register a unique platform driver to manage device.
- register the platform device with the same name as the driver, and resources to let the kernel know the device is there.



## platform drivers - support

- It's important to note:
    - not all platform devices are handled by platform drivers.
        - for example, though I2C and SPI devices are platform devices, they rely on the I2C or SPI not on the platform bus.


## platform drivers - structures

- platform drivers are represented with the `struct platform_driver` type

```c
static struct platform_driver{
    int (*probe)(struct platform_device* pdev);
    int (*remove)(struct platform_device* pdev);
    struct device_driver driver; //contains driver name and owner of driver
}
```

- the `probe` function is called when a device claims your driver after the kernel matches the two.
- `remove` will get rid of the driver when it isn't needed anymore by the device.
- `driver` describes the driver itself.

## platform drivers - registering/removing

- to register a platform driver you can either use:
    - `platform_driver_register()` - will store driver in list of drivers, eventually calling probe once a match occurs.
    - `platform_driver_probe()` - immediatly look for matching device

- to remove a platform_driver, you use the `platform_driver_unregister()` function.

## `module_platform_driver` macro

- when you're developing a platform driver, you don't need to implement the init and exit functions.
- instead, you can use the `module_platform_driver()` macro

```c
// [...] your implementations
static struct platform_driver my_driver = {
    // [...]
}

module_platform_driver(my_driver); //does the init and exit stuff for you.
```

- there are also specific macros for each protocol:
    - `module_spi_driver(struct spi_driver)`
    - `module_i2c_driver(struct i2c_driver)`
    - `module_pci_driver(struct pci_driver)`
    - `module_usb_driver(struct usb_driver)`
    - `module_mdio_driver(struct mdio_driver)`


## `platform_device` struct

```c
struct platform_device {
	const char	*name;
	u32		id;
	struct device	dev;
	u32		num_resources;
	struct resource	*resource;
};
```

- Once you make the driver, you need to feed the kernel with a list of devices that can use the driver.
- this is done with the `platform_device` struct
- the `name` field should be the same as the one in the `platform_driver` struct

## Device Provisioning - reasoning and approaches

- for devices besides hot plug devices (usb and pcie), the kernel doesn't know what devices are in your system.
- you need to explicitly define these resources and where they are.

- there are two approaches:
    - device provisioning (using resource struct) **outdated**.
    - device trees, the new and better way.


## device provisioning - device trees

- the device tree is a hardware description file, where each device is represented with a node, and each node contains the property. 

## how to match platform device and driver?

- this is done with a MODULE_DEVICE_TABLE.
- at compilation time, the build process will extract the information from it, and build a file named `modules.alias`.

```c
#define MODULE_DEVICE_TABLE(type, name)
```

- this macro will initialize a device table:
    - type is the type of devices: i2c, spi, usb, etc.
    - name: a pointer to a xxx_device_id array, where xxx is the protocol.

