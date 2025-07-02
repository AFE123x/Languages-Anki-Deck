# 16. Sending Email and Text Messages

## SMTP

- SMTP is the **Simple Mail Transfer Protocol**
- This defines how email messages should be formatted, encrypted and relayed between mail servers.
- python's smtplib module simplifies this protocol in just a couple functions.

- SMTP handles sending emails to others, IMAP is a different protocol that deals with retrieving emails sent to you.

## Sending email, basic structure

```py
>>> import smtplib
>>> smtpObj = smtplib.SMTP('smtp.example.com', 587) # connects to smtp server
>>> smtpObj.ehlo() # does the first step greet to SMTP server
(250, b'mx.example.com at your service, [216.172.148.131]\nSIZE 35882577\
n8BITMIME\nSTARTTLS\nENHANCEDSTATUSCODES\nCHUNKING')
>>> smtpObj.starttls() # if you're using port 587 (SMTP with TLS encryption), you use this method
(220, b'2.0.0 Ready to start TLS')
>>> smtpObj.login('bob@example.com', 'MY_SECRET_PASSWORD') # login
(235, b'2.7.0 Accepted')
>>> smtpObj.sendmail('bob@example.com', 'alice@example.com', 'Subject: So long.\nDear Alice, so long and thanks for all the fish. Sincerely, Bob') # send email
{}
>>> smtpObj.quit() # close connection
(221, b'2.0.0 closing connection ko10sm23097611pbd.52 - gsmtp')
```

## Sending email - Connecting to SMTP server

- You can connect to the SMTP server via `smtplib.SMTP('server', portnum)`.
- you can get the email server and port number with a google search.

```py
>>> import smtplib
>>> smtpObj = smtplib.SMTP('smtp.gmail.com', 587) # connects to smtp server
```
- This is the server you'd connect to for gmail, for example.


## Sending email, SMTP "Hello" Message

- Once you make the initial server connection with `smtplib.SMTP('smtp.gmail.com', 587)`, you need to call the `ehlo()` method to basically do a "handshake" with the server

```py
>>> import smtplib
>>> smtpObj = smtplib.SMTP('smtp.gmail.com', 587) # connects to smtp server
>>> smtpObj.ehlo() # does the first step greet to SMTP server
(250, b'mx.example.com at your service, [216.172.148.131]\nSIZE 35882577\
n8BITMIME\nSTARTTLS\nENHANCEDSTATUSCODES\nCHUNKING')
```

## Sending email - starting TLS encryption

- Once you connect and establish the connection with the SMTP server, you need to start the TLS encryption (if the port num is 587).
- you can do this with the `.starttls()` method

```py
>>> import smtplib
>>> smtpObj = smtplib.SMTP('smtp.gmail.com', 587) # connects to smtp server
>>> smtpObj.ehlo() # does the first step greet to SMTP server
(250, b'mx.example.com at your service, [216.172.148.131]\nSIZE 35882577\
n8BITMIME\nSTARTTLS\nENHANCEDSTATUSCODES\nCHUNKING')
>>> smtpObj.starttls()
(220, b'2.0.0 Ready to start TLS')
```

## Sending Email - logging into SMTP server

- Once you setup the tls encryption, you need to login
- This is done with the `login()` method

```py
>>> import smtplib
>>> smtpObj = smtplib.SMTP('smtp.gmail.com', 587) # connects to smtp server
>>> smtpObj.ehlo() # does the first step greet to SMTP server
(250, b'mx.example.com at your service, [216.172.148.131]\nSIZE 35882577\
n8BITMIME\nSTARTTLS\nENHANCEDSTATUSCODES\nCHUNKING')
>>> smtpObj.starttls()
(220, b'2.0.0 Ready to start TLS')
>>> smtpObj.login('my_email_address@gmail.com', 'MY_SECRET_PASSWORD')
(235, b'2.7.0 Accepted')
```

- For gmail, you need to setup an application specific password.
- If you use your normal password, it won't work.

## Sending email, sending the email

- After setting the smtp server up, starting tls encryption and logging in, you can send an email
- This is done with `.sendmail()`

