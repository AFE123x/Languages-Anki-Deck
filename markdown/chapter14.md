# 14. The Block I/O Layer

## What are block devices, what are blocks

- Block devices are hardware devices distinguished by **random access** of fixed-size chunks of data.
    - These chunks are known as **blocks**

- Examples of block devices include floppy drives, Blu-ray readers, etc.

## Character devices

- Character devices are accessed as a stream of **sequential data**
    - in other words, one byte after another.

- Examples of this inclued keyboards, serial ports, etc.

## Anatomy of a Block Device

- The smallest addressable unit on a block device is a sector.
    - These are going to be in powers of two, but 512 bytes is the most common size.
- A device cannot address or operate on a unit smaller than the sector, but it can address multiple sectors at a time.

## Block Devices - Software Side

- The software has different goals, and uses another addressable unit known as blocks (the smallest addressible unit).
    - This is used instead of sectors.

- The block size:
    - cannot be smaller than a sector
    - should be a multiple of a sector.
    - cannot be larger than the page size.


## Buffer and Buffer Heads

- When a block is stored in memory, after a read or pending a write, the block is stored in a buffer.
    - Each buffer is associated with one block.
- Each buffer is associated with one block.

- The kernel needs to keep track of metadata of the buffer
    - For example, from what block device, and what specific block.
- For this, there's a descriptor for each buffer.
    - This descriptor is kown as the **buffer head**.

- The buffer head describes the mapping between the on disk block and the physical in memory buffer.


## Problems of Buffer head

- The buffer head structure is large.
    - It was a pain to manipulate data.
- The buffer head only describes only a single buffer.
    - AKA, waste of space.

- These problems resulted in a new structures, `bio`.

## bio structure

- The bio structure represents I/O operations, represented as a list of segments.
    - A segment is a chunk of a buffer contiguous in memory.

- The struct bio will contain bi_io_vec, which is a list of bio_vec structures
    - Each bio_vec is a vector that holds the page, offset and length.
- the bio structure will contain a pointer, `bi_idx`, which keeps track of where the Block I/O operation current is.


## Request Queues

- The block device maintains a request queue to store their pending block I/O requests.
    - This queue is represented with a doubly linked list of requests and associated control information.
- The block driver will pop the requests off the head of the queue, and submit it to it's associated block device.
    - Each request can compose multiple bio structures, which specifies which disk blocks to read from.

## I/O Schedulers

- You can't just pop requests off a queue and expect good performance
    - I/O operations are the slowest on modern computers
- Our goal is to minimize seeks, so the kernel will schedule I/O requests
    - In legit terms, it will merge and sort I/O requests.
    
- If an I/O request is unique (another process isn't making an identical request to merge), it can sort the queue based on, for example, which sector of the disk you're reading from.

- A popular scheduler is the linus elevator.

## The Linus Elevator

- the Linus elevator is a I/O scheduler that was in the linux kernel version 2.4. It was updated in version 2.6.
- This scheduler performs both front and back merging.
    - The type of merging performed depends on the location of the existing adjacent requests.


- The linus elevator does two things:
    - Front merging.
    - Back merging.

- If the request occurs before a existing request, it's placed before the request, **front merging**
    - example, if the existing request wants to access sector 20-29, a new request wanted 10-19 will be placed before the existing one.
- if the request occurs after an existing request, it's placed after the request, **back merging**
    - Like the front merging example, but the other way.


- If merging fails, it'll find anotehr spot where it fits. if that fails, the request is stored at the end of the queue.

- If an existing request is in the queue, that's older than a predefined threshold, the new request is added to the tail.

### cons of Linus Elevator

- The case that the existing request is too old could lead to starving requests.

## Request starvation

- Request starvation is an instance of the problem where I/O writes starve the I/O reads.
    - Write operations can usually be commited to disks when the kernel gets to them, and it's asynchronous.
    - Read operations, on the other hand, is synchronous, and waits for other I/O requests to finish before reading.

- The write requests causes the read requests to wait, starving read starvation.

## Deadline I/O Scheduler

- The deadline I/O Scheduler is associated with an expiration time.
    - By default, it's 500 milliseconds for reads and 5 seconds for writes.
- The deadline I/O scheduler maintains a request queue, sorted by the physical location on the disk, known as the **sorted queue**.
- In addition to the insertion at the sorted queue:
    - read requests are inserted into a read queue.
    - write requests are written to the write queue

- Under **normal operations**, the Deadline I/O scheduler will read from the head of the sorted queue, and place it into the dispatch queue, which is then fed into the disk drive.
- If either the head of the write or read queue expires, the request on the queue is handled instead of handling from the sorted queue.
    - This is what prevents request starvation.


## Deadline I/O Scheduler - Cons

- The Deadline I/O scheduler is great for minimizing read latency, but comes at the cost of global throughput.
- When the system undergoes heavy write activity, the Deadline I/O scheduler will rush to handle read requests while doing write requests.
    - You alternate between reads and writes too often, affecting performance.

## Anticipatory I/O Scheduler

- The anticipatory I/O scheduler uses the Deadline I/O scheduler as it's base:
    - It has a sorted, read, write and dispatch queue, which is similar to the Deadline I/O scheduler.
    - Unlike the scheduler, the Anticipatory I/O scheduler introduces an **anticipation heuristic**.

- The anticipatory I/O scheduler will handle reads similarly to the Deadline I/O scheduler, being placed on the queue, and handled if the time expires.
    - It differs, as after the I/O is handled, the scheduler will wait a few milliseconds, anticipating another read request.
        - This ensures the I/O reads are handled so it doesn't have to switch between reads and writes.

- To know how to correctly anticipate reads, the scheduler does statistics and associated heuristics.
    - These numbers are used to set the pause time.

## CFQ Scheduler

- The Complete Fair Queuing (CFQ)

- The scheduler assigns incoming I/O requests to specific queues based on the process that created the request.
- There's also a final queue for the real I/O requests
- Afterwards, round robin is used to take the I/O requests from each process, and is placed in the final queue.
    - This ensures each process gets it's fair share to I/O

## Noop I/O Scheduler

- The Noop scheduler doesn't really do do sorting or seek-prevention.
- It's simple:
    - When it receives a request, it puts it in an adjacent request.

- The Noop I/O scheduler is intended for block devices that are truly random-access, like flash memory cards.

## Setting linux I/O scheduler on boot

- You can set the I/O scheduler on boot on the kernel command line with `elevator=foo`, where for foo, you'd write:
    - `as` for anticipatory
    - `cfq` - for complete fair queuing
    - `deadline` for deadline
    - `noop` for Noop