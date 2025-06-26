# 9. Organizing Files

## shutil module

- This library lets you copy, move, rename and delete files in your python program.

## Shutil, copying files and folders

- You can copy files using the `.copy()` method
- You can copy an entire folder using `.copytree()` method
```py
import shutil, os

shutil.copy(os.path.join(os.getcwd(),'chapter9-5'),os.path.join(os.getcwd(),'chapter9.md'))
shutil.copytree(os.path.join(os.getcwd(),'markdown'),os.path.join(os.getcwd(),'markdown2'))
```

## Moving and renaming file/folder

to move a file/folder, you can use the `.move()` method

```py
import shutil
shutil.move('/usr/bin/ls','/usr/bin/ls2') # (src, dest)
```

## Permanently Deleting Files and Folders

- `os.unlink(path)` will remove a file
- `os.rmdir(path)` will remove an empty folder
- `shutil.rmtree(path)` will delete the folder and files in the folder.

**Be mindful**: deletes are permanent.

## safe deletes

- Safe deletes mean it'll simply move your file to your computer's recycling bin.
- You can use `send2trash`, which you install using `pip install send2trash`

```py
>>> import send2trash
>>> baconFile = open('bacon.txt', 'a') >>> baconFile.write('Bacon is not a vegetable.')
25
# creates the file
>>> baconFile.close()
>>> send2trash.send2trash('bacon.txt')
```

## Walking a Directory Tree

- You can iterate through all directories using the `os.walk(path)` method
- It will iterate through all directories.
- The function returns an iterator of tuple, `(path,folders,files)`

## Zip Files - viewing metadata

- You ca nwork with ZIP files with the `zipfile` module.
- You first need to create a zipfile object:


```py
import zipfile

exampleZip = zipfile.ZipFile('example.zip')
print(exampleZip.namelist()) # print out contents of zip file
print(exampleZip.file_size) # prints out size of uncompressed file
print(exampleZip.compress_size) # prints size of compress zip.
exampleZip.close() # need to close the file at end
```

- `namelist()` will print all the contents in the zip file.

## `Zipfile`- Extracting

- You can also, unsurprisingly, extract files from a zip file.

```py
import zipfile

exampleZip = zipfile.ZipFile('example.zip')
exampleZip.extract('spam.txt', '.') # extract spam.txt into the current directory
exampleZip.extractall() #extracts all files
exampleZip.close() 
```

## `ZipFile` - creating zip

- You can create zip files by appending `'w'` to the constructor

```py
import zipfile

newZip = zipfile.ZipFile('new.zip', 'w')
newZip.write('spam.txt', compress_type=zipfile.ZIP_DEFLATED)
newZip.close()
```
- Write will compress the file, and add it to the zip file.