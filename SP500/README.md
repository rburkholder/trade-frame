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
