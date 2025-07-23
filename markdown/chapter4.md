# 4. Character Device Drivers

## what is a character device?

- Character devices are devices that transfer data to or from a user application by means of **characters**, in a stream like manner (one byte after another)
- Character drivers expose the properties and functionalities of a device with a special file in the `/dev` directory. 

- character devices are represented in the kernel by the means of the `cdev` type

```c
struct cdev{
    struct kojbect kobj;
    struct module* owner;
    const struct file_operations* ops;
    struct list_head list;
    dev_t dev;
    unsigned int count;
}
```


## `ls -l` - file types

- in linux (inspired by UNIX), everything is a file.

- when you write ls -l, you'll get something like this:

```bash
ls -l /dev
crw-rw-rw-      3,3 root      16 Jul 17:34 󰡯 zero
# [...]
brw-r-----      1,5 root      16 Jul 17:34 󰡯 disk1s1
brw-r-----      1,8 root      16 Jul 17:34 󰡯 disk1s2
brw-r-----      1,6 root      16 Jul 17:34 󰡯 disk1s3
brw-r-----      1,7 root      16 Jul 17:34 󰡯 disk1s4
```

- the first character represents the file type

| character | meaning |
| --- | --- |
| `c` | character device file |
| `b` | block device file |
| `l` | symbolic link |
| `d` | directory |
| `s` | socket |
| `p` | pipe |

## `ls -l` - major minor numbers, `dev_t` attribute

```bash
ls -l /dev
crw-rw-rw-      3,3 root      16 Jul 17:34 󰡯 zero
# [...]
brw-r-----      1,5 root      16 Jul 17:34 󰡯 disk1s1
brw-r-----      1,8 root      16 Jul 17:34 󰡯 disk1s2
brw-r-----      1,6 root      16 Jul 17:34 󰡯 disk1s3
brw-r-----      1,7 root      16 Jul 17:34 󰡯 disk1s4
```

- you can see `3,3`, `1,5`, etc.
    - these are the major and minor numbers

- the major number identifies the device driver controlling the device
- the minor number identifies the unique device itself.


```c
struct cdev{
    // [...]
    dev_t dev;
    // [...]
}
```

- The dev_t type is an unsigned 32 bit number:
    - The major number's represented with 12 bits
    - The minor number is represented with the other 20 bits.

## device number allocation

- There are two ways to allocate device numbers:
    - statically: using `register_chrdev_region()`, where you need to guess a major number not used by another driver.
    - dynamically: using `alloc_chrdev_region()` to obtain a valid device number


## device number allocation - statically

- you can use `register_chrdev_region()`, to statically allocate a character device (you guess the major and minor number)

```c
int register_chrdev_region(dev_t first, unsigned int count, char* name);
```
- here, you need to guess the major and minor number.
- `first` is the major and minor number combined. You can use MKDEV(major, minor) for this.
- `count` is the number of consecutive device numbers needed
- `name` is the name of the associated device or driver.
- returns 0 on success, negative number on failure.

## device number allocation - dynamically

- you can use `alloc_chrdev_region()`, which will find the major and minor number for us.

```c
int alloc_chrdev_region(dev_t* dev, unsigned int firstminor, unsigned int count, char* name);
```

- `dev` will be filled with the major and minor number.
- `firstminor` is the first of the request range of minor numbers
- `count` is the number of minor numbers you need
- `name` is the name of the associated device or driver.

## file operations

- file operations are operations you can do on a file, like reading, writing. etc.
- the operation is a structure called `struct file_operations` containing function pointers to each function.

```c
struct file_operations {
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
    int (*readdir) (struct file *, void *, filldir_t);
    unsigned int (*poll) (struct file *, struct poll_table_struct *);
    int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
    int (*mmap) (struct file *, struct vm_area_struct *);
    int (*open) (struct inode *, struct file *);
    int (*flush) (struct file *);
    int (*release) (struct inode *, struct file *);
    int (*fsync) (struct file *, struct dentry *, int datasync);
    int (*fasync) (int, struct file *, int);
    int (*lock) (struct file *, int, struct file_lock *);
    ssize_t (*readv) (struct file *, const struct iovec *, unsigned long, loff_t *);
    ssize_t (*writev) (struct file *, const struct iovec *, unsigned long, loff_t *);
};
```

- you don't need to implement each operation, so you can leave some out.


## file kernel representation

