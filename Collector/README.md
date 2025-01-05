# Collector - Stream Equity quote/tick to disk

A tool to capture quote and trade data for symbols to the tradeframe.hdf5 datafile.

Useful for data acquisition for back-testing and for machine learning.

Equities, Futures, and Options are eligible instruments.

It is designed to collect a complete 23 hour trading session.

$ cat x64/debug/collector.cfg
symbol_name=GLD
symbol_name=SPY
stop_time=17:30:00

For continuous futures based upon the IQFeed naming scheme, the '~' in the file is converted to a corresponding '#'.  The continuous form is automatically converted to the appropriate front month's symbol.

Stop Time is in the Eastern time zone.  Only the time is required as Collector can be run on a daily basis.
The collector will expire at the indicated time, regardless of the current day.
The sample 17:30 is mid-way between the current futures session (which ends at 17:00 Eastern) and the new futures session (which begins at 18:00 Eastern).
