# CollectFuL2 - Collect Futures L2 Data

A tool to capture level I and level II data for a future to the tradeframe.hdf5 datafile. 

It is designed to collect a 23 hour trading session.

This has been tested with @ES# and @ESZ12 only.  It will not work with equities.

$ cat x64/debug/futuresl1l2.cfg
symbol_name=@ES~
stop_time=17:30:00

The '~' is converted to a '#' for an IQFeed named continuous future.  The continuous form is automatically converted
to the appropriate front month's symbol.

Stop Time is in Eastern time zone.  Only the time is to be supplied.
The collector will expire at the indicated time, regardless of the current day.
The sample 17:30 is mid-way between the current futures session (which ends at 17:00 eastern) and the new futures session (which begins at 18:00 eastern).
