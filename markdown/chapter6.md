# 6. The Concept of a Device Tree

## what is a device tree?

- A device tree is a hardware description file.
- It's represented by a tree like structure, where each node is a device.


## device tree - datatypes

- There are a few data types:

| type | example |
| --- | --- | 
| string | "a string"; | 
| list of strings | "string 0", "string 1"; | 
| integer | <197>; |
| list of ints | <0xbeef 123 0xabcd4>;|
| byte array | [0x01 0x23 0x45 0x67]; |
| boolean | boolean_property; |


- for boolean, the presence implies true.


## device tree - node naming conventions

- nodes must have the name in the form `<name>@<address>`
    - the `@<address>` is optional, and should be the primary address used to access address.
    - If it can't be addressed, you can use it to uniquely identify a unique instance

```cs
expander@20 {
    compatible = "microchip, mcp23017";
    reg = <20>;
};

i2c@221a0000 {
    compatible = "fsl,imx6q-i2c", "fsl,imx21-i2c";
    reg = <0x021a0000 0x4000>;
};
```

## aliases, labels and phandles

- A label is used ta tag a node, iniquely identifying.
- in this, gpio1 is a label.
```cs
gpio1 : gpio@0209c0000 {
    compatible = "fsl,imx6q-gpio", "fsl,imx35-gpio";
}
```

- phandle is a pointer handle, which is a pointer to another node.

```cs
node_label: nodename@reg {
    // [...];
    gpios = <&gpio1 7 GPIO_ACTIVE_HIGH>; //pointing to gpio1 label.
};
```

- an alias is similar to a lookup table

```cs
aliases {
    ethernet0 = &fec;
    gpio0 = &gpio1;
    gpio1 = &gpio2;
    mmc0 = &usdhc1;
    // [...]
};
```

## compiling device tree

- A Device Tree has three files:
    - `.dts` is the source file
    - `.dtsi` which represents SoC level definitions (like .h files)
    - `.dtb` which is the compiled binary.


- to compile a device tre, you use `dtc`, which is a device tree compiler.

## representing and addressing devices

- When definiing devices, there are common properties. especially with devices sitting on the bus known to the kernel (SPI, I2C, etc.)

- The properties include reg, address cells and size cells

- each addressable device gets a `reg` property in the form `reg = <address0 size0 [address1size1] [address2size2] ...>`
- the address size and cell size are defined with the `#size-cells` and `#address-cells`, which defines how big the address and size is in 32 bit cells.


## Addresssing I2C and SPI devices

- I2c and SPI devices aren't memory mapped devices.
    - instead, the parent device driver, the bus controller, would perform access on behalf of the CPU.

- devices would be represent as subnode of the I2C/SPI bus the node sits on.

````cs
&i2c3 {
    status = "okay";

    temperature-sensor@49 {
        compatible = "national,lm73"; //compatible devices
        reg = <0x49>; // I2C address
    };

    pcf8523: rtc@68 {
        compatible = "nxp,pcf8523";
        reg = <0x68>; // I2C address
    };
};
````
- by default, the size-cell is 0, so you just put the address

## addressing platform devices

- these are memory mapped devices.
- you can use the reg to define the memory regions.
- this device should have, in compatible, `"simple-bus"`, which is a simple memory mapped bus with no special handling.


## describing devices

- the job of a driver is to handle and manage a device.
- it needs to know about the devices configuration parameters, which we can define in the device tree.
- It also includes resources (memory regions, interrupt lines, DMA channels, clocks, etc.)

```cs
uart1: serial@02020000 {
    compatible = "fsl,imx6q-uart", "fsl,imx21-uart";
    reg = <0x02020000 0x4000>;
    interrupts = <0 26 IRQ_TYPE_LEVEL_HIGH>;
    clocks = <&clks IMX6QDL_CLK_UART_IPG>,
    <&clks IMX6QDL_CLK_UART_SERIAL>;
    clock-names = "ipg", "per";
    dmas = <&sdma 25 4 0>, <&sdma 26 4 0>;
    dma-names = "rx", "tx";
    status = "disabled";
};
```

## named resources

