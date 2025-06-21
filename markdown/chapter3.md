# 3. The Simplest Drivers

## What is a driver?

- The linux kernel separates user applications from the underlying hardware.
    - You cannot access periphirel registers, or ram directly
    - The MMU enforces this.
- Instead, drivers will interact with the hardware on behalf of the userspace applications.

## What is a Kernel Module? Static vs. Dynamically loaded module?

- A kernel module is simply an extension of the kernel.
    - AKA, a kernel module **isn't always a driver**.

- A static module is one built into the kernel image.
- Dynamic modules can be loaded, tested and unloaded without rebooting the kernel.

- The kernel modules are usually located in `/lib/modules/<kernel_version>/` on the root filesystem.

## Kernel module, `init()`/`exit()` function

- `init()` is called when a module is loaded.
    - You'd allocate items here.
- `exit()` is called when a module is unloaded.
    - You free items here

- To register the functions, you'd use the `module_init()` and `module_exit()` macros.

## Kernel module - licensing

- You need to specify the specific license when creating a module.
- You use the `MODULE_LICENSE` macro for this.

- The kernel will panic if the license selected isn't a GNU license. 
- In fact, the kernel may refuse to call certain APIs without i


## hello world kernel module - c code

```c
#include <linux/module.h>
static int hello_init(void){
    pr_info("Hello world init\n");
    return 0;
}

static void hello_exit(void){
    pr_info("Hello world exit\n");
}
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AFE123x");
MODULE_DESCRIPTION("This is a print out Hello World module");
```

## hello world kernel module - Makefile

```Makefile
obj-m += hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```


## Adding parameters

- You can add parameters to your kernelmodule using `#include<linux/param.h>`

```c
module_param(name, type, perm);
```

- name is the name of the parameter, the variable
- type is the type of variable.
- perm is the permission
    - `0644` is a standard permission.

```c
#include <linux/module.h>
static int num = 5;
module_param(num, int, S_IRUGO);
static int __init hello_init(void){
    pr_info("parameter num = %d\n", num);
    return 0;
}
static void __exit hello_exit(void){
    pr_info("Hello world with parameter exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AFE123x");
MODULE_DESCRIPTION("This is a module that accepts parameters");
```

## getting time elapsed in module

- you can get the amount of time in seconds that passed since a driver was loaded using `do_gettimeofday()`
    - `do_gettimeofday()` will populate a `struct timeval` with seconds and microseconds

```c
#include<linux/time.h> /* contains modules */
struct timeval{
    __kernel_time_t tv_sec; /* seconds */
    __kernel_suseseconds_t tv_usec; /* microseconds */
}
```


### Sample usage

```c
#include <linux/module.h>
#include <linux/time.h>
static int num = 10;
static struct timeval start_time; /* global variable */
module_param(num, int, S_IRUGO);

static void say_hello(void){
    int i;
    for (i = 1; i <= num; i++)
    pr_info("[%d/%d] Hello!\n",i,num);
}

static int __init first_init(void){
    do_gettimeofday(&start_time);
    pr_info("Loading first!\n");
    say_hello();
    return 0;
}

static void __exit first_exit(void){
    struct timeval end_time;
    do_gettimeofday(&end_time);
    pr_info("Unloading module after %ld seconds\n",
    end_time.tv_sec - start_time.tv_sec); /* get elapsed time */
    say_hello();
}
module_init(first_init);
module_exit(first_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alberto Liberal <aliberal@arroweurope.com>");
MODULE_DESCRIPTION("This is a module that will print the time \
since it was loaded");
```