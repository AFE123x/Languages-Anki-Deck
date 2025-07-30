# 13. The Linux Device Model

## What is the linux device model

- this introduced a device topology and organization with the Linux Device models.

- It introduced many things, like:
    - concept of classes, for devices of the same type.
    - communication with user space with sysfs.
    - Object Oriented like programming style to kernel.

## Linux Device Model - data structures

- There are various data structures:
    - buses are represented with the `struct bus_type`
    - device drivers are represented with `struct device_driver`


## Linux Device model - Bus representation

- The kernel represents the bus with the `struct bus_type` data type.

```c
struct bus_type {
    const char *name;
    const char *dev_name;
    struct device *dev_root;
    struct device_attribute *dev_attrs; /* use dev_groups instead */
    const struct attribute_group **bus_groups;
    const struct attribute_group **dev_groups;
    const struct attribute_group **drv_groups;
    int (*match)(struct device *dev, struct device_driver *drv);
    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);
    void (*shutdown)(struct device *dev);
    int (*suspend)(struct device *dev, pm_message_t state);
    int (*resume)(struct device *dev);
    const struct dev_pm_ops *pm;
    struct subsys_private *p;
    struct lock_class_key lock_key;
};
```

- this includes callbacks, like match (called when a new driver or device is added on bus), probe, etc.
- `const struct dev_pm_ops *pm;` contains a bunch of power management options.



## bus registration

- to registor a bus, you'd first populate the `bus_type` struct, then register it using `bus_register);`

## device driver representation, how to register

- We represent device drivers with the `device_driver` struct.
- it contains things like:
    - device name
    - bus type
    - callback functions like probe, remove, shutdown, etc.
    - match table for the kernel to identify device


- you register the device driver using `driver_register`


## device representation

```c
struct device {
    struct device *parent;
    struct kobject kobj;
    const struct device_type *type;
    struct bus_type *bus;
    struct device_driver *driver;
    void *platform_data;
    void *driver_data;
    struct device_node *of_node;
    struct class *class;
    const struct attribute_group **groups;
    void (*release)(struct device *dev);
};
```

- We represent devices using the `device` structure.
- it contains things, like a pointer to the device's parent, bus, and the type
    - it forms a heirarchy


## registering device

- to register to device, you use the `device_add()` function


## Linux Device Module - K structures

- There are three important structures:
    - `kobject`
    - `kobj_type`
    - `kset`


## kobject structure, general + initializing/freeing

- this is the core of the device model
- it provides the Object Oriented Programming style to the kernel.
    - It's mainly used for reference counts, and exposing the hierarchy and relationships within the kernel


```c
struct kobject {
    const char *name;
    struct list_head entry;
    struct kobject *parent;
    struct kset *kset;
    struct kobj_type *ktype;
    struct sysfs_dirent *sd;
    struct kref kref;
    /* Fields out of our interest have been removed */
};
```

- To initialize a kobject, you need to:
    - create an empty kobject using `kobject_create()`
    - initialize it using `kobject_init`
- to free a kobject, you need to use the `kobject_put()` function.
- This will make a directory in sysfs. Its specific location depends on the parent kobject's location.


## kobj_type struct

- this is the struct that defines the behavior of kobjects
- it controls what happens when the kobject is created/destroyed
- each kobject contains a `kobj_type`

```c
struct kobj_type {
    void (*release)(struct kobject *);
    const struct sysfs_ops sysfs_ops;
    struct attribute **default_attrs;
};
```

- within the `kobj_type`, we have a sysfs_ops struct, which contains a show and store function
- `default_attrs` contains a list of attributes belonging to the kobject.


## sysfs - attributes: structs and functions

- files in the sysfs directories are attributes, which represent an object property of the kobject.

```c
struct attribute{
    char* name; //name of attribute
    struct module* owner; //owner of module
    umode_t mode; //permission
}

int sysfs_create_file(struct kobject* kobj, const struct attribute* attr); //add file
void sysfs_remove_file(struct kobject* kobj, const struct attribute* attr); //remove file
```

## sysfs attributes, adding group of attributes

```c

struct attribute_group{
    struct attribute** attrs; //array of attributes
}
int sysfs_create_group(struct kobject* kobj, const struct attribute_group* grp );
int sysfs_remove_group(struct kobject* kobj, const struct attribute_group* grp );
```

- You'd create an attribute_group struct, and add the array of attributes.
    - Once you do that, you can create the group.


## device attributes struct, adding/removing

- device_attribute is a wrapping around attr, but contains two callback functions: show and store

```c
    struct device_attribute {
    struct attribute attr;
    ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
    ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
};

DEVICE_ATTR(_name, _mode, _show, _store);
```

- `show` is the callback when you read the attribute
- `store` is the callback when you write to the attribute.


```c
int device_create_file(struct device* dev, const struct device_attribute* attr);
void device_remove_file(struct device* dev, const struct device_attribute* attr);
```

## sysfs attributes - variants

- There's obviously the device_attribute variant, but there are more:
    - `bus_attribute`
    - `driver_attribute`
    - `class_attribute`