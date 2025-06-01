# 17. Devices and Modules

## Device Types

- In linux, similar to all unix sytsems, we classify devices as one of three types:
    - Block device.
    - Character device.
    - Network device.
    - misc devices
- Block devices are addressable in blocks, and support seeking (the random access of data).
    - examples: hard drives, discs, memory devices
- character devices provide access to data only as a stream of characters (bytes).
    - example: keyboard, mice, printers
- network devices provide access to a network.
- misc devices, are a simplified form of a character device.

## Virtual device drivers

- Virtual device drivers are drivers for **pseudo devices**.
    - Examples of these are random number generator (/dev/random for example)

## Modules

- modules are loadable kernel objects.
    - This means that you can load, and unload kernel code.

## write a hello world kernel module

```c
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
/**
  * This function is called when you insert the module using insmod
  * @return It returns zero if it succeeds, non-zero otherwise
  */
static int initboi(void){
	printk(KERN_ALERT "You didn't do you taxes\n");
	return 0;
}
/**
  * Function called when rmmod is used
  *
  */
static void goodbye(void){
	printk(KERN_ALERT"AUF WIEDERSEHEN\n");
}

module_init(initboi); //assigns the function to use when module is initialized
module_exit(goodbye); //assigns the function to use when module is removed

MODULE_LICENSE("GPL"); //specify the license (required to compile)
MODULE_AUTHOR("ARUN FELIX"); //specify the author name (optional, but useful)
MODULE_DESCRIPTION("ontehu"); //a description of the module (optional, but useful)
```

## Write the makefile for a helloworld kernel module

```Makefile
obj-m += helloworld.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

- `obj-m` specifies the name of the kernel object file
- KDIR is the directory of the build folder of the kernel
    - `shell uname -r` specifies the specific kernel currently being used
- `-C $(KDIR)` calls the kernel build system
- `M=$(PWD)` specifies where to build the module
- `modules` is the keyword that you want to build a module.

## module parameters

- You can specify parameters when loading modules:

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

static int myint = 42;
static char *mystr = "default";

module_param(myint, int, 0644); //myint is an integer argument, with permission RW-R--R--
MODULE_PARM_DESC(myint, "An integer parameter"); //describes argument
module_param(mystr, charp, 0644);
MODULE_PARM_DESC(mystr, "A string parameter");

static int __init mymodule_init(void)
{
    pr_info("Hello, myint = %d, mystr = %s\n", myint, mystr);
    return 0;
}
/* exit funciton similar to before */

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");

```

- To load the module (after compiling)

```bash
sudo insmod mymodule.ko myint=123 mystr="custom"
```

