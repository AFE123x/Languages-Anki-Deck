# 4. Lists

## List data type, length of array

- The list is a value that contains multiple values in an ordered sequence.

```py
spam = ['cat', 'bat', 'fish', 'elephant']
print(spam[0]) # prints 'cat'
print(spam[1]) # prints 'bat'
print(spam[2]) # prints 'fish'
print(spam[3]) # prints 'elephant'
print(spam[-1]) # prints 'elephant'
```
- One quirk is you can use negative indices on items. It just wraps around.
### getting length

- You can get the length of an array using `len()`

## list type - Slices

- A slice is a new list containing a part of the original array.
    - slices are formated as `arr[start index: end bounds]`
```py
spam = ['cat', 'bat', 'rat', 'elephant']

new_spam = spam[1:3] # ['bat', 'rat'] doesn't include rat

print(spam[:2]) #prints ['cat', 'bat']
print(spam[1:]) # prints ['bat', 'rat', 'elephant']
```
- You can omit the start or end indexes.

## List Concatenation and List Replications

- Similar to strings, you can use the `+` operator to combine two arrays
- Similar to strings, you can use the `*` operator to replicate an array

```py
>>> [1, 2, 3] + ['A', 'B', 'C']
[1,2,3,'A', 'B', 'C']
>>> ['X', 'Y', 'Z'] * 3
['X', 'Y', 'Z','X', 'Y', 'Z','X', 'Y', 'Z']
```

## Removing/adding items from list

### removing elements from list
- You can remove values from a list using the `del` statement

```py
>>> spam = ['cat', 'bat', 'rat', 'elephant']
>>> del spam[2] # deletes 'rat' from list.
>>> spam
['cat', 'bat', 'elephant']
>>> spam.remove('cat') # will remove element from array.
```

### adding items to list with append and insert

```py
>>> spam = ['cat', 'dog', 'bat']
>>> spam.append('moose')
>>> spam
['cat', 'dog', 'bat', 'moose']
>>> spam = ['cat', 'dog', 'bat']
>>> spam.insert(1, 'chicken')
>>> spam
['cat', 'chicken', 'dog', 'bat']
```
- append will add an item to the end of the list
- insert will add the element at a specific index.

## Checking if item is in list

- you can you the `in` keyword to see if an item is in the list

```py
>>> 'howdy' in ['hello', 'hi', 'howdy', 'heyas']
True
>>> spam = ['hello', 'hi', 'howdy', 'heyas']
>>> 'cat' in spam
False
>>> 'howdy' not in spam
False
>>> 'cat' not in spam
True
```

## Augmented assignment operations

- You can do augmented operations on variables

| old way | augmented way |
| --- | --- |
| `spam = spam + 1` | `spam += 1` |
| `spam = spam - 1` | `spam -= 1` |
| `spam = spam * 1` | `spam *= 1` |
| `spam = spam / 1` | `spam /= 1` |
| `spam = spam % 1` | `spam %= 1` |


## Methods

- the method is similar to a function, but is associated with an instance of a variable.

```py
>>> spam = ['hello', 'hi', 'howdy', 'heyas']
>>> spam.index('hello')
0
>>> spam.index('heyas')
3
>>> spam.index('howdy howdy howdy')
Traceback (most recent call last):
    File "<pyshell#31>", line 1, in <module>
        spam.index('howdy howdy howdy')
ValueError: 'howdy howdy howdy' is not in list
```


### array, sorting values

- You can sort items in a list using `.sort()`

```py
>>> spam = [2, 5, 3.14, 1, -7]
>>> spam.sort()
>>> spam
[-7, 1, 2, 3.14, 5]
>>> spam.sort(reverse=True) # you want it in reverse order
>>> spam
[5, 3.14, 2, 1, -7]
```

## tuples, converting between list and tuple

```py
eggs = ('hello', 42, 0.5)
>>>eggs[0]
'hello'
```
- Tuples let you hold different types.
- The difference is that tuples are immutable, so you cannot modify them.

- you can convert between lists and tuples

```py
>>> tuple(['cat', 'dog', 5])
('cat', 'dog', 5)
>>> list(('cat', 'dog', 5))
['cat', 'dog', 5]
>>> list('hello')
['h', 'e', 'l', 'l', 'o']
```

### copy and deepcopy functions

- When you pass an array or tuple in a function, it'll be passed in as a reference.
    - Therefore, it'll change the original array.
- This is where `copy()` and `deepcopy()` functions come in.


```py
>>> import copy
>>> spam = ['A', 'B', 'C', 'D']
>>> cheese = copy.copy(spam)
>>> cheese[1] = 42
>>> spam
['A', 'B', 'C', 'D']
>>> cheese
['A', 42, 'C', 'D']
```
- Deepcopy works similarly, but recursively copies the contents inside (for example, the items in the list are arrays too!)