```py
>>> import smtplib
>>> smtpObj = smtplib.SMTP('smtp.gmail.com', 587) # connects to smtp server
>>> smtpObj.ehlo() # does the first step greet to SMTP server
(250, b'mx.example.com at your service, [216.172.148.131]\nSIZE 35882577\
n8BITMIME\nSTARTTLS\nENHANCEDSTATUSCODES\nCHUNKING')
>>> smtpObj.starttls()
(220, b'2.0.0 Ready to start TLS')
>>> smtpObj.login('my_email_address@gmail.com', 'MY_SECRET_PASSWORD')
(235, b'2.7.0 Accepted')


>>> smtpObj.sendmail('my_email_address@gmail.com', 'recipient@example.com', 'Subject: So long.\nDear Alice, so long and thanks for all the fish. Sincerely, Bob')
```

- Here, the subject is stored as `Subject: subject\n`, where you need the new line. The rest of the email is the main body.

## Sending email, closing server

- Once you send your mail, you close the smtp server with `.quit()`

```py
>>> import smtplib
>>> smtpObj = smtplib.SMTP('smtp.gmail.com', 587) # connects to smtp server
>>> smtpObj.ehlo() # does the first step greet to SMTP server
(250, b'mx.example.com at your service, [216.172.148.131]\nSIZE 35882577\
n8BITMIME\nSTARTTLS\nENHANCEDSTATUSCODES\nCHUNKING')
>>> smtpObj.starttls()
(220, b'2.0.0 Ready to start TLS')
>>> smtpObj.login('my_email_address@gmail.com', 'MY_SECRET_PASSWORD')
(235, b'2.7.0 Accepted')
>>> smtpObj.sendmail('my_email_address@gmail.com', 'recipient@example.com', 'Subject: So long.\nDear Alice, so long and thanks for all the fish. Sincerely, Bob')

>>> smtpObj.quit()
(221, b'2.0.0 closing connection ko10sm23097611pbd.52 - gsmtp')
```

## IMAP

- SMTP handles the sending of emails, IMAP handles the receiving
- IMAP stands for **Internet Message Access Protocol**.
- This standard defines how to communicate with an email provider server to retrieve emails sent to your email address.

- python has a built in `imaplib` module, but the third party `imapclient` module is easier to use.

- `imapclient` downloads emails in a complicated format, so you'll need to convert it into a string value.
- This is easily done with the `pyzmail` module, which does the conversion.

## `imapclient`, overview

```py
>>> import imapclient
>>> imapObj = imapclient.IMAPClient('imap.gmail.com', ssl=True)
>>> imapObj.login('my_email_address@gmail.com', 'MY_SECRET_PASSWORD')
'my_email_address@gmail.com Jane Doe authenticated (Success)'
>>> imapObj.select_folder('INBOX', readonly=True)
>>> UIDs = imapObj.search(['SINCE 05-Jul-2014'])
>>> UIDs
[40032, 40033, 40034, 40035, 40036, 40037, 40038, 40039, 40040, 40041]
>>> rawMessages = imapObj.fetch([40041], ['BODY[]', 'FLAGS'])
>>> import pyzmail
>>> message = pyzmail.PyzMessage.factory(rawMessages[40041]['BODY[]'])
>>> message.get_subject()
'Hello!'
>>> message.get_addresses('from')
[('Edward Snowden', 'esnowden@nsa.gov')]
>>> message.get_addresses('to')
[(Jane Doe', 'jdoe@example.com')]
>>> message.get_addresses('cc')
[]
>>> message.get_addresses('bcc')
[]
>>> message.text_part != None
True
>>> message.text_part.get_payload().decode(message.text_part.charset)
'Follow the money.\r\n\r\n-Ed\r\n'
>>> message.html_part != None
True
>>> message.html_part.get_payload().decode(message.html_part.charset)
'<div dir="ltr"><div>So long, and thanks for all the fish!<br><br></div>-
Al<br></div>\r\n'
>>> imapObj.logout()
```

- You first start up IMAP server, login, search/fetch and extract the emails, then log out.

## IMAP - connect to IMAP server

```py
>>> import imapclient
>>> imapObj = imapclient.IMAPClient('imap.gmail.com', ssl=True)
```
- To connect to an imap server, you create an imap object using `imapclient.IMAPClient()` function.

