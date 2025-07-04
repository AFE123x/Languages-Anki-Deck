import pyautogui
import keyboard
import time

pyautogui.PAUSE = 0

while True:
    if not keyboard.is_pressed('command'):  # on macOS, 'command' refers to ⌘
        pyautogui.keyDown('s')
        pyautogui.keyUp('s')
        time.sleep(0.01)  # small delay to prevent CPU overuse
