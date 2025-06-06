# 1. Python Basics

## Python Expressions, precedence

| Operator | Operation | Example | Evaluates to ... |
| --- | --- | --- | --- |
| ** | exponent | 2 ** 3 | 8 |
| % | modulus | 22 % 8 | 8 |
| // | integer division | 22 // 8 | 2 |
| / | division | 22 / 8 | 2.75 |
| * | multiplication | 3 * 5 | 15 |
| - | subtraction | 5 - 2 | 3 |
| + | addition | 2 + 2 | 4 |

- precedence from left to right: `**`, `*`, `/`, `//`, `%`


## Data type

- A data type is a category of value, where every value belongs to one type.
- The most common types include integers, floating point numbers and strings.

| Data type | Examples |
| --- | --- |
| Integers | -2, -1, 0, 1, 2, 3, 4, 5 |
| Floating-point numbers | -1.25, -1.0, -0.5, 0.0, 0.5, 1.0 |
| Strings | 'a', 'aa', 'aaa' |

## String Concatenation and Replication

```py
>> 'Alice' + 'Bob'
'AliceBob'
```
- String Concatenation lets you combine strings.

```py
>>> 'Alice' * 5
'AliceAliceAliceAliceAlice'
```
- This is string replication

## Storing Values in Variables

- Variables are like a box where you can store single values.

```py
>>> spam = 40
>>> spam
40
>>> eggs = 2
>>> spam + eggs
42
>> spam + eggs + spam
82
>> spam = spam + 2
>> spam
42
```

## Getting user input

- You can get user input with `input()`
```py
print('Hello world!')
print('What is your name?')
myName = input()
print('It\'s good to meet you, ' + myName)
```

## Getting length of string, or array type

```py
>>> len('hello world')
11
>> len('')
0
```

## `str()`, `int()` and `float()` functions

- These are pretty much like `sprintf`, `atoi` and `atof` in c

```
>>> str(29)
'29'
>>> int('42')
42
>>> float('3.14')
3.14
>>> int(3.14)
3
```

