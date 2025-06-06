# Linux. USB Drivers

## Initial Device Intentions - USB

- The goal of USB was:
    - connection of the PC to telephone
    - Easy to use
    - Allow for port expansion.

- USB is heirarchial, and there's a single host controlling the whole thing.
- The host uses a master/slave protocol to communicate with attached USB devices.

## USB - Host Controller

- The host controller has certain protocls:
    - OHCI (Open host Controller Interface)
    - UHCI (Universal Host Controller interface)

## USB - hubs

- hubs are usb devices that can connect to other usb devices.
- They have their own controller that manages the devices.

## Full Speed vs Low Speed devices

- low speed devices communicate at 1.5 MBit/s and have only limited capabilities
    - These are mice, keyboard, joysticks, etc.
- Full speed devices (audio and video systems), can communicate up to 12 Mbit/s.

## Data Flow Types

- Communication via USB is done in two directions, using 3 different transfer types:
    - Data directed from the host to device is downstream, or **OUT transfer**.
    - Data directed from the device to host is upstream, or **IN transfer.**

- The other types of transfer include:
    - **control transfer**: short packets, used to configure devices.
        - This include commands like, `GET_STATUS`, `SET_FEATURE`
    - **bulk transfers** are used to request or send reliable data packets using the full bus bandwidth (used commonly by scanners for example).
    - **interrupt transfer**: Similar to bulk transferred, here, the device will send data periodically (every 1ms, for example).
    - **Isochronous transfer**: Send/receive data in real time without reliability. used commonly for audio and video devices.

## Enumeration and Device Descriptor

- When you attach a device to the bus, it'll be assigned a unique number between 1 and 127.
- After, a descriptor is made.
    - The descriptor provides information about the device and it's properties.

- There exists a device descriptor heirarchy.

## Device Descriptors - standard descriptor

https://linuxusbguide.sourceforge.net/USB-guide-1.0.9/descr.gif

- The **device descriptor** describes general information about the device.
- **configuration descriptor** provides information about the specific device configuration.
- **interface descriptor** describes a specific interface within the configuration.
    - here, you can change settings. For example, whether you want to activity the camera, mic, etc.
- **endpoint descriptor**: Provides information needed by the host to determine bandwidth requirements.
- **string descriptors**: just provide additional information in human readable format.

## Device Classes


- The standard device and interface descriptors have attributes relating to it's classification, there are three classifications:
    - class
    - sub-class
    - protocol

- We use these fields to let the host system match the device to the appropriate driver.
- The values for these fields are standardized by the USB device working gorup
- Devices with a similar functionatily are grouped into classes
    - For example, storage devices do similar things.
- A class specification defines the expected behavior and structure of all devices in the class.
    - It provides the framework, but you'll have to implement the features.

## HID

- HID are Human Interface Devices, which are devices used by humans.
    - Examples include keyboard, mouse, etc.

## USB Device Drivers

- When it comes to USb, there are cases where the whole USB device is handled by a single driver.
- Othertimes, each interface will have it's own driver.


## Linux USB Subsystem

https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcT1aNXlsicwaxJdleX8nrJG7ymaCGykaPQ0uA&s

- Linux has a subsystem known as the "USB Core".
    - It has specific APIs to support USB devices and host controllers.
- It also abstracts the hardware and device dependent parts, by defining structures, macros and functions.

- We can break device drivers into two halves:
    - USB Device Drivers
    - Host Controller Drivers
- The USB core provides APIs for both of them.


## USB Device Driver Framework

- USB device drivers are registered and deregistered at the subsystem.
    - A driver needs to register two entry points and it's names.
- The driver may specify a couple of file operations and a minor number.
    - You have register 16 USB devices with one driver.
- The major number for all USB devices is 180.

## usb_driver struct

```c
struct usb_driver{
    const char* name;
    void* (*probe)(struct usb_device*, unsigned int, const struct usb_device_id* id_table);
    void (*disconnect)(struct usb_device*, void*);
    struct list_head driver_list;
    struct file_operations* fops;
    int minor
    struct semaphore serialize;
    int (*ioctl)(struct usb_device* dev, unsigned int code, void* buf);
    const struct usb_device_id* id_table;
}
```

- The name is the name of the module
- probe is the function called when you plug the usb device in
- disconnect is the function called when you unplug the usb device
- driver_list is internal, you can set it to `{NULL, NULL}`.
- fops: the file operations, (read, write, etc.)
- minor (minor number, has to be a multiple of 16.)
- serialize: a semaphore, doesn't really matter
- ioctl: Control what happens during IO syscall
- id_table: the array of devices compatible with driver (based on vendor id and serial number.)

## Registering usb device

- To register a usb driver, you can use `usb_register(struct usb_driver* drv);`
    - This returns 0 on success, otherwise an error value is returned.
- To deregister a usb drivere, you use `usb_deregister(struct usb_driver* drv);`


## Interacting with Other USB Interfaces

### `usb_driver_claim_interface`

```c
void usb_driver_claim_interface(struct usb_driver *driver, struct usb_interface *iface, void *drv_context);
```

