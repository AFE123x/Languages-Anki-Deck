# 5. Dictionaries and Structuring Data

## What is the dictionary type

- The dictionary is a collection of many values.
- Unlike a list, you can have multiple data types.

- You index a dictionary with a key, and the key is associated with a value, also known as a key-value pair.

```py
>>> myCat = {'size': 'fat', 'color': 'gray', 'disposition': 'loud'}
>>> myCat['size']
'fat'
>>> myCat['color']
'gray'
```

## Dictionaries vs. lists

- Unlike lists, items in dictionaries are unordered.
    - There's a index 0 for lists.
    - There's no index 0 for dictionaries.

- The ordering doesn't matter in a dictionary.

```py
>>> spam = ['cats', 'dogs', 'moose']
>>> bacon = ['dogs', 'moose', 'cats']
>>> spam == bacon
False
>>> eggs = {'name': 'Zophie', 'species': 'cat', 'age': '8'}
>>> ham = {'species': 'cat', 'age': '8', 'name': 'Zophie'}
>>> eggs == ham
True
```
- You can see here, eggs and ham are the same, despite having a different order.

## dictionary: `keys()`, `values()`, `items()` methods

- `values()` will iterate through all values in the dictionary

```py
>>> spam = {'color': 'red', 'age': 42}
>>> for v in spam.values():
print(v)
red
42
```

- `keys()` will iterate through all keys in the dictionary
- `items()` will iterate through a tuple of the keys and values.
```py
>>> spam = {'color': 'red', 'age': 42}
>>> for k in spam.keys():
print(k)
color
age
>>> for i in spam.items():
print(i)
('color', 'red')
('age', 42)
```

## Dictionary - `get()` method

- The get() method is a safer version of `dict[key]`.

```py
>>> picnicItems = {'apples': 5, 'cups': 2}
>>> 'I am bringing ' + str(picnicItems.get('cups', 0)) + ' cups.'
'I am bringing 2 cups.'
>>> 'I am bringing ' + str(picnicItems.get('eggs', 0)) + ' eggs.'
'I am bringing 0 eggs.'
```

## Dictionary - `setdefault()` method

- This method will, if the key isn't  in the dictionary, it adds the key with it's value

```py
>>> spam = {'name': 'Pooka', 'age': 5}
>>> spam.setdefault('color', 'black')
'black'
>>> spam
{'color': 'black', 'age': 5, 'name': 'Pooka'}
>>> spam.setdefault('color', 'white')
'black'
>>> spam
{'color': 'black', 'age': 5, 'name': 'Pooka'}
```


