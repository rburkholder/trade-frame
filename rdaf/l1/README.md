# ROOT Data Analysis Framework to review Level I Market Data

Example configuration file called x64/debug/rdaf_l1.cfg
```
symbol=TST$Y
```

* When the application starts, click the 'Turn On' button for IQF to connect to IQFeed
* Use the Actions menu to Start a symbol watch on the chosen symbol
* A chart will update with real time data
* Use the ROOT Cling to review the live data
  * h1 is Histogram Delta
  * h2 is Histogram Total

* ChartData.cpp is where ROOT is populated

