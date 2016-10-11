# git-email with foxmail

1. sudo apt-get install git-email
2. config ~/.gitconfig like this

```
[user]
        name = Chen Feng
        email = puck.chen@foxmail.com
[core]
        editor = vim 
[sendemail]
        smtpencryption = ssl 
        smtpuser = puck.chen@foxmail.com
        smtpserverport = 465 
        smtpserver = smtp.qq.com
        smtppass = xxxxxxxxxxxxxxxxxxxxxx 
```
