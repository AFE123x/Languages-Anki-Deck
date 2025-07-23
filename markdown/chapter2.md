# 2. Device Drivere Basis

## User Space vs. Kernel Space

- The CPU seperates processes by priviledge levels.
    - user apps are more restricted than the kernel.
- Kernel space provides a higher privilidge level than user applications (giving you access to the hardware).
- user space: restricted mode, need to use system calls to interface with device.

## What is a module

- You can think of a module similar to a plugin to a user software.
- the linux lets you do plug and play the modules.

## Module Dependencies

- you can export symbols (functions) using the `EXPORT_SYMBOL` macro, making it available to other modules.

- If you have two modules, A and B, a dependency of module B on module A means that module B is using one of the symbol exported by module A.

## loading modules manually

- there are two ways to load modules:
    - insmod: simple way to load modules.
    - depmod: it will read the modules.dep file, and add the dependencies before adding module.

## loading modules via boottime

- you can create a file, `/etc/module-load.d/filename`, to load modules at boot time.
- here, you can list the modules you want to load.

## unloading odules

- you can unload modules via two approaches:

```bash
rmmod -f mymodule # will forcefully remove a module
modeprobe -r mymodule # remove unused dependencies
```

## simple hello world kernel module

```c
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

static int __init helloworld_init(void){
    pr_info("Hello, world!\n");
    return 0;
}

static void __exit helloworld_exit(void){
    pr_info("End of the world!\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_AUTHOR("Arun Felix");
MODULE_LICENSE("GPL");
```

## `__init` and `__exit` macros

- These are kernel macros that tell the linker to place the code in a dedicate section in the object file.

```c
#define __init __section(.init.text)
#define __exit __section(.exit.text)

static int __init helloworld_init(void){
    pr_info("Hello, world!\n");
    return 0;
}

static void __exit helloworld_exit(void){
    pr_info("End of the world!\n");
}

```

## Module information, extracting information

- you'd want to provide information about the module, like the author, license, description and other things.

- There are a bunch of macros:
    - `MODULE_DESCRIPTION("description)` provides the description of the module
    - `MODULE_AUTHOR("author")` provides the author
    - `MODULE_LICENSE` provides the license the module uses (GPL for example).
    - `MODULE_INFO(field_name, "information about field ")` for miscellanious things.


- you can extract this information using `objectdump -d -j .modinfo`, or you can use the `modinfo`

## Module licenses

- This is important as this may restrict you from accessing certain libraries.
    - If you don't use a GPL license, you won't be able to access symbols (other functions) from other modules exported with `EXPORT_SYMBOL_GPL()`.


## Module Parameters

- Kernel modules can also accept arguments via the `module_param()` macro

```c
#include <linux/moduleparam.h>
// [...]
static char *mystr = "hello";
static int myint = 1;
static int myarr[3] = {0, 1, 2};
module_param(myint, int, S_IRUGO);
module_param(mystr, charp, S_IRUGO);
module_param_array(myarr, int,NULL, S_IWUSR|S_IRUSR); /* */
MODULE_PARM_DESC(myint,"this is my int variable");
MODULE_PARM_DESC(mystr,"this is my char pointer variable");
MODULE_PARM_DESC(myarr,"this is my array of int");
static int foo(){
    pr_info("mystring is a string: %s\n", mystr);
    pr_info("Array elements: %d\t%d\t%d", myarr[0], myarr[1], myarr[2]);
    return myint;
}
//[...]
```

