# trade-frame

This is an application I use for automated trading securities.  
C++ is used throughout for building high-capacity, low-latency trading applications.

I use NetBeans as an IDE for development.  As such, all build and project files are NetBeans related.

Current Market Data Providers and Execution vendors:

* IQFeed: real time market data and historical data
* Interactive Brokers:  real time market data, real time order execution

Securities types:

* Equities
* Options
* Futures
* Futures Options

Libraries used (use my lib-build respository to download and build the various dependencies):

* wxWidgets
* boost
* curl
* zlib
* hdf5
* sqlite
* exelformat

The libraries are cross platform capable: Linux and Windows

The code started out on Windows using Visual Studio, and is now predominately tested on Linux Debian Stretch/Testing.

The lib directory has a series of libraries I use throughout the various applications.  Primary libraries include:

* TFTimeSeries - manage trades, quotes, greeks
* TFSimulation - simulation engine
* TFIQFeed - engine to talk to DTNIQ Feed (ask me for a referral)
* TFInteractiveBrokers - engine to talk to IB
* TFIndicators - some indicators
* TFHDF5TimeSeries - wraps the HDF5 library for storing time series
* TFOptions - options calculations
* TFTrading - manages orders, executions, portfolios, positions, accounts, 
* OUCharting - wrapper around ChartDirector for plots and charts
* OUSqlite - database for maintaining trading records

During its infancy, the code used MFC (Microsoft Foundation Classes), some Berkeley DB code, and various other modules, 
which I now no longer support.  The code remains in the repository for historical value, and for the time it might be 
re-written for current use.

These are some of the currently supported applications:

* Hdf5Chart - view the contents of the hdf5 data set
* HedgedBollinger - some experiements in futures
* IQFeedGetHistory - load up with historical data for looking for trading ideas
* LiveChart - view an instrument in real time
* StickShift2 - some rough code for some options ideas
* ComboTrading - basics of trading multiple securities, such as various options strategies

The announcement on my blog:  http://blog.raymond.burkholder.net/index.php?/archives/679-trade-frame-c++-securities-trading-software-development-framework.html

Some other, possibly, related entries:  http://blog.raymond.burkholder.net/index.php?/categories/23-Trading

