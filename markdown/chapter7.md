# 7. Creating a Storage Strategy

## Storage - options

- An embedded device will need to use storage that takes little power, compact and will be reliable.
- Flash memory has evolved overtime, starting from NOR to NAND, then Managed flash like eMMC.
- Regardless of the storage medium you choose, you need a device to boot and reach a functional state despite power failures and unexpected resets.
- You should choose a filesystem that performs well.

## Storage - NOR

- NOR is expensive, but is reliable.
- NOR is low capacity, only being Megabytes to a gigabyte.

- The memory cells in NOR flash is arranged into erase blocks (usually 128 KiB).
- Each erase cycle will damage the device, eventually becoming unusable.

- NOR flash can be addressed in the CPU address space, and doesn't need initialization.
- NOR is the perfect location for bootloader code.
- You can put the kernel and root filesystem in flash memory, so you can save time not loading them in memory
    - this technique is known as XIP, or eXecute In Place

- The standard NOR interface is the **common flash interface**.

## Storage - NAND, Bits in memory cell, reliabiliby tradeoff

- NAND flash is cheaper than NOR memory, and is available in high capacities.
- It's in the range of megabytes to 10s of gigabytes.


- In first generation NAND, Each memory cell in nand will usually hold a single bit, known as **Single level cell organization (SLC)**
- Later, 2 bit memory cells were used, known as **Multi Level Cells(MLC)** chips.
- 3 bit memory cells are known as **Tri Level Cells (TLC)**.

- When are are more bits per memory cell, it requires more circuitry to work.
- This is the tradeoff: more bits per memory cell - less reliable.

- Access to NAND will require a NAND flash controller.
- you'll need the drivers for it in the bootloader and kernel.
- THe standard register-level interface for NAND chip is the Open NAND Flash Interface, or ONFi.

## Storage - NAND, reads/writes

- NAND flash can only be read/written in pages, so it can't be read in bytes
- This means you cannot directly map NAND is the CPU address space.
- **IMPORTANT**: before writing to nand, you need to erase it

## Storage - NAND, error detection

- transfers to and from the chip are prone to bit flips.
- There are **Error correction codes** to check for this.
- Most SLC chips will use a sample **Hamming code**, which can be implemented in software.
- for MLC and TLC chips, something more complex is used, like **Bose-chaudhuriHocquenghem (BCH)**. This needs hardware support.

- ECCs are stored in an extra special region of memory, known as the Out-Of-Band area.

- For SLC designs, there'll be 1 byte of OOB per 32 bytes of main storage.
    - this means for a 4 KiB page (4006 bytes), the OOB will be 128 bytes.
- MLC and TLC will have more complex ECCs

- The OOB will contain a flag set by the manufacturer defining which memory blocks fail.

# Storage - OOB Area

- OOB area: Out-Of-Band area.
- This contains multiple flags.

- The manufacturer tests all blocks, mark marks a flag for blocks that fail.
- It'll also contain ECC(error correction code) bytes.
- there are some bytes left, which some flash filesystems will use to store filesystem metadata.
- This layout isn't standardized, so different components (boot ROM, bootloader, kernel MTD, filesystem utilities) might assume different OOB formats.
- you're responsible for this.

## Storage - Managed Flash Memory

- Managing flash memory can be a burden, as there's no standard interface hiding the complexities of memory

- Maneged flash memory consists of NAND flash, but has a more standardized interface.
- It combines one or more flash chips with a microcontroller, providing an ideal storage device with small sector size compatible with the conventional file system.
- THe most import type of chip for embedded is the **Secure Digital (SD)** card, and the embedded variant, eMMC

## MultiMediaCard and Secure Digital cards

- MultiMediaCards (MMC) and Secure Digital (SD) are a modern means to store data.
- These cards use the **Serial Peripheral Interface (SPI) protocol.

- The SD card will contain two components, typical for Managed Flash memory
    - Microcontroller: Implements command set, and manages flash memory
    - Nand memory: Holds the bytes


- The microcontroller performs the function of the **flash translation layer**.

## eMMC

- eMMC is the Embedded MMC, which is a MMC, but can be soldered on the mother board.

## Storage - Hard Disks

- In embedded systems, hard disks are rare to find. The only exception is digital video recording setups where you need lots of storage.

## Accessing flash memory in linux

- Raw NOR and NAND flash memory is handled by the **Memory Technology Device Subsystem**, or MTD.
- This provides an interface to read, erase and write blocks to flash memory.
- There are also functions to handle the OOB area and to identify bad blocks.

## Memory Technology devices

MTD (Memory Technology Device) is organized into three layers:

1. **Chip drivers (lowest layer):** Interface directly with NOR or NAND flash chips.
2. **MTD core:** Provides a common interface and logic for different flash chips.
3. **User-level drivers:** Expose flash memory to users as either character devices (`/dev/mtd`) or block devices (`/dev/mtdblock`).

* **NOR flash** needs only a few standard drivers (e.g., CFI-compliant).
* **NAND flash** requires specific drivers for each flash controller, usually provided by the board support package.
* The Linux kernel has around 40 NAND drivers under `drivers/mtd/nand`.

## MTD Partitions

- You probably want to partition flash memory into a bunch of areas: One for bootloader, another for kernel image and root fs.
- In MTD, you can specify the size and location in multiple ways:
    - through the kernel command line with CONFIG_MTD_CMDLINE_PARTS
    - device tree
    - platform mapping driver.


