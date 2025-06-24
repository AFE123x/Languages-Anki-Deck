# Pattern Matching with Regular Expressions

## What is Regelar Expression?

- ReGex (Regular Expression) is a tool.
- It describes a pattern of text.

```cs
\d\d\d-\d\d\d-\d\d\d\d
```
- This is an example of regex we can use to find phone numbers.

## `\d`

- `\d` stands for a digital character, or any number from 0 to 9.

## regex - curly brackets

- instead of writing multiple `\d\d\d` three times, we can use `\d{3}`, which means the same thing.
- You can also have a second number, seperated by `,` to describe a bounds.
    - `\d{3,}` will get texts with 3 or more numbers
    - `\d{,3}` will have up to 3 digits
    - `\d{3,5}` will have beween 3 to 5 digits.

## Creating regex object, finding patterns

- You can create a regex object by import the `re` module.
- to create a regex object, you use `re.compile(regex expression)`
- To find the pattern in the text, you use the `search()` method with the regex object.
```py
phoneregex = re.compile(r'\d\d\d-\d\d\d-\d\d\d\d') # stores pattern to look for
mo = phoneregex.search('my number is 415-555-4242') # finds the text based on pattern
print('found ' + mo.group()) # .group returns a string of matched text.
# output: found 415-555-4242
```

- You can also use findall to find all instances of text that match the pattern:

```py
>>> phoneNumRegex = re.compile(r'\d\d\d-\d\d\d-\d\d\d\d') # has no groups
>>> phoneNumRegex.findall('Cell: 415-555-9999 Work: 212-555-0000')
['415-555-9999', '212-555-0000']
```

## regex - grouping with parentheses

- If you want, you can separate a piece of the text from the rest by using groups.
- This is done with parenthesis

```py
>>> phoneNumRegex = re.compile(r'(\d\d\d)-(\d\d\d-\d\d\d\d)')
>>> mo = phoneNumRegex.search('My number is 415-555-4242.')
>>> mo.group(1)
'415'
>>> mo.group(2)
'555-4242'
>>> mo.group(0)
'415-555-4242'
>>> mo.group()
'415-555-4242'
>>> mo.groups() # return all groups in a tuple
('415', '555-4242')
>>> areaCode, mainNumber = mo.groups()
>>> print(areaCode)
415
>>> print(mainNumber)
555-4242
```

## Matching multiple groups with pipe

- Let's say you want to look for text that can match more than one expression.
- You can use the `|` character

```py
>>> heroRegex = re.compile (r'Batman|Tina Fey')
>>> mo1 = heroRegex.search('Batman and Tina Fey.')
>>> mo1.group()
'Batman'
>>> mo2 = heroRegex.search('Tina Fey and Batman.')
>>> mo2.group()
'Tina Fey'
```

- You can also use this to find several patterns as part of your regex:

```py
>>> batRegex = re.compile(r'Bat(man|mobile|copter|bat)')
>>> mo = batRegex.search('Batmobile lost a wheel')
>>> mo.group()
'Batmobile'
>>> mo.group(1)
'mobile'
```
- this will find text that starts with Bat, and ends with either man, mobile, copter, or bat.

## Optional Matching with `?`

- You can use the `?` to make certain patterns optionaly.

```py
>>> batRegex = re.compile(r'Bat(wo)?man')
>>> mo1 = batRegex.search('The Adventures of Batman')
>>> mo1.group()
'Batman'
>>> mo2 = batRegex.search('The Adventures of Batwoman')
>>> mo2.group()
'Batwoman'
```

## Matching Zero or More with the star

- You can use the `*` symbol to "match zero or more"

```py
>>> batRegex = re.compile(r'Bat(wo)*man') # we can have as many wo in our text
>>> mo1 = batRegex.search('The Adventures of Batman')
>>> mo1.group()
'Batman' # no wo in our text.
>>> mo2 = batRegex.search('The Adventures of Batwoman')
>>> mo2.group()
'Batwoman'
>>> mo3 = batRegex.search('The Adventures of Batwowowowoman')
>>> mo3.group()
'Batwowowowoman'
```

## Matching One or More with Plus

- You can use the `+` symbol to match one or more. It's similar te `*`, but you need the pattern to exist to work.


## Regex - Greedy vs non greedy

- Greedy means that the regex will find the longest string matching the pattern.
- Nongreedy will ƒind the shortest string matching the pattern.
    - To make a regex nongreedy, you add a `?` at the end of yoru expression.

## regex - Chracter Classes

- There are multiple character classes to use:

| Shorthand character class | represents |
| --- | --- |
| `\d` | any digit from 0 to 9 |
| `\D` | any character thas isn't a numeric digital from 0 to 9 |
| `\w` | any letter, numeric digit or underscore character |
| `\W` | any character that isn't a letter, numeric digit or underscore character |
| `\s` | any space, tab or newline character |
| `\S` | any character that isn't a space, tab or newline character |

## regex - carret and dollar sign characters 

- We can add a `^` symbol at the beginning to indicate a match must occur at the beginning of the searched text.
- We can add a `$` symbol at the end indicating the match must occur at the end of the text.

## Wildcard character

- You can use the `.` charact to match match any character to it minus the newline character.

```py
>>> atRegex = re.compile(r'.at')
>>> atRegex.findall('The cat in the hat sat on the flat mat.')
['cat', 'hat', 'sat', 'lat', 'mat']
```