- The server name depends on provider. gmail's server name is `imap.gmail.com`
- you usually need to set `ssl` encryption to true.

## IMAP - Logging in

- Once you start up the server, you can call the `login` method, passing in the username.

```py
>>> import imapclient
>>> imapObj = imapclient.IMAPClient('imap.gmail.com', ssl=True)
>>> imapObj.login('emailaddress@gmail.com', 'my password') # will log you in.
```

- For gmail, you may need to use an application specific password.

## IMAP - searching for email

- getting an email consist of two steps:
    - selecting a folder
    - performing the search


## IMAP - searching for email: selecting a folder

- to select a folder, you can list them all out using `list_folders()` method

```py
>>> import pprint
>>> import imapclient
>>> imapObj = imapclient.IMAPClient('imap.gmail.com', ssl=True)
>>> imapObj.login('emailaddress@gmail.com', 'my password') # will log you in.
>>> pprint.pprint(imapObj.list_folders())
[(('\\HasNoChildren',), '/', 'Drafts'),
(('\\HasNoChildren',), '/', 'Filler'),
(('\\HasNoChildren',), '/', 'INBOX'),
(('\\HasNoChildren',), '/', 'Sent'),
--snip--
(('\\HasNoChildren', '\\Flagged'), '/', '[Gmail]/Starred'),
(('\\HasNoChildren', '\\Trash'), '/', '[Gmail]/Trash')]
```

- The folders function returns an array of tuples consisting of:
    - folder flags
    - delimiter used in name string to separate parent folders and subfolders
    - full name of folder.

- To select a folder, you need to use the `select_folder()` function

```py
imapObj.select_folder('INBOX', readonly=True)
```

## IMAP - searching for email: performing search

- You can search for emails using the `search method`

```py
#initialization done
imapObj.select_folder('INBOX', readonly=True)
imapObj.search('ALL') will return all messages
```

- There are many search criteria you can use, don't need to memorize everything:

| search key | meaning |
| --- | --- |
| 'ALL' | returns all messages in folder |
| 'BEFORE date' | returns mesasges before a certain date: `BEFORE 05-Jul-2015` |

- THe search function doesn't return the email contents, rather, a unique UID.

## IMAP - fetching email

- Once you get the UID, you can fetch the email itself.

```py
#initialization done
imapObj.select_folder('INBOX', readonly=True)
UIDs = imapObj.search('ALL') will return all messages
>>> rawMessages = imapObj.fetch(UIDs, ['BODY[]'])
>>> import pprint
>>> pprint.pprint(rawMessages)
{40040: {'BODY[]': 'Delivered-To: my_email_address@gmail.com\r\n'
'Received: by 10.76.71.167 with SMTP id '
--snip--
'\r\n'
'------=_Part_6000970_707736290.1404819487066--\r\n',
'SEQ': 5430}}
```

- The first argument is the UID list
- The second argument tells fetch to download all the body content for the emails specified in UID list.

## Using pyzmail to get data from message

- Once you get the raw email from the `fetch()` method, you can use pyzmail to parse it:

```py
#initialization done
imapObj.select_folder('INBOX', readonly=True)
UIDs = imapObj.search('ALL') will return all messages
>>> rawMessages = imapObj.fetch(UIDs, ['BODY[]'])
>>> import pyzmail
>>> message = pyzmail.PyzMessage.factory(rawMessages[UID]['BODY[]'])
>>> message.get_subject()
'hello'
>>> message.text_part_get_payload().decode(messag.text_part.charset)
'So long, and thanks for all the fish!\r\n\r\n-Al\r\n'
```
- This will make it easier to access elements.

## IMAP - deleting emails

```py
>>> imapObj.select_folder('INBOX', readonly=False)
v >>> UIDs = imapObj.search(['ON 09-Jul-2015'])
>>> UIDs
[40066]
>>> imapObj.delete_messages(UIDs)
w {40066: ('\\Seen', '\\Deleted')}
>>> imapObj.expunge()
('Success', [(5452, 'EXISTS')])
```
- `delete_messages` will mark emails with the uids with a delete flag
- `expunge()` will delete all emails containing the delet flag.

## IMAP - disconnecting from server

```py
# iniliazation.
imapObj.logout()
```
