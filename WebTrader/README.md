WebTrader is an example web page based application for trading
futures based options.

Emweb's Wt (webtoolkit) is used for building the web server portion
of the application.

This requires that libs-buid/build.sh be used to install wt.

```
$ cat x64/debug/AppTableTrader/etc/choices.cfg
ib_client_id=9
ib_client_port=7496
ui_username=login
ui_password=pass
candidate_future=@ESU22
candidate_future=@NQU22
```

