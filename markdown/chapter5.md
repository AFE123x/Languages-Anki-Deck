# 5. Building a Root Filesystem

## Building File System - RYO approach

- RYO, **roll your own**, is the approach to create the filesystem from scratch.
- It was the standard approach in the early days, but is now only used when ram and storage is very limited.

## How does the kernel get the rootfs? After loading root fs, init

- The kernel will get a root filesystem, either:
    - as an initramfs passed as a pointer by the bootloader
    - by mounting the block device given on the kernel command line
- Once the kernel has the root filesystem, it'll execute the first program, normally known as **init**.

- **init** is responsible for starting other programs, bringing the system to life.

## Components of minimal root fs

- init progroam
- shell
- daemons: background program providing services to other programs
- shared libraries
- configuration files
- device nodes
- virtual filesystems: proc/sysfs
- kernel modules

## rootfs - Filesystem Hierarchy Standard

- The **Filesystem Heirarchy Standard** defines the basic layout of a linux system.
- embedded linux usually will include the following:

| directory | purpose |
| --- | --- |
| `/bin` | Programs essential for all users |
| `/dev` | device nodes and other special files |
| `/etc` | system configuration files |
| `/lib` | essential shared libraries |
| `/proc` and `/sys` | procfs and sysfs virtual filesystems |
| `/sbin` | programs for sysadmins |
| `/tmp` | place to put temporary or volatile files |
| `/usr` | contains additional binaries, libraries and sysadmin utilities |
| `/var` | heirarchy of files and directories that can be modified during runtime |


## User and Group IDs

- On unix systems, every process will belong to a user and one/more groups.
- We uniquely identify users and groups with a 32 bit number known as the **User ID (UID)** and **Group ID (GID)**.
- `/etc/passwd` maps a UID to a name.
- `/etc/group` provides information about groups.

- The root user, also superuser, has a user id of 0.
- the root group has a group id of 0. 

## File permission

- Each file and directory  will have an owner and belong to one group.
- We have the **mode** of the file, which controls the access a process has to a file/directory.
- We define these with three collection of bits: `RWXRWXRWX`
    - R is read, W is write and X is execute permissions.

- There are three groups: Owner permissions, Group permissions, World permissions.

## File permission - additional file permission group

- SUID: Runs the program with the file owner's user ID, not the caller's.
- SGID: Runs the program with the file's group ID, not the caller's group.
- Sticky Bit: Only the owner can delete or rename files in the directory, even if others have write access.

## File Ownership

- Ideally, you want to restrict sensitive resources to only be accessible to the root.

- You can set the permission with `chown`

```bash
$ sudo chown -R root:root *
$ sudo chown <-R for recursive> user:group <filename> 
```

## Programs for Root file systems

- You'll primarily need three main groups of programs:
    - init system: first program that runs
    - shell: program that let's us interact with system
    - utilities: different tools, like ls, grep, etc.

## Adding libraries to rootfs

- One could easily just copy all the libraries, but it can take up quite a lot of space.
- Instead, what if we just take the libraries we need?

- We can use readelf on an executable to see what shared libraries are needed

```bash
readelf -a /usr/bin/ls | grep "Shared library"
```

- From there, we can easily copy and past them in our rootfs.

## stripping shared libraries

- Libraries and programs are compiled with a symbol table, meant to help with debugging and tracing.
- You don't need these in production, so you can get rid of them.
- This is done with the strip utility

```bash
strip sharedlib.so
```

- This will make your shared library smaller.

## Device nodes

- Besides network devices, most devices are represented as a file.
- These files are known as **device nodes**

- To create a device node, we can use the `mknod` utility

```bash
mknod <name> <type> <major> <minor>
```
- name: name of device
- type: `c` for char, `b` for block.
- major/minor: numbers used by kernel to route file requests to drivers.

## Proc and SysFS

- These are two pseudo filesystems that provides a window into the inner working of the kernel.
- You can interact with the kernel and drivers with this, given sufficient permissions.

