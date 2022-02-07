# ROOT Data Analysis Framework to review Level I Market Data

Example configuration file called x64/debug/rdaf_l1.cfg
```
symbol=SPY
time_bins=50000
time_upper=1650000
time_lower=1700000
price_bins=20000
price_upper=6000
price_lower=1000
vol_side_bins=10000
vol_side_upper=5000000
vol_side_lower=-5000000
vol_ttl_bins=5000
vol_ttl_upper=5000000
vol_ttl_lower=0
```

* When the application starts, click the 'Turn On' button for IQF to connect to IQFeed
* An example chart is shown.
* Use the Actions menu to Start a symbol watch on the chosen symbol
* A chart will update with real time data
* The ROOT Cling to review the live data
  * h1 is Histogram Delta
  * h2 is Histogram Total

* ChartData.cpp is where ROOT is populated

