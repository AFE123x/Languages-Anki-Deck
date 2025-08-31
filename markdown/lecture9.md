# 9. Integrating new packages in buildroot

## what is a package in buildroot

- A package is a set of meta-information needed to automate the build process of certain components of a system.
- Can be used for userspace applications and firmware, kernel drivers, bootloaders, etc.
- not the same as the default linux package manager.

## Components of buildroot package
- the application, `foo` would be in the directory `package/foo`
- `Config.in` describes the configuration option for package
- `<pkg>.mk` is written in make, and describes where to fetch the source, how to build and install.
- `<pkg>.hash` providese a hash to check the integrity of the downloaded tarbal.
- `.patch` file, which is applied on the package source code before building.
- other things, like init scripts, example config files, etc.

## `Config.in` basics - individual package

```cs
config BR2_PACKAGE_STRACE 
    bool "strace"
    help
        A useful diagnostic, intsructional, and debugging tool.
        allows you to track what system calls a program makes
        while it is running

        http://sourceforge.net/project/strace
```
- by convention, we name the config `BR2_PACKAGE_<PACKAGE>`
- the package option is a boolean, so you choose yes or no.
- help provides a description of the item.

## `package/<pkg>/Config.in`

- at the top of the package heirarchy is the `package/Config.in`, which links the Config.in files from each package

```cs
menu "Target packages"
menu "Audio and video applications"
    source "package/alsa-utils/Config.in" //location of Config.in
    ...
endmenu
...
menu "Libraries"
menu "Audio/Sound"
    source "package/alsa-lib/Config.in"
    ...
endmenu
...
```

## `Config.in` dependencies

- KConfig allows to express dependencies using the `select` or `depends on` keywords.
    - `select`: if option `A select B` exists, if A is enabled, B is enabled.
    - `depends on` means if A depends on B, A will ony be visible if B is enabled.

```cs
config BR2_PACKAGE_BTRFS_PROGS
    bool "btrfs-progs"
    depends on BR2_USE_MMU # util-linux
    depends on BR2_TOOLCHAIN_HAS_THREADS
    select BR2_PACKAGE_LZO
    select BR2_PACKAGE_UTIL_LINUX
    select BR2_PACKAGE_UTIL_LINUX_LIBBLKID
    select BR2_PACKAGE_UTIL_LINUX_LIBUUID
    select BR2_PACKAGE_ZLIB
    help
    Btrfs filesystem utilities
    https://btrfs.wiki.kernel.org/index.php/Main_Page
comment "btrfs-progs needs a toolchain w/ threads"
    depends on BR2_USE_MMU
    depends on !BR2_TOOLCHAIN_HAS_THREADS
```

## `Config.in`: Dependency Propogation

- KConfig doesn't propogate `depends on` dependencies accross `select` dependencies.
- For example, if `A depends on FOO`, and B has a `select A`, B needs to `depend on FOO`.

## `Config.in.host` host packages

- Most of buildroot packages are cross compiled for the target system.
- There are also packages with a host variant.
- host packages are usually not visible in the menuconfig, but are just dependencies to other packages.
- you'd make a `Config.in.host` file, and you'd name the package `BR2_PACKAGE_HOST_<package>`.

## `Config.in` - sub options

- you can create suboptions using `if ... endif`

```rs
config BR2_PACKAGE_PPPD
    bool "pppd"
    depends on !BR2_STATIC_LIBS
    depends on BR2_USE_MMU
    ...
if BR2_PACKAGE_PPPD
    config BR2_PACKAGE_PPPD_FILTER
        bool "filtering"
        select BR2_PACKAGE_LIBPCAP
        help
            Packet filtering abilities for pppd. If enabled,
            the pppd active-filter and pass-filter options
            are available.
    config BR2_PACKAGE_PPPD_RADIUS
        bool "radius"
        help
            Install RADIUS support for pppd
endif
```

## buildroot - package infrastructures

- each software component to be built by buildroot will come with it's own build system.
- there are many buildtools, like Meson, autotools, CMake, etc.
- buildroot offers **package infrastructures** that accomodate these build systems.
- there's a generic package infrastrucure for non-standard build components.

## Buildroot - generic-package infrastructure

- This is to be used with software components having non-standard build systems.

- The generic package infrastructure handles downloading, extracting and packaging.
- You need to implement the configuration, build and install steps

![alt text](image-3.png)

## The `<pkg.mk>` file

- this isn't your conventional make file
- rather, it's a bunch of defined variables, ending with a ca to the desired package infrastructure macro.

