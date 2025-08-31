# 4. Build tree

## what is the `$(O)` variable?

- This is the output directory.
- You customize it using the `O` variable.

## buildroot, `host folder`

- this contains tools built for the host (cross compiler) and the sysroot of the toolchain.

## buildroot, `target folder`

- this folder has the target root filesystem.
    - It's used to generate the final root filesystem image in the `images` folder.