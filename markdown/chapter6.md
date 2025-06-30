# 6. Selecting a Build System

## What is a build system

- A build system automates the process of building the system.
- The build system will build, from upstream source code: 
    - toolchain
    - bootloader
    - kernel
    - root filesystem

- It allows you to modify certain aspects, like packages, filesystem profiles, open source licenses.

## Customizing Buildroot - Adding your own code

- There are two approaches to adding your own software:
    - build the binary separately, then roll it into the final system as an **overlay**.
    - create a buildroot package that can be selected from the menu.

## Customizing buildroot - Overlays

- An overlay is a directory structure copied on top of the buildroot rootfs at a later stage in the build process.
- It contains executables, libraries, other things you may want.
    - These binaries should be compile with the same tool chain as buildroot.;

- You'd easily compile the binary, using the cross compiler
- afterward, put the binary into a staging area.
- You can then set BR2_ROOTFS_OVERLAY to the path of the overlay.

## Customizing Buildroot - Adding package

- Buildroot packages are in the `pacakge` directory.
- Each will have 2 files:
    - `Config.in`, which contains the snippit of KConfig code needed to make package visible in configuration menu.
    - `<packagefile.mk>`, which is a makefile.

- There's no actual code, rather, it contains instructions on downloading the tarball, or doing git pull to obtain the upstream source. 

- you can look at sample packages, get an idea of what it looks like.

## License Compliance

- Since build-root is open source, you probably want to check out the licenses of the packages you wanna use. 
- Buildroot offers a `make legal-info` command which compiles license information.

## Yocto Project

- The Yocto project is a more complex version of buildroot
- Yocto will have recipes similar to buildroot but:
    - uses python and shellscript
    - uses a task scheduler called bitbake.

## Yocto Project - Configuration

- Once you clone yocto, you nede to configure it.
- First, you'll need to setup the environment, using `source oe-init-build-env`

- there are also many config files, in `conf/`:
    - `local.conf`: specifies specifications of device.
    - `bblayers.conf`: contains path of "meta layers"
    - `templateconf.cfg`: contains name of directory with various conf files.

## Yocto Project - Building

- Once you finish configuring, you can build the system with `bitbake`
- Here, you specify what root filesystem you want to create:
    - `core-image-minimal`: minimal console based system for testing.
    - `core-image-minimal-initramfs`: similar to minimal, but built as ramdisk
    - `core-image-x11`: basic image with support for graphics via X11 server and xterminal terminal app.
    - `core-image-sato`: full graphical system.

## Yocto Project - Layers

- Yocto structures metadata into layers.
- each layer will start with `meta`.

- The core layers include:
    - `meta`: modified OpenEmbedded core.
    - `meta-poky`: metadata for poky distribution
    - `meta-yocto-bsp` contains board support packages for machine.

- `bblayers.conf` contains the list of layers bitbake searches for recipes.

- You can add new layers to extend Yocto's functionality.
- for example, `meta-qt5` is a layer containing the qt5 libraries.

- Each meta layer will have a folder.
- You can configure it in the `conf/layer.conf` folder.
- You can generate meta layers using the `yocto-layer` script.

## Yocto Project layers - adding layer

- You can add a layer using `bitbake-layers`

```bash
$ bitbake-layers add-layer <path-to-layer-directory>
```

- You can then list out the layer using `show-layers`

```bash
$ bitbake-layer show-layer
layer path priority
==========================================================
meta /home/chris/poky/meta 5
meta-yocto /home/chris/poky/meta-yocto 5
meta-yocto-bsp /home/chris/poky/meta-poky-bsp 5
meta-nova /home/chris/poky/meta-nova 6
```

## Yocto Project - Bitbake and recipes

- Bitbake will contain metadata of varying types:
    - **recipes**: contain information about how to get and build software
    - **append**: this allows some details of a recipe to be overridden or extended.
    - **include**: information that's common to several recipes, allowing them to share information.
    - **classes**: contains common build information. Recipes can inherit them.
    - **configuration**: Define various configuration variables.

## Yocto Project - Recipes

- A recipe is a collection of tasks written in python and shell script.
- Tasks include:
    - `do_fetch`
    - `do_unpack`
    - `do_patch`
    - `do_configure`
    - `do_compile`
    - `do_install`

you can get a list of tasks for a recipe using `bitbake -c <task> [recipe]`


