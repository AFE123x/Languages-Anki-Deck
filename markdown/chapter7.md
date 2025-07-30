# 7. I2C Client Driver

## I2C driver architecture

- when you write a driver for a device that lies on a physical bus, the device driver needs to rely on the bus's driver, known as the **controller driver**

- the bus controller driver exports a set of functions to simplify the development of drivers.


## I2C Driver Structure

```c
struct i2c_driver {
    int (*probe) (struct i2c_client*, const struct i2c_device_id);
    int (*remove) (struct i2c_client*);

    void(*shutdown)(struct i2c_client*);
    struct device_driver driver;
    const struct i2c_device_id* id_table;
}
```

- the probe() function is executed when an i2c device is instantiated
- remove() function handles unregistering the device from i2c subsystem.


## i2c - implementing `probe()` function

```c
static int probe(struct i2c_client* client, const struct i2c_device_id* id);
```

- probe does the following:
    - check whether the device is the one you expect
    - check if the i2c bus controller supports functionality needed by device
    - initialize device
    - set up device specific data
    - register the appropriate kernel framework

- the parameters include:
    - struct i2c_client, which represents the i2c device
    - id points tot he 2c device ID entry matching the device being probed.

## adding per_device data to i2c device

- you can add device related data to the device using `i2c_set_clientdata()`.
- you can retrieve it using `i2c_get_clientdata()`

```c
void i2c_set_clientdata(struct i2c_client* client, void* data);
void* i2c_get_clientdata(const struct i2c_client* client);
```

## implementing remove() function

- remove function is the function where you clean up, and unregister the device from the subsystem.


## adding/removing i2c device to subsystem

- there are two functions for this:
    - `gpiochip_add()`
    - `gpiochip_remove()`

## i2c driver device provisioning

- similar to other platform_devices, you'll need an i2c device id table, containing the name and driver_data.

- there's a struct, called i2c_device_id.

```c
i2c_device_id{
    char name[I2C_NAME_SIZE];
    kernel_ulong_t driver_data;
}
```


## communicating with i2c device - approaches

- To communicate with an I2C device, there are two functions to send/receive data:

```c
int i2c_master_send(struct i2c_client* client, const char* buf, int count);
int i2c_master_recv(struct i2c_client* client, const char* buf, int count);
```

- another function is `i2c_transfer`, which can either be a read or write

```c
int i2c_transfer(struct i2c_adapter* adap, struct i2c_msg* msg, int num);
```

- unlike send and receive, you can have a chain of commands. 


- there are also System Mangement Bus (SMBus) compatible functions.

- SMBus is intels spin to I2C. All I2C devices are SMBus compatible, but not the other way.
    - therefore, it's better to use smbus functions:

```c
s32 i2c_smbus_read_byte_data(struct i2c_client *client, u8 command);
s32 i2c_smbus_write_byte_data(struct i2c_client *client, u8 command, u8 value);
s32 i2c_smbus_read_word_data(struct i2c_client *client, u8 command);
s32 i2c_smbus_write_word_data(struct i2c_client *client, u8 command, u16 value);
s32 i2c_smbus_read_block_data(struct i2c_client *client, u8 command, u8 *values);
s32 i2c_smbus_write_block_data(struct i2c_client *client, u8 command, u8 length, const u8 *values);
```

## definining and registering i2c driver

- to define a i2c device, you'd use a device tree

```cs
&i2c3 {
    status = "okay";
    
    bno055@55 {
        compatible = "bosch,bno055";
        reg = <0x55>;
    };
};
```
