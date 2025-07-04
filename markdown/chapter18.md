# 18. Controlling the Keyboard and Mouse with GUI Automation

## `pyautogui` module

- This sends virtual keypresses and mouse clicks to your computer

- because you're automating mouseclicks, you'll have to consider potential failures.

- There are two variables you can set:

```py
>>> import pyautogui
>>> pyautogui.PAUSE = 1 # how long to pause (seconsd)
>>> pyautogui.FAILSAFE = True # enable failsafe feature
```

## Controlling Mouse movement

- pyautogui represents mouse with x y coordinates, where the width and height is the resolution of the screen.
    - you can get the width and height with `pyautogui.size()`
- to get the mouse position, you can call `pyautogui.position()`
- to move the mouse, you can either use `pyautogui.moveTo()` or `pyautogui.moveRel` depending on whether you want to move absolutely, or relatively. It takes three arguments:
    - x axis
    - y axis
    - optionally, duration.

## Controlling mouse click

- you can click the mouse with `.click()`.
- if you want to press mouse down, you can do `.mouseDown()`
- if you want to press mouse up, you can do `.mouseUp()`
- you can do a double click with `.doubleclick()`
- you can do left, right, middle with `.leftClick()`, `.rightClick()`, `.middleClick()`


- arguments: x and y coordinate

## Dragging mouse

- you can drag the mouse using `dragTo()` and `dragRel()`

## Scrolling mouse

- you can use `.scroll()`, where you pass in how many units you want to scroll up/down.

## Getting screenshot.

- You can take screen shots with `pyautogui.screenshot()`, which will return a matrix of RGB pixels (the screenshot)

```py
import pyautogui
im = pyautogui.screenshot()
print(im.getpixel((0,0))) #print (R,G,B)
print(pyautogui.pixelMatchesColor(50, 200, (130, 135, 144))) # prints true or false
```

## Image Recognition

- you can use image recognition to figure out where a button, for example, is on the screen

```py
>>> import pyautogui
>>> pyautogui.locateOnScreen('submit.png')
(643, 745, 70, 29) # (Width, height, width, height)
>>> list(pyautogui.locateAllOnScreen('submit.png'))
[(643, 745, 70, 29), (1007, 801, 70, 29)] # list of all items
```

- if the thing isn't found, it'll return none

## Controlling Keys


- to send a string, you can do `pyautogui.typewrite()`
    - you can pass in a string, or an array of strings.


```py
>>> pyautogui.typewrite('Hello world!')
>>> pyautogui.typewrite(['a', 'b', 'left', 'left', 'X', 'Y'])
```

## Pressing and releasing key

- you press key with `.press()`
- you press key down with `.keyDown()`
- you release key with `.keyUp()`