# SP500 Application

An adventure into trading the S&P 500 using SPY with the support of 
TRIN, TICK and Advance/Decline market supplied indicators.

The initial iteration of this application relies on HDF5 files 
collected by the Collector application with the following configuration:

LibTorch is being introduced for testing an LSTM based model for machine learning.
* libtorch needs to reside in /usr/local/share
* libtorch can be downloaded from https://pytorch.org/get-started/locally/

```
symbol_name_l1=SPY
symbol_name_l1=II6A.Z  #S&P 500 ISSUES ADVANCES DTN - sum to 500
symbol_name_l1=II6D.Z  #S&P 500 ISSUES DECLINES DTN - sum to 500
symbol_name_l1=JT6T.Z  #S&P 500 TICKS NET              - useful
symbol_name_l1=LI6N.Z  #EXCHG ONLY S&P 500 TICKS NET   - useful
symbol_name_l1=RI6T.Z  #S&P 500 TRIN         - not sure
symbol_name_l1=TR6T.Z  #S&P 500 TICKS RATIO  - useful
symbol_name_l1=VI6A.Z  #S&P 500 ISSUES VOLUME UP
symbol_name_l1=VI6D.Z  #S&P 500 ISSUES VOLUME DOWN
stop_time=17:20:00
```

Symbols and their contribution to signals:
```
https://ws1.dtn.com/IQ/Search/#
$ grep 'S&P' x64/mktsymbols_v2.txt  |grep MKTSTATS|grep 500
AI6T.Z  S&P 500 AVERAGE PRICE   DTN     DTN     MKTSTATS

II6A.Z  S&P 500 ISSUES ADVANCES DTN     DTN     MKTSTATS
II6D.Z  S&P 500 ISSUES DECLINES DTN     DTN     MKTSTATS
II6T.Z  S&P 500 ISSUES TOTAL    DTN     DTN     MKTSTATS
II6U.Z  S&P 500 ISSUES UNCHG    DTN     DTN     MKTSTATS

LI6A.Z  EXCHG ONLY S&P 500 TICKS UP     DTN     DTN     MKTSTATS
LI6D.Z  EXCHG ONLY S&P 500 TICKS DOWN   DTN     DTN     MKTSTATS
LI6N.Z  EXCHG ONLY S&P 500 TICKS NET    DTN     DTN     MKTSTATS
LI6T.Z  EXCHG ONLY S&P 500 TICKS TOTAL  DTN     DTN     MKTSTATS

M2006B.Z        S&P 500 200 DAY MOVING AVG BELOW        DTN     DTN     MKTSTATS
M2006V.Z        S&P 500 200 DAY MOVING AVG ABOVE        DTN     DTN     MKTSTATS

TI6A.Z  S&P 500 TICKS UP        DTN     DTN     MKTSTATS
TI6D.Z  S&P 500 TICKS DOWN      DTN     DTN     MKTSTATS
TI6T.Z  S&P 500 TICKS TOTAL     DTN     DTN     MKTSTATS
TR6T.Z  S&P 500 TICKS RATIO     DTN     DTN     MKTSTATS

VI6A.Z  S&P 500 ISSUES VOLUME UP        DTN     DTN     MKTSTATS
VI6D.Z  S&P 500 ISSUES VOLUME DOWN      DTN     DTN     MKTSTATS
VI6T.Z  S&P 500 ISSUES VOLUME TOTAL     DTN     DTN     MKTSTATS
VI6U.Z  S&P 500 ISSUES VOLUME UNCHG     DTN     DTN     MKTSTATS

RI6T.Z  S&P 500 TRIN    DTN     DTN     MKTSTATS
```
* A trin of less than 1.00 usually means that a lot of buyers are taking securities up in price,
and that’s positive.

* A trin above 1.00 indicates that the sellers are acting more strongly,
which means a lot of negative sentiment is in the market.
