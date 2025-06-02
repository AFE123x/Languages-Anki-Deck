# 2. Learning About Toolchains

## What is a toolchain? Typical GNU toolchain setup

- A toolchain is the set of tools that compiles source code into an executable, which you can then run on you target device.
    - This will include the compiler, linker and run-time library.

- The compiler, linker and runtime library are needed to build the three elements of the linux system:
    - The bootloader
    - kernel
    - root filesystem.

### Typical GNU toolchain setup

- binutils: set of binary utilities including the assembler and linker.
- GNU Compiler Collection (GCC)
- C library: a standard API based on the posix specifications.
- kernel headers.
- GDB: A popular type of debugger


## Types of toolchains

- There are two types of toolchains:
    - native: The toolchain runs on the same type of system as the programs it generates
        - For example, creating an x86 application on an x86 device
    - cross: this toolchain runs on a different system then the target, allowing development to be done on a regular PC, then loaded on the target device for testing:
        - example, compiling code on a x86 machine for an stm32


## Deciding on a toolchain - CPU Architecture, compiler prefix

- When you select a toolchain, you need to consider:
    - CPU Architecture: ARM, MIPS, x86-64, etc.
    - Big/little endian operations
    - floating point support
    - Application Binary Interface (ABI): how arguments are conventionally passed into functions (rdi, rsi, etc.)


### Compiler Prefix

- GNU uses a convention when adding a prefix to the tools

```bash
<cpu>-<vendor>-<kernel>-<operatng system>
```
- The CPU is the architecture
- Vendor is the provider of the toolchain
- kernel is the OS (typicall linux here)
- operating system: could be the ABI, or something like gnu or musl.

```bash
gcc -dempmachine
```
- You can figure out the prefix by writing this command.


## Choosing the C Library, options

- There's a standard that IEEE established known as the POSIX standard.
- The C library is the implementation of that interface.

### Options

- glibc: This is the standard GNU C library.
    - It's big and most complete implementation of the POSIX API, but it's not very configurable.
- musl libc: newer library, great for systems with limited amount of RAM and storage.
- uClibc-ng (micro libc-ng): This was originally a microcontroller C library, made to work with micro linux. It now works with full linux.
- eglibc: eglibc is obsolete

## crosstool-NG

- This is a popular utility made to create a toolchain from source.
- To use it, you need to clone it from their repositary, and install it using make and stuff.

```bash
$ git clone https://github.com/crosstool-ng/crosstool-ng.git
$ cd crosstool-ng
$ git checkout crosstool-ng-1.22.0
$ ./bootstrap
$ ./configure --enable-local
$ make
$ make install
```
### Using crosstool-NG

- To see the default configuration for a device, you can add show before the prefix (arm-none-linux-gnueabi for example)

```bash
$ ./ct-ng show-arm-cortex_a8-linux-gnueabi
[L..] arm-cortex_a8-linux-gnueabi
 OS : linux-4.3
 Companion libs : gmp-6.0.0a mpfr-3.1.3 mpc-1.0.3 libelf-0.8.13 expat-2.1.0
 ncurses-6.0
 binutils : binutils-2.25.1
 C compilers : gcc | 5.2.0
 Languages : C,C++
 C library : glibc-2.22 (threads: nptl)
 Tools : dmalloc-5.5.2 duma-2_5_15 gdb-7.10 ltrace-0.7.3 strace-4.10
```

### Actually creating toolchain

- To create the toolchain, you can run ct-ng with the architecture you want

```bash
./ct-ng arm-cortex_a8-linux-gnueabi
./ct-ng menuconfig #lets you configure the buildtool.
./ct-ng build #when you're ready, you can build it
```

## Using toolchain

- After installing, you can use the toolchain.

### gcc cross compiler

- You can compile c code like normal, but with a slight difference:
    - you add the prefix

```bash
arm-cortex_a8-linux-gnueabi-gcc -o hello helloworld.c

arm-cortex_a8-linux-gnueabihf-gcc --target-help # will list out the architecture specific options available.
```

## sysroot, library, and header files

- sysroot is a directory that contains subdirectories for libraries, header files and configuration files.
- You can get the sysroot directory by adding `-print-sysroot` in addition to gcc

```c
arm-cortex_a8-linux-anueabihf-gcc -print-sysroot
```

