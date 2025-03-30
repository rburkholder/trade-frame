# Collector - Stream Instrument Data to Disk

A tool to capture instrument statistics to the tradeframe.hdf5 datafile.

Useful batch oriented data acquisition for use in back-testing and for machine learning.

Currencies, Equities, Futures, and Options are eligible instruments.  At-The-Money Implied Volatility tracking has been added.

It is designed to collect a complete 23 hour trading session.

```
$ cat /var/local/tradeframe/collector.cfg
symbol_name_l1=USDCAD.FXCM
symbol_name_l1=EURUSD.FXCM
symbol_name_l1=GBPUSD.FXCM
symbol_name_l1=SLV
symbol_name_l1=GLD
symbol_name_l1=SPY
symbol_name_l1=@ES~
symbol_name_atm=@ES~
#symbol_name_greeks=@E2BF25C600000
stop_time=17:30:00
```

For continuous futures based upon the IQFeed naming scheme, the '~' in the file is converted to a corresponding '#'.  The continuous form is automatically converted to the appropriate front month's symbol.

Stop Time is in the Eastern time zone.  Only the time is required as Collector can be run on a daily basis.
The collector will expire at the indicated time, regardless of the current day.
The sample 17:30 is mid-way between the current futures session (which ends at 17:00 Eastern) and the new futures session (which begins at 18:00 Eastern).

Using Mountain Standard Time, here is a sample entry in 'crontab -e' to start the collector and run starting Sunday afternoon daily until Friday afternoon.

```
# you'll need to chown +x /usr/local/bin/collector.sh
40 15  *   *   0,1,2,3,4  /usr/local/bin/collector.sh
```

The content of collector.sh might be:

```
#!/bin/sh
cd /var/local/tradeframe
/usr/local/bin/Collector
```

This configuration is based upon the directory with  proper privileges defined at /var/local/tradeframe