## MTD Partitions via Kernel Command Line

You can define MTD partitions at boot using the `mtdparts` kernel parameter:

```bash
mtdparts=<mtd-id>:<partdef>[,<partdef>][;<mtd-id>:<partdef>...]
```

* **`<mtd-id>`**: Name of the flash chip.
* **`<partdef>`**: Partition definition in the form:

  ```
  <size>[@<offset>](<name>)[ro][lk]
  ```

  * `size`: Partition size (use `-` for remaining space).
  * `@offset`: Optional offset (defaults to end of previous partition).
  * `(name)`: Optional partition name.
  * `ro`: Read-only.
  * `lk`: Locked.

**Example:**

```bash
mtdparts=:512k(SPL)ro,780k(U-Boot)ro,128k(U-BootEnv),4m(Kernel),-(Filesystem)
```

This creates five partitions:

* SPL (512k, read-only)
* U-Boot (780k, read-only)
* U-BootEnv (128k)
* Kernel (4MB)
* Filesystem (uses remaining space)


## MTD Partitions - checking configuration at runtime

- you have two options: 
    - use proc: `/proc/mtd`
    - use sysfs: `/sys/class/mtd`

- You can use the `mtdinfo` command to do this.


## MTD Partitions - Using device tree

- you can specify the MTD partitions via device trees. Here's what a 5 partition mtd looks like:

```cs
nand@0,0 { //defines nand node
    #address-cells = <1>; //size of each address
    #size-cells = <1>; //size of the size cell (32 bits default.)
    partition@0 { //defines partition
        label = "SPL";
        reg = <0 0x80000>; //offset 0, size 0x80000 bytes (512 kb)
    };
    partition@80000 {
        label = "U-Boot";
        reg = <0x80000 0xc3000>; // offset 0x80000, size, 0xc3000
    };
    partition@143000 {
        label = "U-BootEnv";
        reg = <0x143000 0x20000>;
    };
    partition@163000 {
        label = "Kernel";
        reg = <0x163000 0x400000>;
    };
    partition@563000 {
        label = "Filesystem";
        reg = <0x563000 0x7a9d000>;
    };
};
```

## MTD Partitions - C driver struct


- you can use the `mtd_partition` struct to list the partitions.

```c
static struct mtd_partition omap3beagle_nand_partitions[] = {
    {
        .name = "X-Loader",
        .offset = 0,
        .size = 4 * NAND_BLOCK_SIZE,
        .mask_flags = MTD_WRITEABLE, /* force read-only */
    },
    {
        .name = "U-Boot",
        .offset = 0x80000;
        .size = 15 * NAND_BLOCK_SIZE,
        .mask_flags = MTD_WRITEABLE, /* force read-only */
    },
    {
        .name = "U-Boot Env",
        .offset = 0x260000;
        .size = 1 * NAND_BLOCK_SIZE,
    },
    {
        .name = "Kernel",
        .offset = 0x280000;
        .size = 32 * NAND_BLOCK_SIZE,
    },
    {
        .name = "File System",
        .offset = 0x680000;
        .size = MTDPART_SIZ_FULL,
    },
};
```

## MTD Device Drivers



* The top level of the MTD subsystem consists of two types of device drivers:

  * **Character devices** (major number **90**):
    * `/dev/mtdN`: Read/write access (minor = N \* 2)
    * `/dev/mtdNro`: Read-only access (minor = N \* 2 + 1)
  * **Block devices** (major number **31**):
    * `/dev/mtdblockN` with minor number **N**


## MTD Character device, `ioctl`

- you can access the underlying flash memory as an array of bytes, so you can read/wret the flash.
- The device implements a bunch of ioctl functions that let you erase blocks and manage OOB areas on the NAND chip.
    - For example, `MEMERASE` erases the block in the MTD partition.

- There are too many ioctl calls for mtd, so I'm not gonna list them all

- There's a set of utility programs known as mtd-utils, which lets you manipulate flash memory.

## The MTD block device, `mtdblock`

- this is a driver linux provides, but it isn't used
- The driver doesn't handle bad blocks in NAND flash, or wear leveling.

## Filesystems on flash

- Ideally we want to efficiently use flash memory for mass storage.
- Unfortunately, there are issues:
    - mismatch between erase block and disk sector size.
    - limited number of erase cycles per erase block
    - need for handling bad blocks on NAND chips.

- Fortunately, there is a solution: **Flash Translation Layers**

## Flash Translation layers - features

- The Flash translation layer will contain the following features:
    - **suballocation**: We divide erase blocks into smaller logical units to avoid wasting space. 
    - **Garbage Collection**: Because of suballocation, an erase block will conatin a mix of good and bad data after using the filesystem.
        - You need to coalesce the good data into one place, and return the empty erase block to the free list.
    - **wear leveling**: Move data to ensure each block is erased roughly the same number of times
    - **bad block handling**: avoid using blocks marked bad, and mark good blocks as bad if they can't be erased.
    - **robustness**: If the embedded device is powered off without warning, the filesystem should be able to cope without coruption.


## Flash Translation layer - deploying layer

- you can deploy this:
    - in the file system, like JFFS2, YAFFS2 or UBIFS
    - in the block device driver
    - in the device controller.
    