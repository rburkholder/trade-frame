## AutoTrade - futures l2

NOTE: turn off the feeds prior to exit or saving.  This prevents updating the structures during save and exit.
NOTE: this application requires a subscription to Futures LII data via IQFeed

This sample project is composed of current best practicies for building an
automated trading application.  Some of the features and functions represented:

* parameters supplied by a configuration file
* live chart being update with quotes, ticks, volume, and trade results
* market depth is available for CME/ICE futures
* state machine to process entries and exits
* portfolios, positions, instruments, orders and executions are recorded in a sqlite3 database
* live L1/L2 data is sourced from IQFeed
* orders are executed at Interactive Brokers (run this as a paper trading example)

This project can be used as a template to try out your own high-frequency trading ideas.

### x64/debug/rdaf/l2/choices.cfg

This is a specialization of the generic AutoTrade project.  This project uses CERN's rdaf (ROOT Data Analysis Framework)
libraries for analysing live data as it arrives.

The parameters are sourced in the configuration file (as an example): 

$ cat x64/debug/rdaf/l2/app.cfg
```
telegram_token=6216......
ib_client_id=7
threads=3
sim_start=no
group_directory=/app/collector/20221220-09:20:13.187534
time_bins=3600
time_upper=20221220T220000
time_lower=20221219T220000
[@ESH23]
feed=<l1|l2m|l2o>
sym_symbol=ES-20230317
trade=yes
emit_fvs=yes
algorithm=<future|equity_option>
period_width=3
ma1_periods=5
ma2_periods=13
ma3_periods=31
stochastic1_periods=13
stochastic2_periods=47
stochastic3_periods=87
price_bins=1000
price_upper=5000
price_lower=3000
volume_bins=100
volume_upper=500000
volume_lower=0
[SPY]
feed=l1
trade=yes
algorithm=equity_option
signal_from=@ESH23
period_width=3
ma1_periods=5
ma2_periods=13
ma3_periods=31
stochastic1_periods=13
stochastic2_periods=47
stochastic3_periods=87
price_bins=1000
price_upper=600
price_lower=200
volume_bins=100
volume_upper=500000
volume_lower=0
```
* group_directory is optional if sim_start is off.

### x64/debug/rdaf/l2/app.db

The database has a number of tables, and can be accessed in a manner similar to this example:

```
$ sqlite3 x64/debug/rdaf/l2/app.db
SQLite version 3.34.1 2021-01-20 14:10:07
Enter ".help" for usage hints.
sqlite> select * from portfolios;
Master|aoTF||1|1|USD|Master of all Portfolios|0.0|0.0
USD|aoTF|Master|2|1|USD|CurrencySummary of USD Portfolios|0.0|0.0
sqlite> select * from positions;
1|USD|SPY||ib01|iq01|SPY|ema|0|0|0|0|0.0|0.0|0.0|0.0
sqlite> .quit
```

For testing purposes, delete or rename the database to start fresh.

### Simulation

* change x64/debug/rdaf/at/choices.cfg to have sim_start=on
* restart the application - simulation mode is auto-started
* trades should show up in similar time frames

### Telegram Messaging

* [BotFather Token](https://core.telegram.org/bots/tutorial) - create bot, obtain token
* populate the telegram_token in choices.cfg
* start application
* from your regular telegram account, send some text to the bot to register chat id
* use menu SendMessage to send a test message to confirm chat id
* chat id will persist in the state file for next startup
* in strategy file, to send message: m_fTelegram( "message text" );