- You can also pass in an array using `module_param_array(name, type, nump, perm);

```c
static int fish[MAX_FISH];
static int nr_fish;
module_param_array(fish, int, &nr_fish, 0444);
```

## Export sybols

- In user space, you have shared libraries.
    - shared libraries offer symbols explicitly exported.
- With kernel modules, you can export symbols, which can be accessed by other modules.
    - To do this, you can use the `EXPORT_SYMBOL()` macro.
    - If you only want GPL license modules to use the symbol, you can use the `EXPORT_SYMBOL_GPL()` macro.

## Device Model - introduction

- The linux kernel introduced a unified device model, which provided a single mechanism for representing devices and the topology of the system.

- This allowed for:
    - minimization for duplicate code.
    - Mechanisms for common facilities, like reference counting.
    - capability to link devices to their drivers and vice versa.

- The device tree was meant for powermanagement.
    - When you turn off the computer, you should turn off the leaves before the parents.

## Device Model - Kobjects

- The heart of the device model is the kobject, short for **kernel object**
- The Kojbect is represented by a struct, `struct kobject`.
    - The KObject, similar to the object class is java, has features like reference counting, name, parent pointer, allowing for the creation of a heirarchy.

```c
struct kobject{
    const char*             name;
    struct list_head        entry;
    struct kobject*         parent;
    struct kset*            kset;
    struct kobj_type*       ktype;
    struct sysfs_dirent*    sd;
    struct kref             kref;
    unsigned int            state_initialized: 1;
    unsigned int            state_in_sysfs: 1;
    unsigned int            state_add_uevent_sent: 1;
    unsigned int            state_remove_uevent_sent: 1;
    unsigned int            uevent_supress: 1;
}
```

- The name points to the name of the kobject
- the parent pointer points to the kobjects parent.
    - This builds the heirarchy in the kernel.
- ultimately, sysfs is a user space filesystem representation of the kobject object hierarchy in the kernel.

## cdev struct

- Kobjects alone aren't very useful, rather, they're embeded in other structures.
- One important structure is the cdev struct, which represents character devices

```c
struct cdev{
    struct kobject                  kobj;
    struct module*                 owner;
    const struct file_operations*    ops;
    struct list_head                list;
    dev_t                            dev;
    unsigned int                   count;
}
```

- Here, the Kobject is embedded in this structure, which enables the structure to become part of the object heirarchy.
    - You can access the parent point with `cdev->kobj.parent`

## Ktypes

- Within the Kobject type exists a `ktye`, which is the kernel oject type.
- The kernel object type defines the default behavior for a family kobject.

```c
struct kobj_type{
    void  (*release)(struct kobject *);
    const struct sysfs_ops  *sysfs_ops;
    struct attribute   **default_attrs;
}
```
- The release pointers is the destructor for when the reference counter reaches 0.
- sysfs_ops is a structure that defines the behavior of sysfs files on reads/writes.
- `default_attrs` define the default attributes of the kobject.
    - If the kobject is exported to sysfs, the attribute is exported as a file.

## Ksets

- The Kset is the kernel object set, which is an collection of kobjects.
- It basically groups multiple Kobjects, like all block devices, in the same place.
- `ktypes` do group devices together, but they also define the default behavior for these devices. `kset` just groups the devices together.

```c
struct kset{
    struct list_head              list;
    spinlock_t               list_lock;
    struct kobject                kobj;
    struct kset_uevent_ops* uevent_ops;
}
```

- list contains a linked list of all kobjects in the kset.
- list_lock is a spinlock protecting the entry from the list.
- `kobj` is a kobject representing the base class the set.
- uevent is a mechanism for communicating with user-space information about:
    - hotplugging of a device
    - hot removal of a device.

## kobject functions

- There are many functions available that let you mess with kobjects.
- If you're writing drivers, you probably will never use this.

### initializing kobject

- Kobjects are initialized with kobject_init, you pass in the kobj, then the ktype, which defines the default behaviors of the kobj.
```c
void kobject_init(struct kobject* kobj, struct kobj_type* ktype);
struct kobject* kobject_create; //shorter way to do it.
```

### Kobject reference count

you can increment/decrement the reference count with the following two functions:

```c
struct kobject* kobject_get(struct kobject* kobj); //increment counter.
void kobject_put(struct kobject* kobj); //decrements counter
```

## sysfs
- The sysfs filesystem is an virtual filesystem that provides a view of the kobject heirarchy.
- The original purpose of the koject heirarchy was power management, but turning the heirarchy into a VFS, allowed for debugging.
- sysfs is usually mounted on `/sys`


### sysfs contents

- Sysfs will usually contain 10 directories:
    - block, representing block devices
    - bus, representing the different system buses.
    - class directory, which has a view of the devices
    - devices, shows a view of the device topology.
    - firmware contains a tree fo the low level subsystems like ACPI.
    - fs contains a view of the registered file systems.
    - the Kernel directory has kernel configuration options.
    - modules contains all the system's loaded modules
    - power contains power management data.
    - devices, which exports the device model to the world.


## Adding/removing kobjects from sysfs

### Adding to sysfs

- You can add kobjects using  `kobject_add()`
- You can create the kobject and add it to sysfs at once using `kobject_create_and_add()`
```c
kobject_add(struct kobject* kobj, struct kobject* parent, const char* fmt, ...);
kobject_create_and_add(const char* name, struct kobject* parent);
```
### Removing from sysfs

finally, you can remove the kobject from sysfs using `kobject_del()`

```c
kobject_del(struct kobject* kobj);
```

## Adding Files to sysfs - introduction

- Kobjects alone only create the directory in `sysfs`.
    - it doesn't include the contents themselves.
Sure! Here's a **refactored and more concise** version of your explanation on adding default attributes to sysfs using `kobj_type`, `attribute`, and `sysfs_ops`:


## 🗂️ Adding Default Files to Sysfs via `kobj_type`

When working with `kobject`s and `kset`s, you can expose kernel data to sysfs using **default attributes** defined in the `kobj_type` structure.


### 🔧 `kobj_type` Structure

The `kobj_type` struct includes:

* `default_attrs`: an array of `attribute` structures representing default sysfs files.
* `sysfs_ops`: a set of callbacks to handle file I/O for those attributes.


### 📄 `struct attribute`

```c
struct attribute {
    const char *name;         // Name of the file in sysfs
    struct module *owner;     // Owning module (can be NULL)
    umode_t mode;             // Permissions (e.g., S_IRUGO for read-only)
};
```

Each `attribute` defines a file in sysfs with a name, owner, and permissions (e.g., `S_IRUSR`, `S_IWUSR`, `S_IRUGO`).


### ⚙️ `struct sysfs_ops`

```c
struct sysfs_ops {
    ssize_t (*show)(struct kobject *kobj, struct attribute *attr, char *buf);
    ssize_t (*store)(struct kobject *kobj, struct attribute *attr, const char *buf, size_t count);
};
```

* `show()` is called on read:

  * Copy the attribute’s value into `buf`.
  * Return number of bytes written or a negative error code.

* `store()` is called on write:

  * Parse the input buffer and update the attribute’s backing variable.
  * Return number of bytes consumed or a negative error code.


### 🧠 Summary

By populating `kobj_type.default_attrs` and `kobj_type.sysfs_ops`, you create default sysfs files for your `kobject` that allow user-space to read/write kernel data via standard file semantics.


## Adding files to sysfs - Creating new attributes

- Typically, the default attributes provided by the ktype associated with a kobject are sufficient.
    - Sometimes, it's better to add special attributes. 
    - This can be done with `sysfs_create_file()` interface.

```c
int sysfs_create_file(struct kobject* kobj, const struct attribute* attr);
```
- This function associates the attribute structure, attr, with the kobject, kobj.
    - On success, it returs 0. On failure, it returns a negative number.
- When you add new attributes, you should be responsible for storing the values.

- You can also create **symbolic links** with `sysfs_create_link()`

```c
int sysfs_create_link(struct kobject* kobj, struct kobject* target, char* name);
```
- This function will create a symbolic link between the two kobjects.

## Adding files to sysfs - destroying attribute

- You can destroy the attributes with `sysfs_remove_file()`

```c
void sysfs_remove_file(struct kobject* kobj, const struct attribute* attr);
```

- If you made a symbolic link, you can remove it with `sysfs_remove_link()`

```c
void sysfs_remove_link(struct kobject* kobj, char* name);
```

## sysfs conventions

- By convention, you should only export one value per attribute file.
- attributes should be simple, text-based, and map to simple C types.
- organize data in a clean heirarchy.
    - this means mapping aprent kobjects appropriately and intuitivetly.
- don't change existing files unless absolutely necessary. 


## Kernel Event Layer

- The kernel event layer implements a kernel to user notification system on top of kobjects.
- sysfs will send software signals indicating events, like "battery low".
- this interface will send events from kernel-space to user-space.
- D-Bus is an example daeman that implements a systemwide messaging bus.

## Kernel event layer - sending events

- You can send events out to user space using `kobject_uevent`

```c
int kobject_uevent(struct kobject* kobj, enum kobject_action action);
```
- The first parameter is the kobject sending the signal.
- The action is the action parameter.
    - There's an enum `kobject_action`.