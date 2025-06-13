# Device Trees

## What problem does the device tree solve?

- A system consists more thna a single **System on a Chip**.
- There are devices known as board peripherals that exist, but aren't built into the SOC.
    - This can include SPI, I2C, SDIO
    - The quirk is that the peripherals **can't announce their existences** by themselves to the Operating System.

- There are two options for this

### Option one, have a init board function that does the initialization. AKA, static.

```c
static void my_board_init(void){
    /* Serial */
    my_board_add_device_serial(&serial_data);
    /* SPI */
    my_board_add_device_spi(&zigbee_data);
    my_board_add_device_spi(&serialflash_data);
    /* Ethernet */
    my_board_add_device_eth(&eth_data);
    /* I2C */
    my_board_add_device_i2c(&eeprom_data);
    /* LEDs */
    my_board_add_device_gpios(&leds_data);
}
```
- Con: If you want to change the components, you need to recompile the kernel.

### Option two: Device Trees

- The linux community wanted to get rid of the approach of hardcoding the device details into the kernel. 
- This introduced **device trees**
- Now, every board vendor needs to make a file called `.dts`, which is the device tree source.


## probe function

- The probe function is a callback invoked by the kernel when a device the driver can handle is detected.
- This is frequently used for bus-based driver models like PCI, USB, etc.

- within the probe function, you initialize the device, allocate resources, register with subsystems, etc.

