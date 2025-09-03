# 1. The perfect commit

## How to create perfect commit

- You need to do two things.
    - add the right changes
    - create a good commit message

### adding right changes

- Adding all the changes isn't ideal.
    - Rather, it's better to group particular changes in each commit.
- The bigger a commit, the harder it gets to understand.
- you can add particular files, or parts of a file to commit.

### Creating good commit message

- commits consist of two things:
    - **subject**: concise summary of what happened
    - **body**: more detailed explatition:
        - what's now different than before
        - reason for change
        - anything to watch out for.
    
```
added the electric feature to project

this is my body, where I do things. hahaha
```
- Adding the space between the subject and body.

## adding parts of file to staging area

- you can do this with `git add -p <filename>`.
    - it'll iterate through the changes to add to staging area
    - you write `y`/`n` depending on if you want to add a particular change.


