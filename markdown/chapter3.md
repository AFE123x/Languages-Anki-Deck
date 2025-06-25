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
    - machine number: defines SoC for devices that don't support device trees
    - details about hardware (ram size, clock speed, etc.)
    - kernel command line (plain ascii string defining kernel behavior.)
    - Optionally, location of Device Tree and Initramfs.

- The rest is figured out by the kernel during runtime.≥

## Device Trees

- A device tree provides a flexible way to define the hardware components of a computer system.
- The tree is loaded by the bootloader and passed to the kernel.

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

## Deciphering Device Tree

```cs
/dts-v1/;
/{ //root of our device tree
    model = "TI AM335x BeagleBone";
    compatible = "ti,am33xx";
    #address-cells = <1>;
    #size-cells = <1>;
    cpus { // cpu node of system
        #address-cells = <1>;
        #size-cells = <0>;
        cpu@0 {
            compatible = "arm,cortex-a8";
            device_type = "cpu";
            reg = <0>;
        };
    };
    memory@0x80000000 { // memory node of system
        device_type = "memory";
        reg = <0x80000000 0x20000000>; /* 512 MB */
    };
};
```

- The device tree starts with a root node, defined as `\`
- the root has two nodes, `cpus` and `memory`
- `cpu@0` is the name of the cpu, and an address that distinguishes the node from others of the same type.
- within the cpu, the `compatible` property is for matching devices with device drivers.

## Device tree - compatible property

```cs
cpus { // cpu node of system
        #address-cells = <1>;
        #size-cells = <0>;
        cpu@0 {
            compatible = "arm,cortex-a8";
            device_type = "cpu";
            reg = <0>;
        };
    };
```

- This property is used by the kernel to find a matching device driver.
- The kernel will compare the string defined by compatible with the string exported by the device driver using `of_device_id`


## Device Trees - Reg Proporties

```cs
reg = <0x80000000 0x20000000>; /* 512 MB */
```

* The `reg` property defines a memory-mapped range: base address (`0x80000000`) and size (`0x20000000` bytes).
* By default, each value is 32 bits. To use 64-bit addresses or sizes, increase the number of cells:

```cs
/ {
    #address-cells = <2>;
    #size-cells = <2>;
    memory@80000000 {
        device_type = "memory";
        reg = <0x00000000 0x80000000 0x00000000 0x80000000>; // 2GB at 0x80000000
    };
};
```

* `#address-cells` and `#size-cells` specify how many 32-bit cells represent addresses and sizes.

```cs
cpus {
    #address-cells = <1>;
    #size-cells = <0>;
    cpu@0 {
        compatible = "arm,cortex-a8";
        device_type = "cpu";
        reg = <0>; // Logical CPU ID
    };
};
```

* CPUs don't have memory addresses, but `reg` uniquely identifies them (e.g., by core ID for SMP systems).


## Device Trees - Labels and Interrupts, phandles

* Device trees describe more than just memory hierarchy — components may also connect to:

  * Interrupt controllers
  * Clocks
  * Voltage regulators


### Labels (phandles)

* A **label** (like `intc:` or `lcdc:`) can be used to reference a node from other nodes.
* When compiled, labels become **phandles** (unique numerical identifiers).
* Labels are useful for linking nodes, e.g., to define interrupt relationships.


### Interrupt Controller Example

```dts
intc: interrupt-controller@48200000 {
    compatible = "ti,am33xx-intc";
    interrupt-controller;
    #interrupt-cells = <1>;
    reg = <0x48200000 0x1000>;
};
```

* `intc:` is the **label** for this node.
* `interrupt-controller` marks this node as an interrupt controller.
* `#interrupt-cells = <1>`: One cell is used to describe each interrupt (just IRQ number in this case).
* `reg`: Physical address and size of the controller.

### Device Connected to Interrupt Controller

```dts
lcdc: lcdc@4830e000 {
    compatible = "ti,am33xx-tilcdc";
    reg = <0x4830e000 0x1000>;
    interrupt-parent = <&intc>;
    interrupts = <36>;
    ti,hwmods = "lcdc";
    status = "disabled";
};
```

* `lcdc:` is the label for the LCD controller.
* `interrupt-parent = <&intc>;` connects this device to the `intc` interrupt controller.
* `interrupts = <36>;` means this device generates IRQ number 36.
* The `compatible`, `reg`, and `status` properties describe the device normally.


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