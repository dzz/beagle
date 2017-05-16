# Debugging 

## Soft Reset

At any point you can restart the scripting environment without having to stop and restart the runtime. The shortcut for this is ``CTRL+F5``

## Telnet Console

If you would like to print or tweak values while your application is running, you can enabled the telnet console.

```
; in application.ini

[APPLICATION]

; sample settings

telnet_enabled=true
telnet_host=127.0.0.1
telnet_port=8888
```

Now you can telnet into your application and run commands using a tool such as PuTTY. The module in which you specified ``init``, ``tick``, ``render`` etc. is inside of the variable ``app``.

```
# sample session (using simple_game):

CTT2 - Debug Server 0.1
--------------------
print(app.Game.player.x)
16.0

app.Game.player.x=99

```


