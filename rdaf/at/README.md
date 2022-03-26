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

### x64/debug/rdaf/at/choices.cfg

Note: there is a new name for the configuration file, and there is a new format.

This is a specialization of the AutoTrade project.  This project uses CERN's rdaf (ROOT Data Analysis Framework)
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

NOTE: I have to test this yet

* collect data during a live session, and at the end of the session, use the Save Values menu item
* restart the application, and set D1 & X to Sim, and turn on
* a Simulation menu will appear
* Simulation->Run to start the simulation
  * trades should show up in similar time frames
