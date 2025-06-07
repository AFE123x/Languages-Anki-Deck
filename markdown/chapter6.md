# 6. Manipulating Strings

## raw strings, multiline strings

### raw strings

- you can place an r before the beginning quotation mark to make it a raw string. 
    - raw strings ignore all escape characters, and print any backslash that appears in the string.

```py
>>> print(r'This is Carol\'s cat.')
This is Carol\'s cat.
```

### multiline strings

- You can use ```'''```, triple quotes to print multiline strings:

```py
print('''Dear Alice,
Sorry things didn't work out.
Sincerely,
bob''')
```

## Multiline comments

- You can have multi line comments by using triple quotations

```py
"""
this is a big beautiful comment
"""
print("contra is a good game")
```

## slices with strings

- Exactly like arrays

```py
>>> spam = 'Hello world!'
>>> spam[0]
'H'
>>> spam[4]
'o'
>>> spam[-1]
'!'
>>> spam[0:5]
'Hello'
>>> spam[:5]
'Hello'
>>> spam[6:]
'world!'
```

## `in` and `not` in strings

- You can use in and not to check if a word is in a string

```py
>>> 'Hello' in 'Hello World'
True
```

## `upper()`, `lower()`, `isupper()`, `islower()`

- `upper()` and `lower()`, will convert a string to either uppercase or lowercase.

- `isupper()` and `islower()` will check if a string is entirely uppercase or lowercase.

```py
>>> spam = 'Hello world!'
>>> spam = spam.upper()
>>> spam
'HELLO WORLD!'
>>> spam = spam.lower()
>>> spam
'hello world!'
>>> spam = 'Hello world!'
>>> spam.islower()
False
>>> spam.isupper()
False
>>> 'HELLO'.isupper()
True
>>> 'abc12345'.islower()
True
>>> '12345'.islower()
False
>>> '12345'.isupper()
False
```

## `startswith()` and `endswith()` methods

- these functions will return true or false, if the string either starts with or ends with a word.

```py
>>> 'Hello world!'.startswith('Hello')
True
>>> 'Hello world!'.endswith('world!')
True
>>> 'abc123'.startswith('abcdef')
False
>>> 'abc123'.endswith('12')
False
>>> 'Hello world!'.startswith('Hello world!')
True
>>> 'Hello world!'.endswith('Hello world!')
True
```

## string - `.join()` and `.split()`

- `join()` will combine a list into a string.
- `split()` will seperate words based on a specific item
```py
>>> ', '.join(['cats', 'rats', 'bats'])
'cats, rats, bats'
>>> ' '.join(['My', 'name', 'is', 'Simon'])
'My name is Simon'
>>> 'ABC'.join(['My', 'name', 'is', 'Simon'])
'MyABCnameABCisABCSimon'
>>> 'MyABCnameABCisABCSimon'.split('ABC')
['My', 'name', 'is', 'Simon']
>>> 'My name is Simon'.split('m')
['My na', 'e is Si', 'on']
```

## Removing Whitespace with `strip()`, `rstrip()` and `lstrip()`

- you can remove white space on the left, right, or both sides with `strip()`, `rstrip()` and `lstrip()`.

```py
>>> spam = '     Hello World '
>>> spam.strip()
'Hello World'
>>> spam.lstrip()
'Hello World '
>>> spam.rstrip()
'     Hello World'
>>> spam = 'SpamSpamBaconSpamEggsSpamSpam'
>>> spam.strip('ampS')
'BaconSpamEggs'
```

