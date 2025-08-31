# 5. Toolchains in Buildroot

## What is a cross compilation toolchain?

- This is a set of tools to build and debug code for the target architecture.
    - For example: building ARM code from an x86-64 PC


## Cross compilation toolchain, components

- binutils
- c/c++ libraries
- kernel headers
- gcc compiler
- gdb debugger

## buildroot - two options for toolchain

- buildroot offers two choices for the toolchain:
    -**internal toolchain**: Buildroot will build the toolchain from source
    -**external toolchain**: Buildroot will use prebuild toolchain

## existing external toolchains

- You can consult `toolcahins.bootlin.com` to get prebuilt toolchains.

## kernel header versions

- youc an set the version of the kernel headers.
- Kernel/user space interfaces are backwards compatible.
    - therefore, you want to use kernel headers either equal or older than the kernel version running on the target.