- the person that writes the driver and the person that writes the device tree are gonna be different.
- for instance, a driver may expect two irq lines at index 0 and 1. What if the order isn't respected?
- To prevent this, the tree has the concept of named resources, so the driver can coordinate with it.

- Named resources means that we need to define a resource list, and naming them, so the driver can access the resource by name.

- there are 4 properties to name the resources:
    - `reg-names`: name the memory regions in the reg property
    - `clock-names` - to name each clock in the clock property
    - `interrupt-names` to name each interrupt int he interrupt property.
    - `dma-names` is for dma properties.

```cs
fake_device {
    compatible = "packt,fake-device";
    reg = <0x02020000 0x4000>, <0x4a064800 0x200>, <0x4a064c00 0x200>;
    reg-names = "config", "ohci", "ehci";
    interrupts = <0 66 IRQ_TYPE_LEVEL_HIGH>, <0 67 IRQ_TYPE_LEVEL_HIGH>;
    interrupt-names = "ohci", "ehci";
    clocks = <&clks IMX6QDL_CLK_UART_IPG>, <&clks IMX6QDL_CLK_UART_SERIAL>;
    clock-names = "ipg", "per";
    dmas = <&sdma 25 4 0>, <&sdma 26 4 0>;
    dma-names = "rx", "tx";
};
```

```c
struct resource *res1, *res2;
res1 = platform_get_resource_byname(pdev, IORESOURCE_MEM, "ohci");
res2 = platform_get_resource_byname(pdev, IORESOURCE_MEM, "config");
struct dma_chan *dma_chan_rx, *dma_chan_tx;
dma_chan_rx = dma_request_slave_channel(&pdev->dev, "rx");
dma_chan_tx = dma_request_slave_channel(&pdev->dev, "tx");
inttxirq, rxirq;
txirq = platform_get_irq_byname(pdev, "ohci");
rxirq = platform_get_irq_byname(pdev, "ehci");
structclk *clck_per, *clk_ipg;
clk_ipg = devm_clk_get(&pdev->dev, "ipg");
clk_ipg = devm_clk_get(&pdev->dev, "pre");
```

- you can see how things are accessed.


## device tree - interrupt handlers, irq controllers and consumer, DT properties, gettting handler on driver

- we divide the interrupt handler into two parts: consumer side and controller side.

- IRQ controllers is the device that exposes IRQ lines to the consumer.
- On the controller side, there are two properties:
    - `interrupt-controller`: boolean expression to mark device as interrupt controller
    - `#interrupt-cell` - how many cells are used to specify an interrupt for interrupt controller.

- the consumer is the device that generates the IRQ.
- for the consumer, there are two properties:
    - `interrupt-parent`: a phandle pointer to the interrupt controller node.
    - `interrupts` the interrupt specifier.


- to get the irq number from the driver, you use the `platform_get_irq()`

```c
int irq = platform_get_irq(pdev, 0);
ret = request_irq(irq,imx_rxint, 0, dev_name(&pdev->dev), sport);
```

- in the device tree, the interrupt variable look like the following:

```cs
// [...]
interrupts = <0 66 IRQ_TYPE_LEVEL_HIGH>;
```
- first number defines the interrupt type:
    - 0: shared peripheral interrupts - for interrupt signals shared among cores.
    - 1: private peripheral interrupts - interrupt signals private to an individual core
- second number is the irq number
- third sense defines the sense level, defined in `include/linux/irq.h`

## extracting values from device tree

- in your driver, you can extract the variables in the DTS from the driver using the of_property_read_*

- `of_property_read_string()` to get a string
- `of_property_read_u32()` to get a u32 int
- `of_property_read_u32_array()` to get a u32 int array

## iterating through subnodes in DT

- you can use the `for_each_child_of_node()` macro to iterate through the nodes

```c
struct device_node *np = pdev->dev.of_node;
struct device_node *sub_np;
for_each_child_of_node(np, sub_np) {
    /* sub_np will point successively to each sub-node */
    // [...]
    int size;
    of_property_read_u32(client->dev.of_node, "size", &size);
    // ...
}
```