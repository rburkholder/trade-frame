# SP500 Application

An adventure into trading the S&P 500 using SPY with the support of 
TRIN, TICK and Advance/Decline market supplied indicators.

The initial iteration of this application relies on HDF5 files 
collected by the Collector application with the following configuration:

```
symbol_name_l1=SPY
symbol_name_l1=II6A.Z  #S&P 500 ISSUES ADVANCES DTN - sum to 500
symbol_name_l1=II6D.Z  #S&P 500 ISSUES DECLINES DTN - sum to 500
symbol_name_l1=JT6T.Z  #S&P 500 TICKS NET              - useful
symbol_name_l1=LI6N.Z  #EXCHG ONLY S&P 500 TICKS NET   - useful
symbol_name_l1=RI6T.Z  #S&P 500 TRIN         - not sure
symbol_name_l1=TR6T.Z  #S&P 500 TICKS RATIO  - useful
stop_time=17:20:00
```

Symbols and their contribution to signals:

```
https://ws1.dtn.com/IQ/Search/#
LI6D.Z  EXCHG ONLY S&P 500 TICKS DOWN   MKTSTATS        DTN     DTN
LI6N.Z  EXCHG ONLY S&P 500 TICKS NET    MKTSTATS        DTN     DTN
LI6T.Z  EXCHG ONLY S&P 500 TICKS TOTAL  MKTSTATS        DTN     DTN
LI6A.Z  EXCHG ONLY S&P 500 TICKS UP     MKTSTATS        DTN     DTN

TI6D.Z  S&P 500 TICKS DOWN      MKTSTATS        DTN     DTN
JT6T.Z  S&P 500 TICKS NET       MKTSTATS        DTN     DTN
TR6T.Z  S&P 500 TICKS RATIO     MKTSTATS        DTN     DTN
TI6T.Z  S&P 500 TICKS TOTAL     MKTSTATS        DTN     DTN
TI6A.Z  S&P 500 TICKS UP        MKTSTATS        DTN     DTN

RI6T.Z  S&P 500 TRIN    MKTSTATS        DTN     DTN

A trin of less than 1.00 usually means that a lot of buyers are taking securities up in price,
and that’s positive.

A trin above 1.00 indicates that the sellers are acting more strongly,
which means a lot of negative sentiment is in the market.
```