- The sysroot contains the following:
    - lib: Contains .so files
    - usr/lib: contains static library files
    - usr/include: contains header for libraries
    - usr/bin contains utility programs that run on the target
    - usr/share, used for localization
    - sbin: provides ldconfig utilities. 


## Components of the C library

- libc: main C library containing POSIX functions
- libm: Contains the math functions.
- libpthread: Contains all the posix thread functions
- librt: realtime extensions to posix, including shared memory and asynchronous I/O

- By default, libc is always linked in. If you want to use anything else, you need to explicitly link it

```bash
arm-cortex_a8-linux-gnueabihf-gcc -o hello hello.c -lm
```

- You can use readelf to figure out what libraries are linked in. 

```bash
arm-cortex_a8-linux-gnueabihf-readelf -a program_name
```

## Linking libraries: Static and Dynamic Linking

- static libraries are linked during compile time.
- dynamic libraries are linked during run time.

## Static Libraries

- Static libraries are linked during compile time.
- Static linking is useful if you need to run a program before the filesystem that holds the runtime libraries are available.
- The consequence is that the binaries will be bigger in size.

- Static libraries have the `.a` file format.

## Creating Static library

- You can create a static library with the ar utility:

```bash
gcc -c test.c
ar rcs libtest.a test1.o test2.o
```
- r means insert/replace file
- c means to create the archive
- s means to add an index (which helps the linker find the symbol.)


- You can then link it with 

```bash
$ arm-cortex_a8-linux-gnueabihf-gcc helloworld.c -ltest -L../libs -I../libs -o helloworld
```
- `-ltest` tells the linker that there's a library named `libtest.a`
- `-L ../libs` tells libtest.a is located in the `../libs` directory.
- `-I ../libs` tells the compiler where te find the header files

## Shared libraries

- Share objects can be linked at runtime.
    - This is more efficient for storage and memory, since we only need to load one copy of the code.
    - This also means that we'll only have to load one copy of the code when running.

```bash
gcc -fPIC -c test1.c
gcc -shared -o test1.so test1.o
```

- `-fPIC` specifies that you want **Position-Independent Code**
    - This means the library can be loaded at any memory address.
- `-shared` indicates that the linker should make a shared object file.

- to compile with the shared library:

```c
$ arm-cortex_a8-linux-gnueabihf-gcc helloworld.c -ltest -L../libs -I../libs -o helloworld
```
- It's the same idea, except the linking will happen by the dynamic linker.

## Makefiles

- Some packages are trivial to compile, including the linux kernel, U-Boot bootloader, and BusyBox
- when you compile with make, you can specify you want to cross compile it using the CROSS_COMPILE variable

```bash
make CROSS_COMPILE=arm-cortex_a8-linux-gnueabihf-
```

## Autotools

- Autotool is a group of tools that are used as the build system.
- Examples include:
    - GNU Autoconf
    - GNU Automake
    - GNU Libtool
    - Gnulib

- The autotool smoothes the difference between the different types of systems the package may've been compiled for. This accounts for different compiler versions, libraries, different location of header files and dependencies with other packages.
- Packages that use autotool will come with a script named configure that checks for dependencies and generate a makefile based on what it finds.

```bash
./configure
make
sudo make install
```

- You can set shell variables to influence the behavior of the script:
    - CC: C compiler command
    - CFLAG: Additional C compiler flags
    - LDFLAGS: Additional linker flags, you can add `-L<lib dir>`
    - LIBS: additional libraries to link, `-lm` for example
    - `CPPFLAGS` Preprocessor flags
    - `CPP` c preprocessors to use.

```bash
CC=arm-cortex_a8-linux-gnueabihf-gcc ./configure
```

- Sometimes it'll fail. Fortunately, the autotool will give insight on how to solve it.

- When you call configure, you need to specify three devices:
    - Build computer is the one buliding the package.
    - Host: The computer the program runs on.
    - Target is the one that the compiler will generate code for.

```bash
CC=arm-cortex_a8-linux-gnueabihf-gcc ./configure --host=arm-cortex_a8-linux-gnueabihf
```

## Package Configuration

- There's a utility known as `pkg-config`, which helps track what packages are installed, and which compile flags are needed.

```
pkg-config <package> --libs --cflags
```

- We can get all the needed cflags for compiling a program with a particular package.
- If you're using a cross compiled library, you need to export the pkgconfig path as the `PKG_CONFIG_LIBDIR`.