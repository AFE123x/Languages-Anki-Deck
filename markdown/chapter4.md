# 4. Configuring and Building the Kernel

## What is the kernel, what does it do?

- The kernel is only one component of the operating system.

- The kernel has three main jobs:
    - Manage resources
    - Interface with hardware
    - provide an API abstraction for userspace programs

## Userspace vs. Kernel space 

- Userspace applications run at a lower CPU priviledge level.
- Kernel space has a high CPU priviledge.

## C library

- The C library provides an interface between User Space and Kernel space.
- C libraries translate user level functions into kernel system calls.

## System Calls

- System calls is an interface provided by the kernel for user space applications.
- The system call interface uses either trap or software interrupts to switch the CPU from low priviledge to high priviledge.
    - The **system call handler** is the interrupt handler used when this trap/software interrupt is used.

## KConfig

- KConfig is the configuration mechanism the kernel uses.
- It's matched with a build system known as Kbuild

### Kconfig file

- The configuration options are in files known as Kconfig
- Kconfig files consist of menus, and submenus
- There are many configuration utilities, which will read the Kconfig files and create a .config file.

## Linux compilation - menu configurations

### menuconfig

```bash
make ARCH=arm menuconfig
```
- Linux lets you configure the kernel to your needs.
- You can open `menuconfig` to open the menu of options
- `ARCH=arm` is a variable meaning you want to compile for arm.

### generic config

```bash
make ARCH=arm multi_v7_defconfig
```

- You can select a generic config so you don't need to start from scratch every time.

### old config

```bash
make oldconfig
```

- You can import your old config, and just customize the new options.


## Kernel Modules

- Kernel modules are used so that correct devices and kernel functions can be loaded at runtime.
- This is the alternative to statically linking them into the kernel.

## Kbuild - Introduction

- Kbuild is the linux kernel's build system, consisting of multiple make scripts
- These scripts will read the .config file, work out dependencies, and compile everything to produce the kernel image.

## Kbuild - `obj-y`, `obj-m`, etc.

- you can conditionally compile certain files based on the configuraiton:

```cs
obj-y += mem.o random.o //unconditionally compile
obj-m += hello.o // compile as module
obj-$(CONFIG_TYY_PRINTK) += ttyprintk.o // compile if CONFIG_TTY_PRINTK option is y
```


## Finding out which kernel target to build
- The kernel targe tot build for depends on the bootloader.

| bootloader | kernel image type |
| U-Boot | uImage or zImage |
| x86 | bzImage |
| other bootloaders | zImage |

## Cross compiling kernel, variables

```bash
make -j4 ARCH=arm CROSS_COMPILE=arm-cortex_a8-linux-gnueabihf- LOADADDR=0x80008000 uImage
```
- `-j4` specifies the number of jobs needed
- `ARCH=arm` specifies we're compiling for arm architecture
- `CROSS_COMPILE=arm-cortex_a8-linux-gnueabihf` defines what cross compiler to use.
- `LOADADDR=0x80008000` defines the **relocation address**.
- `uImage` defines we want to make a uImage

## Cross compiling kernel - relocation address

- Linux introduced multiplatform support for ARM platforms. 
- This poses an issue: the location of physical memory differs between platforms.
- We need to code the relocation address in the uImage header.
- Usually, the relocation address is 0x8000 bytes from the start of physical ram.

## Compiling device trees and modules

```bash
make ARCH=arm dtbs # compiles device tree
```

```bash
make ARCH=arm CROSS_COMPILE=arm-cortex_a8-linux-gnueabihf- modules
```
- This will compile features configured as modules.

```bash
make ARCH=arm CROSS_COMPILE=arm-cortex_a8-linux-gnueabihf modules_install
```
- This will install the modules in the right place.
- By default, they're inserted in /lib/modules
- If you wanna customize, you can use the `INSTALL_MOD_PATH` variable.

## Early user space

- The kernel, after initialization, will setup the userspace
- The kernel will enter the `rest_init()` function, which will start the `PID 1` kernel thread, which calls `kernel_init()`.

- `kernel_init()` will try two things:

**if there's a ramdisk (initramfs)**

- It'll try to run `/init`, which can be overwritten

**if there isn't a ramdisk**

- It calls prepare_nampspace(), which will mount a real root filesystem.
- Once the rootfs is mounted, the kernel will look for the init process to run in the following order:
    - `/sbin/init`
    - `/etc/init`
    - `/bin/init`
    - `/bin/sh`


## Kernel messages

- You can print out useful information in the kernel using `printk()`
- There are different categories based on importance of the statement, with 0 from the highest:

| level | meaning |
| KERN_EMERG | system is unusable |
| KERN_ALERT | Action must be taken immediately |
| KERN_CRIT | critical condition |
| KERN_ERR | Error Condition |
| KERN_WARNING | Warning Condition |
| KERN_NOTICE | Normal but significant condition |
| KERN_INFO | informational |
| KERN_DEBUG | debug level message |

## Kernel Command line

- This is a string passed into the kernel by the bootloader.
- It will define how the kernel behaves.
- For example, you can define the init program to run using `init=`