# 6. Selecting a build system

## What is a build system

- One approach to creating a system is known as the RYO (Roll your own) process.
    - This gives you full control of the system, tailoring it to whatever you want.

- For most cases, the above approach is a waste of time.
- Instead of **ryo**, you can use a **build system**.

- The build system will be able to build some or all of the following:
    - Toolchain
    - Bootloader
    - Kernel
    - Root Filesystem

- You can also:
    - Add your own packages containing applications and kernel changes.
    - Select various root filesystem profiles:
        - large/small
        - graphics/no graphics
    - Track which open source licenses are used by various pacages
    - Have a user friendly user interface.

## Package formats and package managers

- Most linux distributions already contain binaries (pre-compiled programs)
    - They can be in RPM or DEB format
    - RPM is the red hat package: used by distros like fedora
    - DEB is the debian type package, used by mint 

## Buildroot

- Buildroot builds the toolchain, bootloader, kernel and root filesystem.
    - As the principle build tool, it uses **make**

## Using buildroot

- To use buildroot, you need to clone, then make

```bash
$ git clone git://git.buildroot.net/buildroot -b 2017.02.1
$ cd buildroot
$ cd buildroot
$ make qemu_arm_versatile_defconfig # depends on what you're build for
$ make
```

## Overlays

- let's say you have another program you made, and would like to add it over the system.
- There'll be a directory known as `overlay` which will have additional code you can build.
- Another option is to add the program to the Kconfig.

## Adding package to Kconfig

- You can create the `package/helloworld/` subdirectory with a configuration file, `config.in`.

```cs
config BR2_PACKAGE_HELLOWORLD
    bool "helloworld"
    help
        A friendly program that prints hello world.
```
- The first line must be `BR2_PACKAGE_<uppercase package name>`.

- Afterwards, you need to add a the respective makefile, `package/helloworld/helloworld.mk`, to suppely the data needed by buildroot

```cs
HELLOWORLD_VERSION = 1.0.0
HELLOWORLD_SITE = /home/chris/MELP/helloworld
HELLOWORLD_SITE_METHOD = local
define HELLOWORLD_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef
define HELLOWORLD_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/helloworld $(TARGET_DIR)/usr/bin/helloworld
endef

$(eval $(generic-package))
```

## Yocto Project

- The Yocto project is a more complex version of build. It can generate the entire linux distribution for you. 