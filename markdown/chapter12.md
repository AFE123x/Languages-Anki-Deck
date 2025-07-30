# 12. DMA - Direct Memory Access

## What is Direct Memory Access?

- DMA (Direct Memory Access) is a feature where a device can directly access a system's RAM without CPU intervention.
- The DMA Controller is responsible for managing DMA transfers.
    - the CPU tells the DMA controller what to do.


## Setting up DMA mappings - What is it? two types of mappings

- DMA transfers require a suitable memory mapping.

- A DMA mapping consists of:
    - allocating a DMA buffer
    - generating a bus address for it.

- the bus address is represented with the `dma_addr_t` type.

- there are two types of mappings:
    - coherant DMA mapping: this automatically addresses cache coherency issues
    - streaming DMA mapping: this doesn't address it automatically.

```c
#include<linux/dma-mapping.h>
```

## DMA - Coherant mapping, allocation and freeing

```c
void* dma_alloc_coherent(struct device* dev, size_t size, dma_addr_t* dma_handle, gfp_t flag);
```

- this function handles the allocating and mapping of buffer, and gives a kernel virtual address for the buffer. 

```c
void dma_free_coherent(struct device* devi, size_t size, void* cpu_addr, dma_addr_t dma_handle);
```
- this will free the mapping.

- this is considered expensive, as you can only allocate by number of pages that are a power of two.


## DMA - Streaming DMA mapping, constraints, variants

- unlike coherant mapping, there are more constraints:
    - mapping needs to work with an already allocated buffer.
    - mapping needs to accept non-contiguous buffers.
    - buffer needs to be unmapped for the cpu to use.

- you cannot write to the buffer when it's mapped, as it'll invalidate the caches again.

- There are two variants of streaming mapping:
    - single buffer mapping: allows only one page mapping
    - scatter/gather mapping: allows passing of several buffers.


## DMA Engine - general steps

- this is a generic kernel framework for developing DMA controller drivers.
- It uses a concept of chanels, which can then be used by the DMA slaves.

- The steps are as follows:
    - allocate DMA slave channel
    - set slave and controller specific parameters
    - submit transaction
    - issue pending requests and wait for callback notifications.


