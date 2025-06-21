# 4. Character Drivers

## Device Nodes/Virtual files

- Since userspace applications can't access peripheral registers, we need a mechanism to transfer dete between kernel and userspace.
- The data transfer is done by **device nodes**, also known as **Virtual Files

- When a user reads from the node, the kernel will send data captured by the underlying driver into the application memory space.
- When the user writes to the device node, the kernel will copy the data stream provided by the application into the data buffer of the driver.

## Types of device drivers

- Linux supports three types of devices:
    - character devices
    - block devices
    - network devices.

### character devices

- These devices are written and road from without a buffer.
- Common examples include keyboards, monitors, etc.

### Block devices

- These devices are written to and read from in multiples of the block size.
- These are random accessed devices, so Hard Disk Drives for example
- Network devices are accessed with the socket interface. 

## File Operations

- In order to use `open()`, `read()` and `write()` system calls, you need to implement the operations in your driver. This is done with `struct file_operations`.

```c
struct file_operations {
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    int (*mmap) (struct file *, struct vm_area_struct *);
    int (*open) (struct inode *, struct file *);
    int (*release) (struct inode *, struct file *);
};
```

## Exchanging data between user space and kernel space

- The exchange is done with two functions:
    - `copy_from_user()`: copy data from userspace to kernel space
    - `copy_to_user()`: copy data from kernel space to user space


## implementing `read()`/`write()`

- The read and write functions return values are:
    - negative value on error (either -EINTR or -EFAULT error)
    - 0 or more indicates how many bytes were read/written

## major/minor number

- In Linux, all devices are uniquely identified with the **major** and **minor** number
    - The kernel groups devices into **major** numbers, and uses it to identify the correct driver to use when the device is accessed.
    - The **minor** number defines the specific instance of the driver


## `mknod` command

- To create a device node, you use the `mknod` command

```
mknod /dev/mydevice <c/b> <major> <minor>
```

- This isn't the right approach to create devices, and is better solely for learning purposes. 

## Registering and unregistering character devices: statically and dynamically

- You can statically allocate `count` character devices using `register_chrdev_region` and `unregister_chrdev_region`

```c
int register_chrdev_region(dev_t first, unsigned int count, char* name);
void unregister_chrdev_region(dev_t first, unsigned int count);

register_chrdev_region(MKDEV(my_major,my_first_minor),my_minor_count,"cool driver"); //example
```

- to do it dynamically, you can use `alloc_chrdev_region()`
    - Here, the major and minor number are dynamically chosen.
```c
int alloc_chrdev_region(dev_t* dev, unsigned baseminor, unsigned count, const char* name);
```

- `dev_t` is the type used to keep the identifiers of a device (both major and minor numbers.).
    - You can make this using the `MKDEV(major,minor)` macro.
- `baseminor` is the first of requested range of minor numbers
- `count` is the number of minor numbers requested.
- `name` is the name of the device driver.