* Claims an additional interface on the same device, used in `probe()`.
* `driver`: Pointer to the struct registered via `usb_register`.
* `iface`: Interface to claim (from the device’s `usb_config_descriptor`).
* `drv_context`: Pointer to the driver's private data (device-specific context).

---

### `usb_interface_claimed`

```c
int usb_interface_claimed(struct usb_interface *iface);
```

* Returns non-zero if the interface is already claimed by another driver.
* Returns `0` if the interface is available.

---

### `usb_driver_release_interface`

```c
void usb_driver_release_interface(struct usb_driver *driver, struct usb_interface *iface);
```

* Releases a previously claimed interface.

---

### `usb_match_id`

```c
const struct usb_device_id *usb_match_id(struct usb_interface *interface, const struct usb_device_id *id);
```

* Matches a USB interface against a list of `usb_device_id` entries.
* Returns a pointer to the matching entry, or `NULL` if none match.
* Commonly used in `probe()` to find the appropriate device ID from the driver's ID table.

---

Let me know if you want a real-world usage example or a memory layout reference for `usb_interface` and `usb_config_descriptor`.


## Configuring USB device - getting the options

### `usb_device` structure


```c
struct usb_device{
    struct usb_config_descriptor* actconfig; /* active configuration*/
    // ...
    struct usb_device_descriptor descriptor; /* Descriptor */
    struct usb_config_descriptor* config /* list of all the configurations */
}
```

### Accessing all available configuration descriptors

```c
for(int i = 0; i < dev.descriptor.bNumConfigurations; i++){
    struct usb_config_descriptor* cfg = &dev->config[i]; /* how you get a config.*/
}
```

### Accessing available interfaces

```c
for(int j = 0; j < cfg->bNumInterfaces; j++){
    struct usb_interface* ifp = &cfg->interface[j];
}
```

### Accessing alternate settings of interface

```c
for(int k = 0; k < ifp->num_altsetting; k++){
    struct usb_interface_descriptor* as = &ifp->altsetting[k];
}
```

### accessing all endpoint descriptors

```c
for(int l = 0; l < as->bNumEndpoints; l++){
    struct usb_endpoint_descriptor* ep = &as->endpoint[k];
}
```

## Configuring USB device - setting options

- To query the functions, there are multiple functions to facilitate that:

```c
int usb_set_configuration(struct usb_device* dev, int configuration);
```
- this will activate a particular configuration.
    - The configuration should be: `0 <= configuration < dev->descriptor.bNumConfigurations`

```c
int usb_set_interface(struct usb_device* dev, int interface, int alternative);
```
- This function sets an alternate setting for a specified interface on a USB device.
    - `dev` is the device. 
    - interface should be `0 <= interface < dev->actconfig->bNumInterfaces`
    - `alternate` should be `0 <= alternate < dev->actconfig->interface[interface].num_altsetting`

```c
int usb_get_descriptor(struct usb_device* dev, unsigned char desctype, unsigned char descindex, void* buf, int size);
```
- This function is meant to parse extended or vendor specific descriptors.
    - `dev` is your device
    - `desctype` is the descriptor to request (USB_DT_DEVICE for example).
    - descindex is the index of the descriptor (manufacturer is 1, product is 2)
    - buf is a buffer where to write the descriptocdr data
    - size is the number of bytes to read into buf.

```c
int usb_get_string(struct usb_device* dev, unsigned short langid, unsigned char index, void* buf, int size);
```
- `dev`: device
- `langid` language id (0x0409 for english)
- `index` is the index of the stsring descriptor
- `buf` buffer where to write data
- `size` size of buffer.

```c
int usb_string(struct usb device *dev, int index, char *buf,
size t size);
```
- This is a simpler version of `usb_get_string`, but it converts unicode to ascii.

```c
int usb_get_status(struct usb_device* dev, int type, int target, void* data);
```
- gets the status of devie
- type is the part you want to check
- targe tis the endpoint or interface number.
data is a pointer to a 2 byte buffer, where the status will be stored.

## USB Transfer

- To transfer data, you use a transfer structure known as the URB Request Block.
- This contains all the parameters needed to seup any USB transfer type.
- The transfers are done asynchronously to the USB call, and the completion is signaled with a callback function.


- it's a big structure, don't bother remembering everything


## URB Functions

```c
purb_t usb_alloc_urb(int iso_packets);
```
- This creates the actual urb.
- We need to specify the number of `iso_frame_desc` structs at the end.
- If it succeeds, it returns the URB structure, preset to 0.
- If it fails, it returns null

```c
void usb_free_urb(purb_t purb);
```
- deallocates the urb.

```c
int usb_submit_urb(purb_t purb);
```
- This function sends a transfer request asynchronously to the USB core.

```c
int usb_unlink_urb(purb_t purb);
```
- this cancels a scheduled request before it's completed.

## PRB Macros

- There are many macros you can 

```c
FILL_CONTROL_URB(purb, dev, pipe, setup packet, transfer_buffer, transfer buffer length, complete, context);
FILL_BULK_URB(purb, dev, pipe, transfer_buffer,transfer_buffer_length, complete, context);
FILL_INT_URB(purb, dev, pipe,transfer_buffer,transfer_buffer_length,complete,context,interval);
FILL_CONTROL_URB_TO();
FILL_BULK_URB_TO();
```