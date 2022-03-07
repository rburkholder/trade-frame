## AutoTrade 

This sample project is composed of current best practicies for building an
automated trading application.  Some of the features and functions represented:

* parameters supplied by a configuration file
* live chart being update with quotes, ticks, volume, and trade results
* state machine to process entries and exits
* portfolios, positions, instruments, orders and executions are recorded in a sqlite3 database
* live data is sourced from IQFeed
* orders are executed at Interactive Brokers (run this as a paper trading example)
* trading is automatically limited to North America trading hours
* quotes and ticks can be manually saved to an HDF5 based timeseries database at end of session

This project can be used as a template to try out your own high-frequency trading ideas.

Quotes are used to supply the trading signals in a high-frequency fashion.

### x64/debug/AutoTrade.cfg

This example uses a three time period exponential moving average to generate the entry/exit signals.  
The parameters are sourced in the configuration file (as an example):

x64/debug/AutoTrade.cfg:

```
symbol=SPY
#symbol=@ES~
period_width=5 # seconds
ma1_periods=5
ma2_periods=8
ma3_periods=12
group_directory=/app/AutoTrade/20220218 19:23:05.489488
sim_start=off
```

When there is no data available, set the group directory such nothing follows the '='

### x64/debug/AutoTrade.db

The database has a number of tables, and can be accessed in a manner similar to this example:

```
$ sqlite3 x64/debug/AutoTrade.db
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

NOTE: I have to test this yet

* collect data during a live session, and at the end of the session, use the Save Values menu item
* restart the application, and set D1 & X to Sim, and turn on
* a Simulation menu will appear
* Simulation->Run to start the simulation
  * trades should show up in similar time frames

