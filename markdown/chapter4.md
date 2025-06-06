# 4. Configuring and Building the Kernel

## What does the kernel do?

- The kernel has three main jobs:
    - manage resources
    - interface with hardware
    - provide an API to offer a useful level of abstraction to user space programs.
        - You have user space and kernel space utilities.
        - For example, c library is userspace
        - Syscall handlers and device drivers are kernel space.

## Userspace vs. Kernel space 

- In userspace, you run at a lower CPU Priviledge level.
    - This means you can do very little beside make library calls.
- Kernel space runs at a higher CPU priviledge level.
    - When you invoke a system call, the kernel will invoke a trap/interrupt to elevate itself into kernel mode.
- The C standard library is the interface between user and kernel space.

## Linux Kernel development cycle

- Linux is fast paced, with newer versions coming out ever 8-12 weeks.
- Linus Torvalds manages the development kernel tree, and you can clone the repository.
- the kernel development cycle occurs in window of 2 weeks.
    - Here, linus will accept merges.
- After this phase, the stabilization phase begins, where he produce weekly release condidates.
- you can keey track of changes on linux kernel newbie.

## KConfig

- You can customize linux to fit whatever needs you have.
- There's a configuration mechanism known as **KConfig.
    - **KConfig** is integrated with a build system known as **Kbuild**.

- You can choose configuration options using a heirarchy of files named as Kconfig.

- The top config looks like:

```cs
mainmenu "Linux/$ARCH $KERNELVERSION Kernel Configuration"
    config SRCARCH
        string
        option env="SRCARCH"
source "arch/$SRCARCH/Kconfig" //brings in other Kconfig files
```

- `arch/$SRCARCH/Kconfig` brings in another Kconfig file, which is architecture dependent.

- Kconfig files consist mostly of menus, with the menu and endmenu keyword.
- Menu items are denoted with `config`

```cs

menu "Character devices"
[...]
config DEVMEM
bool "/dev/mem virtual device support"
default y
help
Say Y here if you want to support the /dev/mem device.
The /dev/mem device is used to access areas of physical
memory.
When in doubt, say "Y".
[...]
endmenu
```
- There are multiple ways to classify options
    - `bool` tells us that the option is a enabled or disable tag.
    - `tristate`: yes, no, or compile it as a module
    - `int`: an integer with decimal notation
    - `hex`: an unsigned integer value using hexadecimal notation
    - `string`: a string value

- There are many utilities that read these Kcofig files, then produce the .config file.

## linux make utilities

```zsh
make ARCH=arm menuconfig
```

- This will provide an entire tui menu with all the configurations provided by the KConfig files


## Kernel Modules

- Kernel modules aren't super useful in embedded, as the hardware and kernel configuration is already known at compile time.

- There are only a few cases where kernel modules are a good idea:
    - you're developing proprietary modules, for licensing reasons
    - reduce boot time deferring non-essential drivers later.
    - There are a number of drivers that are better loaded, as compiling them would cost too much memory.


## Finding out which kernel target to build

- This heavily depends on what your bootloader expects:
    - U-boot usually requires uImage, but can use zImage or bootz
    - x86 uses bzImage file
    - Most other bootloaders: use zImage file

to build a zImage file, you run the comand:

```
make -j 4 ARCH=arm CROSS_COMPILE=arm-cortex_a8-linux-gnueabihf- zImage
```

- uImage is a kernel format used by U-Boot (legacy, with a special header).

- When you build a uImage using mkimage, it requires a fixed load address (e.g., LOADADDR=0x80008000).

- Different SoCs have different physical memory start addresses (e g., one might start at 0x80000000, another at 0x40000000).

- The kernel is typically relocated to 0x8000 bytes from the start of RAM (e.g., 0x80008000 if RAM starts at 0x80000000).

- Multi-platform kernels don’t have a single fixed load address, which conflicts with the uImage requirement.

## Compiling device trees and modules

```bash
make ARCH=arm dtbs # compiles device tree
make modules # compile modules
```

## Early user space

- To transition from kernel initialization to user space:
    - The kernel mounts a root filesystem, then executes a program in said filesystem.
    - This is done via a ram disk, or mounting a real filesystem on a block device.
    - The kernel starts it's first thread, with PID 1, and runs it.

