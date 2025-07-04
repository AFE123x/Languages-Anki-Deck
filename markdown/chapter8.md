# 8. Updating Software in the Field

## What would you want to update? Obstacles

- You'd probably want to update the following:
    - bootloader (hardest to update, least frequently updated)
    - kernel
    - root filesystem
    - system applications (easiest to update, most frequently updated)
    - device specific data.

## Basics of software updates consideration

- We want to software update mechanism to be:
    - robust: An update shouldn't make a device unusable
    - fail-safe, so there's a fall back mode if all else fails.
    - secure, to prevent device from being hijacked by people installing unauthorized updates.

## System updates - atomicity

- Updates should be atomic: there should be no stage where only part of the system is update while others aren't.
- The full update shouldn't be interrupted.

- To achieve this, you need to install the update alongside the running system, then switch from the old system to new system.

## System updates - approaches to atomicity

- There are two approaches to achieving atomicity:
    - have two copies of the root fs and other components (symmetric image update or A/B image update)
    - have two or more copies of the rootfs is different subdirectories of the system partition, then use **chroot** to select one of them (known as **atomic file update**)

## System updates - fail-safe, points to failure

### Fail safe - ideals
- The next problem to consider, recovering from an update that was installed correctly, but contains code that stops the system from booting.
    - We'd want to system to solve this and revert to a previous working image.

### Fail safe - points of failure

- kernel panic: Caused by possible bug in kernel driver, or unable to run init system.
    - in this case, you'd want to reboot the system after a few seconds of a panic, which can be set in the kernel configuration, or in the kernel command line.
- Oops (fatal kernel error): Here, you can make the system panic on an oops.
    - This can be set on the command line or in the kernel config.
- kernel launches init successfully, but main application fails to run:
    - In this case, you'd want a watchdog.
    - A watchdog is a timer that's frequently reset. If it doesn't reset within a certain time period, the system is restarted.
- bootloops: if the kernel panics or the watchdog times out every time, the system will reoot continueally.
    - There'll usually be code in the boot loader to detect this, and revert to a previously working version.
    - uboot has a few variables, like `bootcount`, `bootlimit`, `altbootcmd`

## uboot - boot loop related variables, how to reset boot count

- to handle a bootloop, uboot has three variables:
    - `bootcount`: counted every time the processor boots, reset when system is successfully booted
    - `bootlimit`: a limit to how many times the kernel can boot.
    - `altbootcmd`: if the kernel fails to boot, and the bootcount exceeds bootlimit, the `altbootcmd` command is used, or enters a recovery mode.

- To reset the boot count, we can use U-boot utilities that will be able to access uboot environment variables at runtime.
    - `fw-printenv` prints value of uboot variable.
    - `fw_setenv` sets the value of uboot variable.

- The bootloader will create a configuration file, `/etc/fw_env.config` containing the location and size of the uboot environment.

- To prevent unecessary writes to flash, you can set the `upgrade_available` to only increment `bootcount` if `upgrade_available` is set to 1.

## Making updates secure

- you want to implement an update mechanism to provide a reliable source to install security patches and other features.
- We don't want others to use the same mechanism to install unauthorized versions of software.

- One vulnerability are fake updates.
    - To prevent this, we need to authenicate update server before starting the download.
    - We also need a secure transfer channel, like HTTPS, to guard against tamping with the download stream.

- Another concern is the authenticity of updates provided locally.
- You can detect these by using secure boot protocols in the bootloader.
- The kernel image can be signed at the factory with a digital key, so the bootloader can check the key before loading the kernel, and refuse boot if they don't match.

## Symmetric Image update

- This is one approach to applying software updates.
- Here, well have two copies of the OS, labelled A and B.
- The bootloader will have a flag, to indicate whether to boot OS image A or B.

- Initially, the flag will be set to boot from A.
- When the system updates, OS A will apply the update and overwrite B, then set the flag to boot from B.
- Every update, the two alternate.


- This works, but has drawbacks:
    - updating an entire filesystem, which is large, puts a strain on the network.
        - this can be resolved by only sending the filesystem blocks that have changed.
    - You need to keep additional storage space for a redundant copy of the root.

## Asymmetric Image Update

- This is similar to Symmetric Image update, but you'll have a minimal recovery OS solely for updating the main one.
- When you install an update, you set the bootflag to point to the recovery OS.
    - If the update's interrupted, the bootloader can go to the recovery OS, letting it resume the update.

- This works, but has drawbacks:
    - when the Recovery OS runs, the device isn't operational.


## Atomic File Updates

- With this approach, you have multiple copies of the root filesystem present in multiple directories of a single filesystem.
    - You'd use `chroot` to choose one of them at boottime
- This will let one directory tree get updated while another one is mounted as the root directory.
- instead of copying unchained files, you can use symlinks, which saves disk space and reduce the amount of data to be downloaded.

- OSTree is an implementation of this idea.

## OTA Updates

- These are **Over the Air** updates.
- these can push software updates to a device or group of devices via a network.

- For this to work, the client will poll the update server occasionally to check if there are any updates pending.

- There are two projects that you can use:
    - Mender in managed made
    - hawkbit