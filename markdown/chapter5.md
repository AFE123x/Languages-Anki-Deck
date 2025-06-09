# 5. Building a Root Filesystem

## What should be in the root filesystem?

- After the bootloader runs the kernel, the kernel gets a root filesystem.
    - The bootloader wil either give a pointer to it from the bootloader, or mount the block device given by the `root` parameter.
    - From the point, it'll run things like an init system, shell, etc.


- The minimal root filesystem requires the following components:
    - init: the first program to run, initializing other systems
    - Shell: give you a command prompt, but to allow shell scripts called by init to run
    - daemons: background programs that provide services to others.
    - shared libraries: most programs are linked with shared libraries.
    - configuration files: configuration for init and other daemons.
        - Usually a series of text files in the /etc directory.
    - device nodes: special files that give access to various drivers.
    - /proc and /sys, which represents the kernel data structures as a heirarchy of directories and files.
    - kernel modules

## rootfs - directory layout

- The init system is expecting a particular layout of files.
    - If you're using a different version of the init system, or you're using your own, you're more than free to do it however you want.

- Due to this ambiguity, there required a standardization, as programs will expect certain files to be in certain places.
- This gave birth to the **filesystem hierarchy standard (FHS)**

- Embedded devices usually include the following:
    - `/bin`: programs essential to all users
    - `/dev`: device nodes and other special files
    - `/etc`: system configuration files
    - `/lib`: essential shared libraries
    - `/proc` the proc file system
    - `/sbin`: Programs essential to the system administrator
    - `/sys` the sysfs filesystem
    - `/tmp`: A place to put temporary or volatile files
    - `/usr`: additional programs, libraries and sys admin utilities
        - located in `/usr/bin`, `/usr/lib` and `/usr/sbin` respectively
    - `/var`: A heirarchy of files and directories that can be modified at runtime.


## POSIX File Access Permissions

- Every process, for example, will beling to a user and one or more groups.
    - We identify this user with a 32 bit number known as the User ID, or UID.
    - The mapping between the UID to a name and other informaiton are stored in `/etc/passwd`
    - We uniquely identify groups with a Group ID, or PID, where the information is stored in `/etc/group`

- There's a root user and root group
    - Their UID is 0, and GID is 0 respectively.
    - The root user is the super user, as it can bypass most permision checks and access all resources in the system.

- Each file and directory has an owner, belonging to one group.
    - The permissions a process has to a file is controlled by a set of access premission flags, also known as the mode of the file.
    - The permission flag consists of 3 groups of 3 bit numbers.
        - The first collection is the owner's permission to the file.
        - The second collect is the member in the groups access to the file
        - The final group is everyone else

```
rwx rwx rwx
usr grp wrld
```

### Additional bits

- SUID (4): when a file is executed, it changes the UID to the owner of the file.
- SGID (2): when a file is executed, it changes the GID to the group the file it belongs to.
- sticky (1): restricts file deletion, so a user can't delete files owned by another user.

```bash
sudo chmod 4755 bin/ping #will 
```
- Here, we change the SUID

## File ownership permissions

- There are a lot of sensitive files that you don't want other processes to touch
    - for example, `/dev/mem` is the memory file, and you don't want people to touch that.
- Therefore, you should give such sensitive files root access only

```bash
sudo chown -R root:root *
```
- This changes the ownership to `*` to be owned by the root user and root group.
- `-R` indicates recursion.

## BusyBox - influence on embedded

- The shell lets us lounch other programs.
    - In order to launch said programs, we need to store them.

- This has two issues:
    - 1: You need to find all the utilities to cross compile
    - 2: This will likely take too much space.

- BusyBox was originally invented to work on a 1.44 MB floppy disk, which served useful in the embedded linux community.
    - BusyBox rewrote these utilities, which are cut back, but do the job.


```bash
busybox cat my_file.txt
```
- This is the busybox equivalent to the `cat` command.
- what you can do is create a symbolic link from /bin/cat to /bin/busybox, so you can do the same thing.

## Reducing size by stripping .so

- You can remove unecessary parts of shared libraries symbol table to save on space

```
strip rootfs/lib/libc2.22.so
```
- This will remove:
    - debug symbols
    - relocation information
    - comments

- This can cut the binary size significantly.

## Device Nodes

- In unix, everything is a file (except network interfaces).
- This include devices.

- To create a node, you use `mknod`

```bash
mknod <name> <type> <major> <minor>
```
- `name` is the name of the device node you want to create
- `type` is for either c for character or b for block device
- major and minory are pairs of number to identify the appropriate device driver code.

- BusyBox only needs two nodes to boot:
    - `/dev/console`: which should have the permission 600
    - `/dev/null`: which should have permission 666

```
cd ~/rootfs
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1
```

- To remove a device node, you can use `rm`, as it's just a normal file.

## Proc and Sysfs

### procfs

- `proc` and `sysfs` are two **pseudo filesystems**, that gives a window into the inner workings of the kernel.
    - Kernel data is represented as a heirarchy of directories

- THese two file systems give you the means to interact with the kernel.

- To mount these filesystems

```bash
mount -t proc proce /proc
mount -t sysfs sysfs /sys
```

- `/proc` was made to expose information about processes to user space.
    - Each PID will have be a directory in `/proc`
    - Each file in the directory will give information.
        - `/proc/cpuinfo` tells you about the cpu
        - `/proc/interrupts` has information about interrupts

