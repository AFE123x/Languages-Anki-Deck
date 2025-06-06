# 3. Functions

## `def` keyword

- The def keyword defines a function


```py
def hello():
    print('howdy')

hello()
hello()
hello() # calling the function
```

## function parameters, keyword arguments

### Function parameters

- you can pass in arguments on functions:

```py
def hello(name):
    print('Hello, ' + name)

hello('Alice')
hello('Bob')
```

### Keyword arguments

- These are basically default arguments.

- in the print function, you can use end or sep to specify what to print at the end, and between arguments.

```py
print('Hello', end = '') # doesn't print new line character
print('World')
print('cats','dogs','mice', sep=',') # will print cats,dogs,mice
```

## function return values

- functions can return values.

```py
def getzero():
    return 0 # we return 0

print(getzero()) # prints zero
```

## global variables

- you can have global variables in python

```py
eggs = 42

def func():
    print('i wanna have ' + eggs + ' eggs.') # this will print 42

func()
```

- To actually modify global variables, you need to use the `global` keyword.

```py
eggs = 42

def spam():
    global eggs
    eggs = 69
spam()
print(eggs) # will print 69
```

## Exception Handling

```py
def spam(divideboi):
    return 42 / divideboi

print(spam(2))
print(spam(0)) # our program will throw a ZeroDivisionError exeption
print(spam(20))
```

- To handle this using a `try-except` block

```py
def spam(divideboi):
    try:
        return 42 / divideboi
    except ZeroDivisionError:
        print('invalid argument')
        return None

print(spam(2))
print(spam(0)) # our program will throw a ZeroDivisionError exeption
print(spam(20))
```