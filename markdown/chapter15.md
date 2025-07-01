# 15. Keeping Time, Scheduling Tasks, and Launching Programs

## Time module

- you can use python's `time` module to use the system clock to get the current time.

```py
>>> import time
>>> time.time()
1751301604.436296 # describes how much time passed from "Unix epoch"
```

- `Unix epoch` is January 1st, 1970 Coordinated Universal Time.

```py
>> import time
>> for i in range(3):
        print('Tick')
        time.sleep(1) # will put thread to sleep for 1 second.
        print('Tock')
        time.sleep(1)
Tick
Tock
Tick
Tock
Tick
Tock
>>> time.sleep(5)
```

## Rounding numbers

- You can round numbers using `round()`

```py
>>> import time
>>> now = time.time()
>>> now
1425064108.017826
>>> round(now, 2)
1425064108.02
>>> round(now, 4)
1425064108.0178
>>> round(now)
1425064108
```

## The datetime module

- The datetime module, which is more useful than getting the raw `Unix epoch` timestamp.

```py
>>> import datetime
>>> dt = datetime.datetime.now() # datetime.datetime(2015, 2, 27, 11, 10, 49, 55, 53)

>> dt.year, dt.month, dt.day
(2015, 10, 21)
>> dt.hour, dt.minute, dt.second
(16,29,0)

# getting time from epoch timestamps
>>> datetime.datetime.fromtimestamp(1000000)
datetime.datetime(1970, 1, 12, 5, 46, 40) # 1000000 seconds from unix epoch
>>> datetime.datetime.fromtimestamp(time.time())
datetime.datetime(2015, 2, 27, 11, 13, 0, 604980)
```

- you can also get the dates with the time from the Unix Epoch

```py
holloween2015 = datetime.datetime(2015,10,31,0,0,0)
newyears2016 = datetime.datetime(2016,1,1,0,0,0)
print(halloween2015 == oct31_2015) # prints True 
print(newyears2016 > halloween2015) # prints True
```

## Timedelta type

- `timedelta` represents a duration of time

```py
>>> delta = datetime.timedelta(days=11, hours=10, minutes=9, seconds=8)
>>> delta.days, delta.seconds, delta.microseconds
(11, 36548, 0)
>>> delta.total_seconds()
986948.0
>>> str(delta)
'11 days, 10:09:08'
```

You can also add the `timedelta` type to the `datetime` type.

```py
>>> dt = datetime.datetime.now()
>>> dt
datetime.datetime(2015, 2, 27, 18, 38, 50, 636181)
>>> thousandDays = datetime.timedelta(days=1000)
>>> dt + thousandDays
datetime.datetime(2017, 11, 23, 18, 38, 50, 636181)
```

## Converting `datetime` objects into strings, vice versa

- You can convert datetime objects into strings using `strftime()`

```py
>>> oct21st = datetime.datetime(2015, 10, 21, 16, 29, 0)
>>> oct21st.strftime('%Y/%m/%d %H:%M:%S')
'2015/10/21 16:29:00'
>>> oct21st.strftime('%I:%M %p')
'04:29 PM'
>>> oct21st.strftime("%B of '%y")
"October of '15"
```

- You can convert a string into a date with `strptime()`

```py
>>> datetime.datetime.strptime('October 21, 2015', '%B %d, %Y')
datetime.datetime(2015, 10, 21, 0, 0)
>>> datetime.datetime.strptime('2015/10/21 16:29:00', '%Y/%m/%d %H:%M:%S')
datetime.datetime(2015, 10, 21, 16, 29)
>>> datetime.datetime.strptime("October of '15", "%B of '%y")
datetime.datetime(2015, 10, 1, 0, 0)
>>> datetime.datetime.strptime("November of '63", "%B of '%y")
datetime.datetime(2063, 11, 1, 0, 0)
```

## Python Multithreading

```py
import threading

def threadboi():
    print('i\'m a therad!!!')

threadobj = threading.Thread(target=threadboi)
threadobj.start()

threadobj.join()
print('byebye')
```