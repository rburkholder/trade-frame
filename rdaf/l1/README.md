# ROOT Data Analysis Framework to review Level I Market Data

Example configuration file called x64/debug/rdaf_l1.cfg
```
symbol=TST$Y
time_bins=3600
#          yyyymmdd hhmmss
time_upper=20220228T000000
time_lower=20220201T000000
price_bins=200
price_upper=3000
price_lower=2000
vol_side_bins=100
vol_side_upper= 500
vol_side_lower=-500
vol_ttl_bins= 100
vol_ttl_upper=1000
vol_ttl_lower=0
```

* When the application starts, click the 'Turn On' button for IQF to connect to IQFeed
* Use the Actions menu to Start a symbol watch on the chosen symbol
* A chart will update with real time data
* Use the ROOT Cling to review the live data
  * h1 is Histogram Delta
  * h2 is Histogram Total

* ChartData.cpp is where ROOT is populated

Some Document references:
* https://root.cern/about/
* https://root.cern/install/
* https://root.cern/cling/
* https://root.cern/manual/first_steps_with_root/
* https://root.cern/manual/integrate_root_into_my_cmake_project/
* https://root.cern/manual/creating_a_user_application/
* https://root-forum.cern.ch/t/how-to-embed-a-tcanvas-in-external-applications/28247
* https://root.cern/manual/embed_root_in_gui/
* https://root.cern/doc/master/index.html
* https://root.cern/doc/master/classTRint.html