## buildroot generic-package infrastructure `.mk` - naming conventions

- The buildroot package infrastructure makes many assumptions on variable and file names.
    - package must be in `package/<pkg>/`, where pkg is the package in all lowercase.
    - the Config.in option enabling package must be named `BR2_PACKAGE_<PKG>`, where PKG is the name of the package in upper case.
    - variables in the mk file need to be prefixed with `PKG_`, where PKG is the uppercase name of the package.

## global namespace 

- namespace of variables in KConfig is global, so you can't have two variables with the same name.

## buildroot make - package downloadnig related variabes

- `<pkg>_SITE` is the download location (url to download)
- `<pkg>_VERSION` is the version of the package
- `<pkg>_SOURCE` is the name of the downloaded file
- `<pkg>_PATCH` is the list of patches to apply before building
- `<pkg>_EXTRA_DOWNLOADS` is a list of additional files to downoad with the source code.

## hash files

- there's a `.hash` file used to check the integrity of downloaded files

- it's structured as follows:

```hash
<hashtype>  <hash>  <file>
md5 7f104064b906ad8c7329ca5e409a32d7 perl-5.32.1.tar.xz
sha1 1fb4f710d139da1e1a3e1fa4eaba201fcaa8e18e perl-5.32.1.tar.xz
```

## Describing dependencies

- `<pkg>_DEPENDENCIES` is used to descripbe the dependencies of the current package.
    - This, unlike the dependency in `Config.in`, will guarantee the dependencies is built before the current package.

## mandatory vs. optional dependencies

- Software dependencies can have option or mandatory dependencies.
- to handle mandatory dependencies:
    - use `select` or `depends on` on the main package option in `Config.in`
    - use `<pkg>_DEPENDENCIES`
- to handle optional dependencies:
    - handle it automatically in the `.mk` file. **this is the better option, reduce complexity**.
    - add a package sub-option in the `Config.in` file.

## Defining where to install 

- packages can install files in the following locations:
    - `$(TARGET_DIR)`, the target root filesystem
    - `$(STAGING_DIR)`, which is the compiler's sysroot
    - `$(BINARIES_DIR)`, which where the final image is located.

- you can define whether or not you want to install packages in the directories with:
    - `<pkg>_INSTALL_TARGET`, default yes.
    - `<pkg>_INSTALL_STAGING`, default no.
        - if yes, then `<pkg>_INSTALL_STAGING_CMDS` is executed
    - `<pkg>_INSTALL_IMAGE` defaults to no, but will run `<pkg>_INSTALL_IMAGES_CMDS` if yes

## describing actions for `generic-packages`

- as mentioned, generic-package only does the downloading, extracting and patching steps.
- you need to implement everything else.

- There are multiple variables you need to setup:

| variable | meaning |
| --- | --- |
| `<pkg>_CONFIGURE_CMDS` | aways called |
| `<pkg>_BUILD_CMDS` | always called |
| `<pkg>_INSTALL_TARGET_CMDS` | called when `<pkg>_INSTALL_TARGET = YES`|
| `<pkg>_STAGING_CMDS` | called when `<pkg>_INSTALL_STAGING = yes`|
| `<pkg>_INSTALL_IMAGES_CMD` | called when `<pkg>_INSTALL_IMAGES = YES`|
| `<pkg>_INSTALL_CMDS` | always called for host packages|


## `<pkg>.mk` - describing actions

- There are multipe commands for this

| variable | meaning |
| --- | --- |
| `$(@D)` | source directory of package|
| `$(MAKE)` | call make |
| `$(MAKE1)` | when the package doesn't buid properly in parallel mode|
| `$(TARGET_MAKE_ENV)` and `$(HOST_MAKE_ENV)` | you pass into make to ensure the `PATH` is correct |
| `$(TARGET_CONFIGURE_OPTS)` and `$(HOST_CONFIGURE_OPTS)`| to pass CC, LD, CFLAGS, etc.|
| `$(TARGET_DIR)`, `$(STAGING_DIR)`, `$(BINARIES_DIR)` and `$(HOST_DIR)` | defines the destination directories |

## variable inheritence

- For some variables, you can inherit the parent variable, or overwrite it:
    - for example, if `HOST_BAZ_<var>` isn't define, it inherits from `BAZ_<var>` instead.
- Not all variables support inheritence:
    - `HOST_<PKG>_BUID_CMDS` isn't inherited from `<PKG>_BUILD_CMDS`