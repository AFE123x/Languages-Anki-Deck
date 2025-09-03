# 11. Advanced Package Aspects

## Buildroot - security vulnerability tracking

- security is important in embedded systems.
- The embedded linux systems usually integrates 10-100+ open source components.
    - hence, keeping track of security vulnerabilities is difficult.
- the industry relies on **Common Vulnerability Exposure** to document security issues

- buildroot identifies if packages are affected by known CVEs using the National Vulnerabiity Database.

```bash
make pkg-stats
```
- this will generate a `pkg-stats.html` and `pkg-stats.json` file.
- this only applies to open source packages, not your custom code.

## `<pkg>_IGNORE_CVES` variable

- you can add these for packages, where the CVE report isn't relevant.

```Makefile
# example, package/bind/bind.mk
BIND_IGNORE_CVES = CVE-2017-3139
```

## Common Platform Enumeration

- This gives a uinque identifier to a software release

- by default, buid uses: `cpe:2.3:a:<pkg>_project:<pkg>:<pkg>_VERSION:*:*:*:*:*:*:*`

- you can modify these with:
    - `<pkg>_CPU_ID_PREFIX`
    - `<pkg>_CPE_ID_VENDOR`
    - `<pkg>_CPE_ID_PRODUCT`
    - `<pkg>_CPU_ID_VERSION`
    - `<pkg>_CPU_ID_UPDATE`

## patching packages: why?

- Patches apply changes to your source code.
- These are useful for:
    - fixing cross-compilation issues
    - backport bug or security fixes from upstream.
- patches are automatically applied by buildroot after extracting the package, but before configuring it.


## patching packages: ordering

- Patches are applied in the following order:
    - patches mentioned in the `<pkg>_PATCH` variable of the `.mk` file.
    - patches present in the package directory, `package/<pkg>/*.patch`
    - patches present in the **global pack directory**
- in each case, they're applied in:
    - order specified in a series file
    - otherwise, alphabetic ordering

## patching packages: patch conventions

- the name of patches should start with a sequence number, indicating the order in which they should be applied

```patch
0001-auto-type-sizeof-rework-autotest-to-be-cross-compila.patch
0002-aoeuaoeu.patch
```

- Each patch should contain a description of what the patch does 
- each patch should be signed off by author
- patches can easily be generated using `git format-patch` or `quilt`

## generating patches with git

- to apply patches, you can do `git am /path/to/patches`
- to generate patch, you do `git format-patch commit-number`

## generating patch with quilt

- in the package source code, you make a directory `patches`
- here, you import the existing buildroot patches using `quilt import /path/to/bulidroot/patches/*.patch`
- apply the patches using `quilt push -a`
- create new patch using `quilt new 0001-patchname.patch`
- edit a file with `quilt edit <filename>`
- refresh the patch using `quilt refresh`


package-specific users

- packages can define their own custom user/group using the `<pkg>_USERS` variable.

```Makefile
define <pkg>_USERS
    username uid group pid password home shell group comment
endef

# example
define AVAHI_USERS
    avahi -1 avahi -1 * - - -
endef
```

## file permissions and ownership

- by default, buildroot changes the ownership of all files to 0:0, or owned by root user and group
- you can define a default set of permissions for certain files/directories using `system/device_table.txt`
- the `<pkg>_PERMISSIONS` variable allow packages to define special ownership and permissions for files and directories.

```Makefile
define <pkg>_PERMISSIONS
name type mode uid gid major minor start inc count
endef

define SUDO_PERMISSIONS
    /usr/bin/sudo f 4755 0 0 - - - - -
endef
```
- here, major, minor, start, inc and count aren't used

## init system - adding scripts

- buildroot offers support for many init systems: BusyBox, systemd and openRC.
- when a package wants to install a program to be started at boot time, they need a startup script (sysvinit/busybox), a systemd service file, etc.

- there are variables to define these shell commands:
    - `<pkg>_INSTALL_INIT_SYSV`
    - `<pkg>_INSTALL_INIT_SYSTEMD`
    - `<pkg>_INSTALL_INIT_OPENRC`


```Makefile
# bind.mk

define BIND_INSTALL_INIT_SYSV
    $(INSTALL) -m 0755 -D package/bind/S81named \
    $(TARGET_DIR)/etc/init.d/S81named
endef

define BIND_INSTALL_INIT_SYSTEMD
    $(INSTALL) -D -m 644 package/bind/named.service \
    $(TARGET_DIR)/usr/lib/systemd/system/named.service
endef
```

## Config scripts

- libraries that don't use `pkg-config` will often have a small script that allows applications to query the compiler and linker flags to use the library
    - for example, `curl-config`
- you can list these scripts in the `<pkg>_CONFIG_SCRIPTS` variable
    - buildroot will adapt the prefix, header and library path to make them suitable for cross-compilation.

```Makefile
# libpng.mk
LIBPNG_CONFIG_SCRIPTS \
    libpng$(LIBPNG_SERIES)-config libpng-config
```

## Hooks

- buildroot's package infrastructure often implements a default behavior for certain steps:
    - `generic-package` implements the download, extract and packaging steps.
    - other infrastructures like `autotools-package` implements the configure, build and installation steps
- you may want to do additional actions before/after one of these steps.
    - **hooks** let you allow packages to add custom actions

## package hooks - principle

- there are **pre** and **post** hooks avaiable for each step of the package compilation states
    - steps include: download, extract, rsync, patch, configure, build, install, install staging, install target, install image, legal info
- hooks follow the format of `<pkg>_(PRE|POST)_<step>_HOOKS`
    - example: `CMAKE_POST_INSTALL_TARGET_HOOK`

- a hook variable will contain a list of make macros, so you'd use `+=` to register additional hooks to a hook point

```Makefile
define BIND_TARGET_REMOVE_TOOLS
    rm -rf $(addprefix $(TARGET_DIR)/usr/bin/, $(BIND_TARGET_TOOLS_BIN))
endef
BIND_POST_INSTALL_TARGET_HOOKS += BIND_TARGET_REMOVE_TOOLS # adds command to hook
```

## packages - overriding commands

- you can override steps (configure, buid, etc.) using `overriding commands`.
- for example, defining `<pkg>_EXTRACT_CMDS` or `<pkg>_BUILD_CMDS` in your package.mk file wil override the package infrastructure implementation.


```Makefile
JQUERY_SITE = http://code.jquery.com
JQUERY_SOURCE = jquery-$(JQUERY_VERSION).min.js
define JQUERY_EXTRACT_CMDS
    cp $(DL_DIR)/$(JQUERY_SOURCE) $(@D)
endef
```

## virtual packages

- there are situations where different packages provide different implementations of the same interface.
- For example, OpenGL
    - it has a default API, but each HW vendor will provide it's own OpenGL implementation.
- we need a mechanism to know what mechanism to use: this is where **virtual packages come in**.
- ibgles will match API with the correct package based on the system

## virtual package, defining

- You would select `BR2_PACKAGE_HAS_LIBEGL` and `BR2_PACKAGE_HAS_IBGLES`

```cs
config BR2_PACKAGE_SUNXI_MALI_MAINLINE
    bool "sunxi-mali-mainline"
    select BR2_PACKAGE_HAS_LIBEGL
    select BR2_PACKAGE_HAS_LIBGLES
config BR2_PACKAGE_PROVIDES_LIBGLES
    default "sunxi-mali-mainline"
```

```Makefile
SUNXI_MALI_MAINLINE_PROVIDES = libegl libgles # only detects if two providers for the same virtual package are enabled.
```