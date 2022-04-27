## AutoTrade

NOTE: turn off the feeds prior to exit or saving.  This prevents updating the structures during save and exit.
NOTE: this application requires a subscription to Nasdaq LII data via IQFeed

This sample project is composed of current best practicies for building an
automated trading application.  Some of the features and functions represented:

* parameters supplied by a configuration file
* live chart being update with quotes, ticks, volume, and trade results
* market depth is being fed as well
* state machine to process entries and exits
* portfolios, positions, instruments, orders and executions are recorded in a sqlite3 database
* live L1/L2 data is sourced from IQFeed
* orders are executed at Interactive Brokers (run this as a paper trading example)
* trading is automatically limited to North America trading hours
* quotes, ticks and marketdepth can be manually saved to an HDF5 based timeseries database at end of session

This project can be used as a template to try out your own high-frequency trading ideas.

### x64/debug/rdaf/at/choices.cfg

This is a specialization of the generic AutoTrade project.  This project uses CERN's rdaf (ROOT Data Analysis Framework)
libraries for analysing live data as it arrives.

The parameters are sourced in the configuration file (as an example):

$ cat x64/debug/rdaf/at/choices.cfg
```
ib_client_id=5
threads=2
sim_start=off
group_directory=/app/rdaf/at/20220311-18:33:33.786804-1
time_bins=3600
time_upper=20220316T000000
time_lower=20220315T000000
[SPY]
price_bins=200
price_upper=500
price_lower=300
volume_bins=100
volume_upper=10000
volume_lower=0
```
group_directory is optional if sim_start is off.

### x64/debug/rdaf/at/example.db

The database has a number of tables, and can be accessed in a manner similar to this example:

```
$ sqlite3 x64/debug/rdaf/at/example.db
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

* collect data during a live session, and at the end of the session, use the Save Values menu item
* change x64/debug/rdaf/at/choices.cfg to have sim_start=on
* restart the application - simulation mode is auto-started
* trades should show up in similar time frames