### inode type
- the kernel describes files as an instance of `struct inode`
- the inode is a filesystem data structure holding information about a file (like it's type).

### file type

- there's also a `struct file` structure.
- this describes an open file in the kernel that relies on the lower level inode structure.


## How to allocate and register character device

1. reserve a major and range of minor numbers with the `alloc_chrdev_region()` function.
2. create a class for device using `class_create()`
3. create the struct file_operation, then call cdev_init() and cdev_add() to register device
4. use device_create(), and give it a proper name, giving us the file file in the `/dev` directory.

[example](https://github.com/PacktPublishing/Linux-Device-Drivers-Development/blob/master/Chapter04/dummy-char.c)

## exchanging data between kernel space and user space

- In your character device, you may want to add functionatily for the write and read method.

- The program reading/writing to the driver is likely a userspace level application
- There needs to be a bridge between userspace and kernel space.
- There are two functions for this: `copy_from_user()` and `copy_to_user()`

```c
unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);
```

`__user` indicates the pointer is untrusted.
- `to` is our destination buffer (where are we writing the data to?)
- `from` is our source buffer (where are we reading the data from?)
- `n` is the number of bytes to write/read.

## exchanging data between kernel and user space, single value copy

- there are two macros: `put_user` and `get_user`

```c
put_user(x, ptr); //copy x from kernel space to user space
get_user(x, ptr); //get a value from user space and store it in x
```

- these macros return 0 on success and -EFAULT on error.

## Implementing open method

- the `open` method is called everytime someone opens your device file.

```c
int (*open)(struct inode* inode, struct file* filp); //function prototype for open
```

- in the inode argument, it will contain a field, `i_cdev`, which contains our device-specific data.
    - we can use the container_of macro to get device data

```c
struct pcf2127 {
    struct cdev cdev; //example cdev type
    unsigned char *sram_data;
    struct i2c_client *client;
    int sram_size;
    // [...]
};
```

- in the open function, you'll usually allocate memroy as needed, and turn on the device if supported.

## implementing release method

- this is similar to the `open` function, but we'll be freeing any memory used and shut down the device is supported/needed.

```c
int (*release)(struct inode* inode, struct file* filp);
```

## Implementing read and write method

```c
ssize_t(*write)(struct file *filp, const char __user *buf, size_t count, loff_t *pos);
ssize_t (*read) (struct file *filp, char __user *buf, size_t count, loff_t *pos);
```
- `buf` is the buffer from userspace to write/read from.
- `count` is the number of bytes to read/write
- `*pos` is the start position from which data should be read in the file.

## implementing llseek method

```c
loff_t(*llseek) (struct file *filp, loff_t offset, int whence);
```

- you return the new position in the file.
- `loff_t` is an offset defining how much to change from the current position
- `whence` defines where to read from.

## poll - introduction and implementing method

- you'd use the poll function to implement a passive wait.
- the poll function is called whenever the user space program performs `select` or `poll`.

```c
unsigned int (*poll) (struct file *, struct poll_table_struct *);
```

- in this method, you'd likely use `poll_wait()`

```c
void poll_wait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p)
```

- the poll wait function adds the device associated with the file structure to a wait queue.
    - the task will then wake up depending on the event registered in the poll_table structure.

### implementing poll

1. Declare wait queue for each event type (read, write, exception)
2. implement poll function using poll_wait.
3. notify waitqueue when there's new data or device is writable.

## Implementing `ioctl` in Linux - creating commands

The `ioctl` system call allows user-space programs to send control commands to device drivers for actions beyond standard I/O.

### ioctl Command Format

The command is a 32-bit number encoding:

* **Magic number (8 bits):** Identifies the device type.
* **Command number (8 bits):** Unique per command.
* **Direction (2 bits):** Read, write, or both.
* **Size (14 bits):** Size of the data type.

### Helper Macros

Linux provides macros to define these commands:

* `_IO(magic, cmd)`: No data transfer
* `_IOR(magic, cmd, type)`: Kernel returns data
* `_IOW(magic, cmd, type)`: Kernel receives data
* `_IOWR(magic, cmd, type)`: Bi-directional transfer

These macros ensure uniqueness and proper formatting.

## Implementing `ioctl` in Linux - writing method

```c
long ioctl(struct file *f, unsigned int cmd, unsigned long arg);
```
- basically, all you need to do is have a massive switch case statement, and handle whatever command is being used?