# 3. All About Bootloaders

## What does a bootloader do?

- The bootloader has two main jobs:
    - initialize the system to a basic level
    - load the kernel
    - misc. jobs, like settings

**initializing systems**
- When you initially turn on the system, a lot of features will not be on, the system will be in a minimal state.
    - Because of this, the system bootstrap will have several phases of code, bringing more of the systems into operation.

**loading kernel**

- When the systems are initialized, the bootloader will load the kernel into RAM and create the execution environment for it.

**settings

- The boot loader may also have to provide a maintenance mode, loading a new boot image into memory, and run diagostics.

## Boot Sequence - Old way

- Back then, the bootloader only had to be loaded in non-volatile memory at the reset vector of the processor.
    - **NOR Memory** was the common solution as it can be mapped directly to the address space.
- On systems, there'll be a reset vector at an address, towards the end of the address space.
    - When the system boots, it does a jump instruction to the starting location of part of the bootloader code.

- Once the bootloader code runs, it can then run the DRAM controller, so the main memory can become active.
    - As a result, the bootloader can copy itself to the DRAM, then load the kernel.

- This becomes more **complicated** when you move away from a simple linearly addressable storage medium like NOR flash.

## Bootloader Phases

- Phase 1: ROM Code
- Phase 2: Secondary program loader
- Phase 3: TPL 

## Bootloader Phases: Phase 1

- Phase 1 is the ROM code phase.

- Since the DRAM isn't initialized yet, the System roms ROM code, stored on the SoC.
- At this point, the DRAM isn't initialized, so only the Static RAM is available, which doesn't require a memory controller.

- Most SoC's only have a small amount of SRAM on the chip, varying from as little as 4 kb to hundred kb.

- The ROM code can load small chunks of code from one of the preprogrammed locations into SRAM.
    - This chunk of code is called the Secondary program loader

## Bootloader Phases: Phase 2

- Phase two is the Secondary Program Loader
- This part is responsible for setting up the memory controller and other parts of the system
- This is done in preparation for loading the Tertiary Program Loader(TPL) into DRAM.
- The the TPL is loaded, the cpu can jump to that area

## Bootloader Phases: Phase 3

- The final phase is the Ternary Program Loader (PTL).
- This is the full bootloader.
    - Here, you can use a command line interface, which will let you do maintenance on the kernel.
- In this phase, it loads the kernel image, and an optional initramfs and FDT.

## UEFI Firmware

- Most embedded x86/x86-64 and some ARM designs, have firmware based on the **Universal Extensible Firmware Interface** (UEFI) standard.
- The boot sequence is similar to the 3 phases of a boot loader.
    - **phase 1**, processor loads the initialization firmware from flash memory.
    - **phase 2**: Initializes DRAM controller and other interfaces, allowing it to loading up an EFI boot manager from the EFI partition on a disk.
    - **phase 3**: The boot manager can then load the kernel and optional features in memory.

## Moving from Bootloader to kernel

- When the bootloader gives control to the kernel, it transfer some information:
    - machine number: to inform the kernel to identify the type of SoC
    - details about the hardware, like size of the ram, and clock speed.
    - kernel command line: a string that controls the behavior of linux
    - optional features like the location and size of the device tree and initramfs.

- You can't pass a lot of information from the bootloader, and the kernel will either have to figure it out during runtime, or the details need to be hardcoded into the kernel as **platform data**

## Device Trees

- A device tree provides a flexible way to define the hardware components of a computer system.
- The tree is loaded by the bootloader and passed to the kernel.
    - The programmer can opt to bundle the device tree with the kernel image itself.

```cs
/dts-v1/;
/{
    model = "TI AM335x BeagleBone";
    compatible = "ti,am33xx";
    #address-cells = <1>;
    #size-cells = <1>;
    cpus {
        #address-cells = <1>;
        #size-cells = <0>;
        cpu@0 {
            compatible = "arm,cortex-a8";
            device_type = "cpu";
            reg = <0>;
        };
    };
    memory@0x80000000 {
        device_type = "memory";
        reg = <0x80000000 0x20000000>; /* 512 MB */
    };
};
```
- The device tree represents a computer system with components joined together into a tree like heirarchy.
- the `/` represents the root node, and the subsequent nodes representing the hardware of the system.

- In this example, we have a device containing cpus and memory nodes.
    - Within the cpus, we have a single cpu node, `cpu@0`.

- By convention, you have `@0`, or whatever number to differentiate identical components.

- The cpu and root nodes have a `compatible` property, which the kernel uses to find a matching device driver

## Device Trees - Reg Proporties

- The memory and cpu nodes also have a `reg` property.
- The reg refers to a range of units in a register space.
    - The reg property consists of two values which represents the start address and the length of the range.

```cs
reg = <0x80000000 0x20000000>; /* 512 MB */
```

- In this example, we have a single bank of memory starting at address 0x80000000, and is 0x20000000 bytes long.

- If you have a 64 bit address, you need two sells, for each address range.

```cs
/ {
    #address-cells = <2>;
    #size-cells = <2>;
    memory@80000000 {
        device_type = "memory";
        reg = <0x00000000 0x80000000 0 0x80000000>;
    };
};
```
- Here, we define the number of cells in the address cell and size cell.
- `0x00000000 0x8000000`tells us our address begins at`0x000000008000000`.
- `0 0x80000000` tells us our memory size is `0x0000000080000000` bytes long.

```cs
// ...
    #address-cells = <1>;
    #size-cells = <1>;
    cpus {
        #address-cells = <1>;
        #size-cells = <0>;
        cpu@0 {
            compatible = "arm,cortex-a8";
            device_type = "cpu";
            reg = <0>;
        };
    };
// ...
```
- In our example, we can see the reg as well.
    - cpu's don't have memory addresses, but we can uniquely identify them. For instance, we may want to address multicore processors with 0, 1, etc.


## Device Trees - Labels and Interrupts, phandles

- We can also connect devices to interrupt handlers, clock sources, and voltage regulators.
    - These type of connections are known as **phandles**

```cs
/dts-v1/;
{
    intc: interrupt-controller@48200000 {
        compatible = "ti,am33xx-intc";
        interrupt-controller;
        #interrupt-cells = <1>;
        reg = <0x48200000 0x1000>;
    };
    lcdc: lcdc@4830e000 {
        compatible = "ti,am33xx-tilcdc";
        reg = <0x4830e000 0x1000>;
        interrupt-parent = <&intc>;
        interrupts = <36>;
        ti,hwmods = "lcdc";
        status = "disabled";
    };
};
```
- Here, we have a interrupt controller.
    - The interrupt-cells identifies what type of interrupt does it handle, an "interrupt property"
- the reg defines where the handler is, and it's size.


## Device Tree - Include Files

- It's common to group similar devices into diferent files, with the extension .dtsi

- You can include `dtsi` files in the `dts` file using `/include/`
- You can also include header files, which contains things like C macros.
```cs
/include/ "vexpress-v2m.dtsi"
#include <dt-bindings/pinctrl/am33xx.h>
```

## Compiling a device tree

- The bootloader and kernel require a binary representation of the device tree.
- The compiler to use is `dtc`, or device tree compiler.
- The output file of dtc is a dtb, which is a **device tree binary**.

```bash
dtc simpledts-1.dts -o simpledts-1.dtb
```
- The compiler doesn't give helpful error messages, and only checks for syntax.