# 8: Reading and Writing Files

## Files and File paths

- A file has two properties:
    - Filename: Written in one word.
    - path: Specificies the location of file on computer.

## back/forward slash on Windows vs. OSX/Linux

- Windows uses `\` and linux/macos uses `/`.
- It's ideal for your program to differentiate between the two.
- To do this, you can use `os.path.join()`, which will concatenate the values into it's file path.

```py
import os
print(os.path.join('usr','bin','ls')) # usr\\bin\\spam on windows
filename = 'accounts.txt'
print(os.path.join('C:\\Users\\asweigart', filename)) # C:\Users\asweigart\accounts.txt on windows
```

## Current Working Directory, changing directory

- To get the current working directory, you can use `os.getcwd()`
- To change directories, you can `os.chdir()`
```py
import os
print(os.getcwd()) # /Users/arunfelix/Documents/ on unix
os.chdir(os.path.join(os.getcwd(),'school'))
print(os.getcwd()) # /Users/arunfelix/Documents/school on unix
```

## Creating new folders

- To create a new folder, you'd use `os.makedirs()`
- It will recursively create the folders needed in order for the path to exist

```py
import os
os.mkdir(os.path.join(os.getcwd(),'pa2/part1'))
```

## `os.path` module

- The os.path module has many helpful functions for file paths:

| function | description |
| --- | --- |
| `os.path.abspath(path)` | Will create an absolute path given a filename path |
| `os.path.isabs(path)` | Will check if path is an absolute path |
| `os.path.relpath(path,start)` | will give the relative path from the `start` path `path`. If start isn't providing, current directory is used |
| `os.path.dirname(path)` | give the dirname before the base name |
| `os.path.split(path)` | similar to dir name, but will separate the basename and dirname. |

## Getting file size and folder content

- you can get the size of a file using `os.path.getsize(path)`, returning the number of bytes.
- you can use `os.listdir(path)` to list all the files in a directory.
    - returns an array of files/directories in path.

## checking path validity

- You can check if a path exists using `os.path.exists(path)`
- You can check if the path is a file using `os.path.isfile(path)`
- You can check if path is a directory using `os.path.isdir(path)`

## Opening files

- You can open a file using the `open()` function.

```py
fd = open("/usr/bin/ls")
```

## reading from file

- Once you open the file, you can read it:
    - with `.read()` if you want to read everything
    - with `.readlines()` if you want to split it line by line.

## Writing to file

- You can write content to a file using write.
- You must provide `write mode` or `append mode` permissions before writing to a file.
    - Write mode means you can overwrite the existing file, add 'w' as arg to open
    - Append mode means you can add text to the end of an existing file, add 'a' as arg to open.

## saving variables with shelve module

- You can save variables even after closing a program using the shelve module.

```py
import shelve

shelfFile = shelve.open('mydata')
cats = ['zophie','pooka','simon']
shelfFile['cats'] = cats # will add cats variable to shelf
selfFile.close() # save variable to file
``` 

- Later, you can access the variables in another program.

```py
import shelve
shelfFile = shelve.open('mydata')
print(shelfFile['cats']) # ['zophie','pooka','simon']
shelfFile.close()
```

## Saving variables with pprint.pform() function

- You can also use `pprint.pformat()` to save variables as a string.

```py
>>> import pprint
>>> cats = [{'name': 'Zophie', 'desc': 'chubby'}, {'name': 'Pooka', 'desc': 'fluffy'}]
>>> pprint.pformat(cats)
"[{'desc': 'chubby', 'name': 'Zophie'}, {'desc': 'fluffy', 'name': 'Pooka'}]"
>>> fileObj = open('myCats.py', 'w')
>>> fileObj.write('cats = ' + pprint.pformat(cats) + '\n')
83
>>> fileObj.close()

# later

>>> import myCats
>>> myCats.cats
[{'name': 'Zophie', 'desc': 'chubby'}, {'name': 'Pooka', 'desc': 'fluffy'}]
>>> myCats.cats[0]
{'name': 'Zophie', 'desc': 'chubby'}
>>> myCats.cats[0]['name']
'Zophie'
```