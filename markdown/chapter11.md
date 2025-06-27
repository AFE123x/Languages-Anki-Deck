# 11. Web Scraping

## Opening a webpage with `webbrowser` module

- you can open a tab to a webpage using `webbrowser.open`

```py
import webbrowser

webbrowser.open('https://www.google.com/')
```
- This will open a tab to google chrome.

## Downloading files from webpage with `requests` module

- You can download a webpage from the web using the `requests` module

```py
import requests

res = requests.get('http://www.gutenberg.org/cache/epub/1112/pg1112.txt') # will return a response object

assert res.status_code == requests.code.ok, "request failed"

print(res.text[:250]) # will print first 250 characters of webpage text.
```

## Beautiful Soup, what is it?

- Parsing html with regex would be very stupid, just use beautiful soup.

- BeatifulSoup isn't a built in module, so you need to install it, using pip, brew or whatever you have.

## BeautifulSoup - creating object

- To make a BeautifulSoup object, you need html to parse
- You can use the `requests` module for this

```py
import requests, bs4

res = requests.get('http://nostarch.com') # gets response
res.raise_for_status() # Checks if requests failed
noStarchSoup = bs4.BeautifulSoup(res.text) # contsructs beautiful soup class
```

## BeautifulSoup - Finding element, reading content

- To find the elements, you'd use the `select()` function

```py
import requests, bs4

res = requests.get('http://nostarch.com') # gets response
res.raise_for_status() # Checks if requests failed
noStarchSoup = bs4.BeautifulSoup(res.text) # contsructs beautiful soup class

print(noStarchSoup.select('div')) # will print array of div tags.
print(noStartSoup.select('#author')) # will print array of elements with the id attribtue of author.

mything = noStartSoup.select('#author')[0] # will get first element with author id

print(mything.getText()) # will get text nested in tag with author id
print(mything.get('id')) # print author
```

## Controlling browser with selenium

- The `selenium` module lets you control a selenium-controlled browser, like firefox.

## Selenium - Opening webpage

```py
from selenium import webdriver
browser = webdriver.firefox() # opens a firefox window
browser.get('http://inventwithpython.com') # will direct browser to inventwithpython.com
```

## Selenium - Finding elements on page

- there are many methods for this.
- they all start with `.find_element_by_*` or `.find_elements_by_*`
    - for example, `.find_element_by_link_text(text)` to find an <a> element containing the text provided
- The method will return a webelement object, which represents the element
```py
from selenium import webdriver
browser = webdriver.firefox() # opens a firefox window
browser.get('http://inventwithpython.com') # will direct browser to inventwithpython.com

webattribute = browser.find_element_by_link_text("Click Here") # will find a tag containing "Click Here"
```

## Selenium - WebElement methods

- WebElements are the object representation of the web elements.

| attribute/method | description |
| --- | --- |
| tag_name | tag name |
| get_attribute(name) | value for the element's "name" attribute
| text | text within element |
| clear() | clears text in textfild or text area |

- There's a lot more

## Selenium - clicking page

- You can simulate a mouse click using `.click()` method on webelem object

```py
>>> from selenium import webdriver
>>> browser = webdriver.Firefox()
>>> browser.get('http://inventwithpython.com')
>>> linkElem = browser.find_element_by_link_text('Read It Online')
>>> type(linkElem)
<class 'selenium.webdriver.remote.webelement.WebElement'>
>>> linkElem.click() # follows the "Read It Online" link
```

## Selenium, filling and submitting form, special keys

- You can send key strokes to a text field using `send_keys()` method on a webelement object

```py
>>> from selenium import webdriver
>>> browser = webdriver.Firefox()
>>> browser.get('http://gmail.com')
>>> emailElem = browser.find_element_by_id('Email')
>>> emailElem.send_keys('not_my_real_email@gmail.com')
>>> passwordElem = browser.find_element_by_id('Passwd')
>>> passwordElem.send_keys('12345')
>>> passwordElem.submit()
```

- You can also send special keys, like the arrow, enter, etc.

```py
>>> from selenium import webdriver
>>> from selenium.webdriver.common.keys import Keys
>>> browser = webdriver.Firefox()
>>> browser.get('http://nostarch.com')
>>> htmlElem = browser.find_element_by_tag_name('html')
>>> htmlElem.send_keys(Keys.END) # scrolls to bottom
>>> htmlElem.send_keys(Keys.HOME) # scrolls to top
```

## Selenium - Clicking Browser BUttons

- You can also, with the browser object, press the back, forward, refresh and quit buttons

```py
>>> from selenium import webdriver
>>> from selenium.webdriver.common.keys import Keys
>>> browser = webdriver.Firefox()
>>> browser.get('http://nostarh.com')
>>> browser.back() # will go back a page.
```
