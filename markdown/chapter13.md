# 13. The Virtual Filesystem

## What is a Virtual Filesystem?

- The VFS is the subsystem of the kernel that implements the file and filesystem-related interfaces provided to user-space programs.
    - In order for the `cp` utility to work, it needs a VFS to mediate this.
    - Let's say we want to coppy a file from a harddrive to an sd card, the VFS handles the transfer of data between different filesystems.
- VFS provides a common file model that can represent any filesystem general feature set and behavior.

## Filesystem abstraction layer

- The linux kernel provides an abstraction layer it's low level filesystem interface.
    - This lets linux support different filesystems, even with different features.
- VFS provides a common file model that can represent any filesystem general feature set and behavior.

- When you interact with files, you don't need to worry about the file _system methods

### Example - Write

- `write(fd,buf,len)` is a posix function.
    - This system is handled by a VFS system call called `sys_write()`
    - sys_write uses the write method for the filesystem it's writing to.


## UNIX Filesystems

- Unix provided four basic filesystem-related abstractions:
    - files
    - directories
    - inodes
    - mount points

- The filesystem is a heirarchial storage of data adhering to a specific structures

- The typical operations done on filesystems include creation, deletion and mounting.

- On unix, file systems are mounted at a specific mount point in a global heirarchy, known as a namespace.
    - This lets all mounted filesystems to appear as entries in a single tree.

- In contrast, windows breaks up the file namespace into letters, like `c:`, which is stupid and inferior.

## What is a file, typical file operations

- A file is simply on ordered string of bytes.
    - The first byte indicates the beginning of the file, and the last byte marks the end of a file.
- Each file will have a readable name for identification by both the system and user.

### Typical file operations

- Typical file operations include:
    - read
    - write
    - create
    - delete

## Directories

- Files are organized in directories.
    - A directory is a folder.

- In UNIX, a directory is a normal file that lists the files cointained inside it.

## Inode

- Unix separates the concept of a file from the assocated information, like access permissions, size, owner, etc.
    - The metadata is contained in a data structure known as the inode, short for index node.
- The inodes are tied together with the filesystem's own control information, which is stored in the **superblock**

## Superblock

- A superblock is a data structure containing information about the filesystem as a whole.

## VFS - Four Primary object types 

- There are four primary object types:
    - superblock object: Represents specific filesystem
    - inode object: represents a specific file
    - dentry object, represents a directory entry
    - file object, represents an open file associated with a process.


## Operations object

- The operations object is contained within the `superblock`, `inode`, `dentry` and `file` objects (these are our primary objects).
    - This operations object describes methods that cna kernel can invoke against the primary objects

- The operations include:
    - `super_operations` object, which has methods that the kernel can invoke on a specific filesystem.
    - `inode_operations` object, which can invoke methods on specific files
    - `dentry_operations` which can invoke methods on directory entries
    - `file_operations` objects, which contain methods a process can invoke on an open file, like `read()` and `write()`.


- These are structures that contain function pointers.