- Proc provides information about processes to user space.
- In addition, there are files that give information about parts of the kernel, like `/proc/cpuinfo`.

- Sysfs presents the kernel driver model to user space.
- It resembles a hierarchial structure relating to devices and the drivers the way they're connected to each other.

## Mounting Filesystem with `mount`

- `mount` lets you attach a filesystem to a directory within another filesystem.

```bash
mount [-t vfstype] [-o option] device directory
```
`-t` specifies the type of filesystem, like proc, sysfs, ext4, etc.
- `-o` is additional options
- `device` is the block device
- `directory` is where you want to mount the device

```bash
mount -t proc procfs /proc
mount -t proc nodevice /proc
```
- `procfs` and `nodevice` are ignored by the mount command.


## Kernel Modules and Filesystems

- When you build and install linux kernel modules, they'll be stored (.ko, module.dep files, etc.) in `/lib/modules/<kernel-version>`
- When you update the kernel, you need to update the modules as well, otherwise, you'll have a filesystem kernel conflict.

## Transferring rootfs to target

- There are three main approaches:
    - initramfs
    - Disk image
    - Network file system

## What is the actual initramfs file? What are the three different ways to create the initramfs

- The Initial RAM Filesystem, initramfs, is a compressed **cpio** archive.
- There are three ways to create a boot ramdisk:
    - standalone archive
    - cpio archive embedded in the kernel image
    - device table, where the kernel build system processes


## creating boot initramfs - Standalone initramfs

- Once you finish setting up the rootfs (creating directories, compiling utilities), you can archive it

```bash
cd ~/rootfs
find . | cpio -H newc -ov --owner root:root > ../initramfs.cpio
cd ..
gzip initramfs.cpio
mkimage -A arm -O linux -T ramdisk -d initramfs.cpio.gz uRamdisk
```
- `cpio -H newc -ov --owner root:root`, cpio creates the cpio archive: 
    - newc is a type of CPIO format
    - `o` means it's making output, `v` means verbose
    - `--owner root:root` gives permission to root group and user
- `gzip initram.cpio` will compress the `.cpio` file into a `cpio.gz` file.
- `mkimage -A arm -O linux -T ramdisk -d initramfs.cpio.gz uRamdisk`: adds a uboot header to file
    - `-A arm` defines architecture
    - `-O linux` defines the OS is linux
    - `-T ramdisk` defines this image is a ramdisk
    - `-d initramfs.cpio.gz` is the input file
    - `uRamdisk` is the output file.

## Creating boot initramfs - Building into kernel image

- Instead of making a separate initramfs file, you can build it into the kernel.
- This is done with a kernel configuration, CONFIG_INITRAMFS_SOURCE, where you'd specify the path of the `.cpio` file you made. 

## Building initramfs using device table.

- A device table is simply a text file that lists the files directories and links that go into an archive or filesystem image.
- The kernel will create the cpio on boot time.

## Init Program

- When a UNIX system is booted up, it'll first call a program named `init`.
- configuring it varies by init systems. For example, busybox has it's config in the `/etc/inittab` folder.
- The init program will start **daemons**, which are processes that run in the background.

## Configuring user accounts

- You don't want to run all programs as root.
- user names are configured in `/etc/passwd`
- group names are configured in `/etc/passwd`

## device nodes - improved `mknod`

- mknod, unless you want to save time by not created device nodes, shouldn't be used.
- instead, you should use one of the following:
    - `devtmpfs`
    - `mdev`
    - `udev`

## creating device nodes - `devtmpfs`

- this is a pseudo file system that's mounted over `/dev` at boot time
- the kernel populates it with nodes for devices the kernel knows about, and create new nodes for devices as they're detected at runtime.


## Creating device nodes - `mdev` and `udev`

- This is a busy box applet that populates the `/dev` directory with device nodes, as instructed by the configuration file.

- udev is the mainstream version of it. 

