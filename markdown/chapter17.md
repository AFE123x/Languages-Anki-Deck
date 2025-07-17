# 17. i2c

## What is I2C

- I2C is a popular protocol which can let you communicate with multiple devices using only two wires.

- This is advantageous over SPI since you don't need a select pin for each device.
    - This does come at the cost of speed.
- Instead, Each slave device is assigned a unique address, which is 7 bits. 


## I2C - wires

- There are two wires:
    - SCL: Serial Clock - controls timing of communication.
    - SDA: Serial Data - has actual data.


## I2C - Communication

- In I2C, the SDA line will change only if the Clock is low.
    - this means the bits are read when the clock is high.

- There are two exceptions to this:
    - when starting: you set SDA to low while SCL is high
    - when stopping: SDA goes to high while SCL is high

## I2C data format

- when you address a specific device, it'll only be 7 bits. The last 8th bit will indicate if you want to do a read/write.

## I2C - Acknowledgement bit

- After sending an 8 bit, an acknowledgement bit is sent:
    - if it's low (0), it's an ACK, good
    - if it's high (1), it's a NACK, meaning there was an error.


## I2C - steps

![alt text](image-2.png)

- First, the SDA line will be brought to low, to indicate the beginning of communication
- Second, the AπVR sends the address of the slave device followed by a 0 to indicate write mode
- The slave device will acknowledge.
- The AVR sends tho address to tell the sensor what register it's going to read (for example, the temperature register in a temperature sensor).
- slave acknowledges again.
- the AVR will send the address of slave again, but with a 1, indicating the AVR wants to read from the slave.
- the sensor sends acknowledgement.
- AVR sends the data (however many bytes there are.)