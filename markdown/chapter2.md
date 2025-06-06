# 2. Flow Control

## Bollean Values, comparison operators, boolean operators

**Boolean Values**

- These are data types with two possible values: 
    - `True`
    - `False`

**Comparison operators**

| Operator | Meaning |
| --- | --- |
| `==` | Equal to |
| `!=` | Not equal to |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal to |
| `>=` | Greater than or equal to |

**Binary Boolean Operators**

- There are multiple operators:
    - `and`, similar to `&&`
    - `or`, similar to `||`
    - `not`, similar to `!`


## Elements of flow control

- There are a couple of elements that facilitate flow control
    - **Conditions**: A condition statement in an expression that evaluates to true/false.
    - **blocks of code**: We group lines of python code into blocks.
        - We identify blocks with indentations.

```py
if name == 'Mary':
    print('Hello, Mary!')
if password == 'swordfish':
    print('Access granted.')
else:
    print('Access Denied')
```

## Flow control statements, if and loops

- there are many important clauses

### If statements

- `if` statement: just like if statements.
- `else` statement: just like else statements.
- `elif` statements: Similar to else if statements.

### while loop statements

- `while condition:`
```py
spam = 0
while spam < 5:
    print('spam')
    spam = spam + 1
```

### for loop statements

- let's say you want to run a particular block a fixed number of times.
- You can use the for loop statement and range() function

```py
for i in range(5):
    print('jimmy is ' + str(i) + ' years old')
```

## `break`/`continue` keyword

### break statements

- break will leave a while loop's clause early

```py
while True:
    print('Please enter yo name')
    name = input()
    if name == 'your name':
        break
print('Thank you!')
```

### Continue statement

- Continue statements are used in loops. They will just jump to the start of the loop.

```py
while True:
    print('Who are you?')
    name = input()
    if name != 'Joe':
        continue
    print('hello joe. whot\'s your password?')
    password = input()
    if password == 'swordfish':
        break
print('Access granted')
```

## Range function


````py
for i in range(5): # Will print from 0 to 4
    print(i)

for i in range(12,16): # will print from 12 to 15
    print(i)

for i in range(0,10,2): # will print from 0 to 10, and increment i by 2
    print(i)
````


## Importing modules

- Python programs can call a basic set of functions, which are part of the **standard library**
    - This includes `print()`, `input()`, etc.

- You can import modules using the `import` keyword.

```py
import random

for i in range(5):
    print(random.randint(1,10))
```

- You can import multiple libraries on the same line, separating by comma

```py
import random, sys, os, math
```

## ending program early with sys.exit()

```py
import sys # you need to import sys to use sys.exit()

while True:
    print('Type to exit.')
    response = input()
    if response == 'exit':
        sys.exit()
    print('you typed ' + response + '.')
```

