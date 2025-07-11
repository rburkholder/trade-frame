# SP500 Application

An adventure into trading the S&P 500 using SPY with the support of
TRIN, TICK and Advance/Decline market supplied indicators via libtorch training.

LibTorch is being introduced for testing an LSTM based model for machine learning.
* libtorch needs to reside in /usr/local/share
* libtorch can be downloaded from https://pytorch.org/get-started/locally/
* if one or more NVidia cards are present, libcuda can be used by libtorch
* see [Installing LibTorch with Cuda on NVIDIA GeForce RTX 4070](https://blog.raymond.burkholder.net/index.php?/archives/1285-Installing-LibTorch-with-Cuda-on-NVIDIA-GeForce-RTX-4070.html) for installation hints.

The initial iteration of this application relies on HDF5 files
collected by the Collector application with the following
collector.cfg configuration:

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

Note: m_pStrategy->InitForNextDay() assumes that the Collector has been started prior to 18:00 EST
the previous evening to facilitate the collection of currencies and overnight futures

Current code:
* runs a training cycle, then
* runs a prediction cycle

The configuration file provides key settings (including two hyperparameters):

```
$ cat x64/debug/sp500.cfg
# choose 1 of 3 modes:
#mode=view_training_data
#mode=view_validate_data
mode=train/validate
# two files for running the train/validate cycle:
file_training=collector-20250629.hdf5
file_validate=collector-20250630.hdf5
# fine tuning:
#learning_rate=0.001
#num_epochs=10000
# rough testing:
learning_rate=0.01
num_epochs=1000
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
