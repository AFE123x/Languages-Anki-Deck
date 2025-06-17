# 1. Building the System

## Main Components of a Linux Embedded system

- Bootloader
- Kernel
- System call interface
- C runtime library
- System shared library
- Root filesystem

## Bootloader

- Linux can't be started without a **bootloader**.

- The bootloader initializes the system. At a minimum:
    - configure the memory system
    - Load kernel image and device tree at correct address.
    - Optional loading of Init RAM disk at correct memory address.
    - Setting of kernel command line and other parameters.

- Usually, the bootloader also initializes a serial console for the kernel.

- **uboot** is popular for linux embedded devices.

## Linux Kernel

- The linux kernel is a clone of the UNIX operating system.
- The kernel is the lowest level of software, charged with:
    - managing hardware
    - running user programs
    - maintaining security.

## System Call Interface and C Runtime Library

- System Call Definition: The fundamental interface between an application and the Linux kernel, enabling user space applications to interact with the kernel.
- C Runtime Library Function: Provides applications with access to OS resources and functions by abstracting the OS system call interface.
- Common C Runtime Libraries: glibc, uClibc, eglibc, dietlibc, and newlib, with glibc being the default for the Yocto project.

## System Shared Libraries

- System Shared Libraries Definition: Libraries loaded by programs at startup, providing access to:
    - system functionalities like compression, encryption
    - kernel resources.
- System Shared Libraries Function: Encapsulate system functionalities, acting as building blocks for applications interacting with the system.
- Required Libraries: Libc, Libm, Libpthread, Libdl, Libcrypt, Libpam, Libz, Libncurses, Libutil.
- Library Locations: /lib, /usr/lib, /usr/local/lib.
- Libc Functionality: Elementary language support, OS platform services, direct access to the OS System-Call-Interface.


## Root file system

- Root Filesystem Definition: The root filesystem is the top-level directory in a Linux system, containing all other directories and files.
    - mounted as /.
- Filesystem Hierarchy Standard (FHS): Defines the structure of the Linux filesystem.
    - ensuring compatibility and allowing applications to locate system files and configurations.
- Key Directories: Includes directories like /bin, /sbin, /etc, /home, /root, /lib, /dev, /tmp, /boot, /mnt, /opt, /usr, /var, /sys, and /proc, each with specific purposes.

## Bootloader process

- We can break the bootloader process into three steps:

**phase 1 - ROM Code**

- Here, the SoC will have code stored internally that runs on reset or poweron. This is known as **ROM code**.
- The SRAM is small in size, so it cannot do anything besides load the **secondary program loader** into the SRAM.

- SRAM is used since it doesn't have a memory controller that needs to be initialized.

**Phase 2 - Secondary program loader**

- the Secondary program loader will initialize the memory controller and other parts of the system needed for the **Tertiary program loader**.
- Once it's initialized, the Secondary programmer will load the **Tertiary Program Loader** into DRAm.

**Phase 3 - TPL**

- this is the full bootloader (U-boot for example)
- There'll usually be a cli that lets you perform maintenance, alike loading kernel images.
    - There's a way to load the kernel automatically.

- In the phase, the bootloader will usually load the kernel image and compiled device tree binary into DRAM, and pass the addresses of the two to the kernel.

**Phase 4 - Kernel initialization**

- In this phase, the kernel will initialize low level devices, like the MMU.

**phase 5 - `start_kernel()`**

- Here, the kernel will call `start_kernel()`, which will:
    - initialize the kernel core (memory, scheduling, interrupts, etc.)
    - Initialize statically compiled drivers
    - Mount root filesystem
    - Execute the first process, `init`

## Building Linux embedded system, intro and three approaches

**intro**

- When building an embedded linux system, you need to:
    - select a cross toolchain: assembler, compiler, libraries, etc.
    - Select the different packages that run on the target (bootloader, kernel, root filesystem)
    - Install the compile goods on the device.

**approaches**

1. **Manually compile everything**. It gives you total control, but it's tedious and harder to reproduce builds on other machines.
2. **Use complete distribution** (ex: ubuntu/debian): easy to do, but hard to customize
3. **Use a build frame**, like Buildroot or Yocto. You can customize and reproduce builds easily. 