- `/proc/sys` will contain files that control the state and behavior of the kernel subsystem.

### sysfs

- sysfs is meant to present the kernel driver model to userspace.


## Mounting filesystems and virtual filesystems

- To mount a filesystem, you use the `mount` command

```cs
mount [-t vfstype] [-o options] device directory
mount -t ext4 /dev/mmcblk0p1 /mnt
```
- In the example command, you can access the cotnents of `/dev/mmcblk0p1` by going into the `/mnt` directory.

### Mounting virtual file systems
```
mount -t proc procfs /proc
mount -t proc nodevice /proc
```
- These two do the exact same thing.


## Transferring root filesystem to target

- After creating the skeleton root filesystem in your staging directory, the next task is to transfer it to the target.
- There are three options:
    - **initramfs**: filesystem image that's loaded into ram by the bootloader.
    - **disk image**: copy of the root filesystem formatted and ready to be loaded onto the mass storage device on the target.
    - **network filesystem**: You can wirelessly mount it, great for prototyping

## Creating/booting boot initramfs

- The initramfs is a compressed cpio archive
    - This is similar to tar and zip, but easier to decode.

- There are three ways to create a boot ramdisk
    - as a standalone cpio archive
    - as a cpio archive embedded in the kernel image
    - as a device table, which the kernel build system processes as part of the build.


### booting initramfs

- the easiest thing to do is run a shell on the console.
    - We can add the `rdinit=/bin/sh` to run the kernel command line.

## creating standalone initramfs

- To create the standalone cpio, you would do the following

```bash
$ cd ~/rootfs
$ find . | cpio -H newc -ov --owner root: root > ../initramfs.cpio
$ cd ..
$ gzip initramfs.cpio
mkimage -A arm -O linux -T ramdisk -d initramfs.cpio.gz uRamdisk

- 
```

## Building initramfs into kernel image

- Some bootloaders wil not be able to load the initramfs as a separate file.
- Therefore, you'll have to integrate the file within the kernel.
    - To do this, you set the CONFIG_INITRAMFS_SOURCE to the full path of the cpio archive you created.

## Building an initramfs using device table

- A device table is simply a text file that lists the files, directories, device nodes and links that go into an archive or filesystem image.
- Once you create this file, you can set the option, CONFIG_INITRAMFS_SOURCE, and set it to the path of the device table.

```
dir <name> <mode> <uid> <gid>
file <name> <location> <mode> <uid> <gid>
nod <name> <mode> <uid> <gid> <dev_type> <maj> <min>
slink <name> <target> <mode> <uid> <gid>
```

- There is a script that makes it easy to do this in the kernel source code known as `scripts/gen_initramfs_list.sh`.


## Configuring Init program

- The init program will first read a configuration file, `/etc/inittab`.

```
::sysinit:/etc/init.d/rcS
::askfirst:-/bin/ash
```
- the firts line will run a shell script, rcS.
- The second line will write a message.
    - It will shart the ash shell when you press enter.


### Startnig daemon processes

- In the inittab, you can do this with respawn:

```bash
::respawn: /sbin/syslogd -n
```

## Init Program, configuring user accounts

- You don't want to run everything as root, as that'd cause a security vunerability.
- Instead, you want to create unpriviledged user accounts, and use them where full root is not necessary.
- usernames are cofigured in `/etc/passwd`, which contains metadata about the user

- Below is a sample content of `/etc/passwd`
```
root:x:0:0:root:/root:/bin/sh
daemon:x:1:1:daemon:/usr/sbin:/bin/false
```
- `root` is the username
- `x` means the password is stored somewhere else
- `0` is the user id
- `0` is the group id
- `root` is the name again
- `/root` defines the start folder
- `/bin/sh` defines what login interface to use.


## Utilities for managing device nodes

- Constantly using mknod is cumbersome. There are other ways to create them automatically
    - devtmpfs: this is a pseudo filesystem that mounts over /dev at boot time.
    - mdev: busybox applet that will populate a directory with device nodes based on a config file `/etc/mdev.conf`.
    - `udev`: mainstream version of mdev, flexible and good for higher end embedded devices. It's a part of systemd.


## Creating filesystems images with device tables

- You can create the initramfs using a device table. 
- this is great for creating device nodes without being root.

- Embedded developers also want to:
    - customize device nodes and permissions
    - create images for ext2, jiffs2, and ubifs filesystem
    - build rootfs without sudo.


- There are many filesystem tools that support device tables
    - For example:
        - `mkfs.jpps2`, commonly used for raw flash memory
        - `mfks.ubifs`, used for NAND flash
        - `genext2fs`, used for SD cards, MMC, managed flash.


## device table format

```
<name> <type> <mode> <uid> <gid> <major> <minor> <start> <inc> <count>
```
- name is the path of the file `/dev/null`
- type is for the type of device:
    - f for file
    - d for directory
    - c for character device
    - b for block device
    - p for pipe
- mode is the file permission (755, 666)
- uid, gid is the user and group id
- major and minor are device numbers
- start, inc, count: For creating multiple device nodes in sequence.

## Creating image from staging directory and device table

```bash
genext2fs -b 4096 -d rootfs -D device-table.txt -U rootfs.ext2
```

- `-b 4096` is the size of the image in bytes
- `-d rootfs` is the staging area, that contains `/bin`, `/etc`, and `/dev` files
- `-D device-table.txt` is the path to the device table file
- `-U` preserves UID and GID
- `rootfs.ext2` is the output file