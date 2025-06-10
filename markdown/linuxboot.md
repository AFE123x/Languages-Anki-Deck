# Linux Boot Process

## Linux Boot Requirements

- To run linux on an embedded, we need 4 software components:
    - **RBL(Rom Boot Loader)**: first picee of code to run when you power the board. It's provided by the vendor.
        - Theng RBL is responsible for loading the second stage loader from the internal memory of SOC
    - **SPL (Secondary Program Loader)/MLO (memory loader)**: load and execute the third stage boot loader.
        - For example, u-boot
    - **u-boot**: Load the execute the linux kernel from DDR memory.
    - **linux kernel**: The actual operating system., which will initialize the Root Filesystem.

## Beagle board boot options

- The beaglebone black offers many options for booting the system from (NAND, NOR flash, etc.)

- You can specify the order of the boot loader by filling the **sysboot** register.
    - For example, if you set **sysboot** to `0b00001`, which will try to boot from UART0, then XIP, then MMC0 then SPI0
    - check the data sheet for the actual boot orders.

- When the system boots the secondary stage boot loader, it'll first check the **sysboot** register, and come up with the list of booting devices.
    - This is controlled by hardware, cannot use software for it.

## Changing boot mode of beaglebone

- The boot button will adjust the configuration
    - If S2 is pressed, then the boot order is: SPI0, MMC0, USB0, UART0
        - Good if you don' want to load from eMMC (or MMC1)
    - If S2 isn't pressed, eMMC is booted first, then SD card, then UART0, USB0.

## Boot options:

- eMMC Boot (MMC1): This is built onto the board
- SD boot (MMC0): This is an SD card
- Serial: uses the serial port
- USB port: Straight forward.

## ROM Startup Sequence

- First, it will initialize:
    - It will setup the stack.
    - Calls the main function of program
    - Initialize watchdog timer
    - Configure the clock and PLL (Phase Lock loop)
    - load the second stage bootloader.
    - It'll load the MLO from the external memory into the internal ram. 

## Phase Lock Loop Engine

- It takes a low frequency source, like an Crystal oscillator.
- The PLL will then output a higher clock frequency to run the various subsystems of the SOC.
- The clock speed is set by the **sysboot** registers, bits 15:4.
- The clock initialization is done in the rom, you cannot change it.
- You could adjust the speed later in the Second stage bootloader.

## RBL to MLO/SPL

- When the ROM bootloader loads the SPL into SRAM, it'll load an iamge header, containing two importants things
    - load address: which SRAM address to load MLO.
    - total size of MLO: how big is the SRAM program.

## MLO/SPL job

- The MLO and SPL initialized the SOC to the point that U-Boot can into external RAM (DDR memory).

- specifically:
    - uart console initialization to print out debug messages
    - reconfigure the PLL to desired value.
    - Initializes DDR registers to use DDR memory
    - muxes configuration of boot perihperal pin, since it's next job is to load the u-boot from the boot peripheral.


## Why can't the AM335x RBL load Uboot directly into DDR?

- The AM335x RBL (ROM boot loader) cannot load uboot directly into DDR3 by skipping SPL.
- The ROM code doesn't know what kind of DDR RAM you're using.
- The SPL/MLO will know the kind of DDR connected, as you implemented that configuration in the SPL code.
- Also, the ROM size and SRAM size are limited.

## What does U-Boot do?

- Initialize some peripherals like I2C, NAND, FLASH, etc.
- Load the linux kernel image from various boot sources to the DDR memory
    - The kernel could come from SD card, etc.

- Uboot looks for a uimage file:
    - this consists of:
        - a 64 byte uboot header
        - a zimage

## uEnv.txt

- You should also provide a file called `uEnv.txt`
    - This instructs how u-boot behaves

## uboot image, uboot header

- The uboot image consists of a 64 byte uboot header on top of a zimage.
- This will include checksums, size of the zimage, etc.
- You can decode the raw ram dump of the kernel with the `md` utility in uboot.