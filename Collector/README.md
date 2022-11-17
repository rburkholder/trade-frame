# Collector

A tool to capture level I and level II data for a future to the tradeframe.hdf5 datafile.

This has been tested with @ES# only.  It will not work with equities.


$ cat x64/debug/futuresl1l2.cfg
symbol_name=@ES~
stop_time=17:30:00

The '~' is converted to a '#' for a continuous series future.  The continuous form is automatically converted
to the appropriate front month's symbol.

Stop Time is in Eastern time zone.  Only the time is to be supplied.  The collector will expire at the indicated time, regardless of the current day.  The sample 17:30 is 
mid-way between futures end and the new futures session.
