# 3. buildroot source and build tree

## buildroot source tree - foundational materials

- `makefile` this handles the configuration and orchestration of the build
- `config.in` consists of many configuration options, will include othel config.in

## buildroot source tree - system related folders
- `arch/` contains other config.in files, degfining architecture specific options
- `toolchain/` contains options for tool chain specific things.
`system/` contains config options for thingsn like teh init system, `/dev` handling, etc.
- `linux/` linux related configurations.
- `package/` contains all user space packages
- `fs/` logic for generating filesystem image
- `boot/` bootloader packages
- `configs/` default configuration files for various platforms (the defconfigs)
- `board/` board specific files, like kernel pathes, image flashing scripts, etc.

## buildroot sourec tree - support related folders

- `support/` misc utilities (libtool patches, download helper, etc.)
- `utils/` various utilities for buildroot developers, like brmake, test-pkg.
- `docs/` buildroot documentation.
    - this is the official buildroot documentation.
