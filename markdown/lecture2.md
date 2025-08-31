# 2. Introduction to Buildroot

## What is buildroot?

- Buildroot can build a toolchain, rootfs, kernel and a bootloader.
- It's easy to configure, via menuconfig, xconfig, etc.
- uses make and kconfig
- supports many architectures.

## Configuring buildroot, building

- Buildroot uses Kconfig, so you have the following:
    - `make menuconfig`
    - `make nconfig`
    - `make xconfig`
    - `make gconfig`

- to build, you simply run `make`, or the script `./utils/brmake`

## out of tree builds: What are they?

- Out of tree builds allow one to use an output directory different from `output/`
- This lets you build different buildroot configurations from the same source tree.
- You specify the custom directory using `O=/path/to/directory` when you use make.

```bash
make O=../foo-output/ menuconfig
```

- if you're in the folder (like `foo-output`), you specify add `-C` to use the buildroot make system.

```bash
make -C ../buildroot/ O=$(pwd) menuconfig
```

## Full config files vs. def config

- If you us the `.config` file, you'd 4742 lines worth of options: 
    - realistically, you don't wanna read all dat.
- the default config only includes the values for options where the non-default values are chosen.
    - AKA, you'll only have to go through fewer configs

## creating/using, listing defconfig

- There are many defconfig files stored in the `configs/` directory
- to setup the defconfig, you'd do `make <foo>_defconfig`
- to create a defconfig, you do make savedefconfig
- to list defconfigs, you use `make list-defconfigs`
