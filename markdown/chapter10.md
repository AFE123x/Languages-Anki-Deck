# 10. Starting Up - The init Program


## What does the init system do?

- At boot, it'll start daemon programs and configure system parameters to get the system in a working state
- Launches a login doemon, that lets you login.
- adopts orphaned processes.
- respond to the termination of init's child, via the SIGCHILD interrupt.
- restarts daemons that were terminated.
- handles system shutdown.


## Main init programs

- there are three popular init programs
    - busybox: simplest
    - system v: medium complexity
    - systemd: complex init program.
    

## Configuring BusyBox init

- You can add commands/rules to the `/etc/inittab` program.
    - this doesn't scale well as you add more programs
- You can use shell scripts placed in the `/etc/init.d` directory.
    - busy box has two scripts: rcS and rcK
    - rcS will run scripts on start that start with capital S.
    - rcK will run scripts on kill that start with capital K.

## System V init

- System V has more features than BusyBox, but has been replaced with systemd.

- Compared to busybox, System V has:
    - boot scripts are in a more modular format.
    - has a concept of runlevels. 
        - This will group scripts depending on the state of the system.

## system V inittab

- system V also uses `/etc/inittab`, but adds runlevels:

```bash
id:runlevels:action:process
si::sysinit:/etc/init.d/rcS # example
```

## adding new daemon

- lets say you have a program written as a traditional daemon.

- You will have a init.d script similar to the following

```bash
#! /bin/sh
case "$1" in
    start)
        echo "Starting simpelserver"
        start-stop-daemon -S -n simpleserver -a /usr/bin/simpleserver;;
    stop)
    echo "Stopping simpleserver"
    start-stop-daemon -K -n simpleserver;;
    *)
    echo "Usage: $0 {start|stop}"
    exit 1
esac
exit 0
```

- `start-stop-daemon` is a helper function that makes it easier to manipulato background processes..

## systemd

- this is a more common and bigger init system.
- instead of scripts, it uses config files.
- It accounts for explicit dependencies between services.