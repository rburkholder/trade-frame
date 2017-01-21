# trade-frame

## Introduction

This is an application I use for automated trading securities.  
C++ is used throughout for building high-capacity, low-latency trading applications.

I use NetBeans as an IDE for development.  As such, all build and project files are NetBeans related.

## Building

Scripts are library version specific. I use Debian Stretch/Testing x64.  Build notes are as of 2016/09/25.
I use the NVidia video card with the binary NVidia drivers (building wxWidgets needs this).
Have about 10G free to build the project and related libraries and installs

Debian Stretch netinst (daily snapshot usually works): 
https://www.debian.org/devel/debian-installer/
http://cdimage.debian.org/cdimage/daily-builds/daily/arch-latest/amd64/iso-cd/


```
# run with bash
# create working directory
mkdir rburkholder
cd rburkholder
# git for latest code, wine for IQFeed daemon
sudo apt-get update && apt-get install git wine wget
sudo dpkg --add-architecture i386 && apt-get install wine32
wget http://www.iqfeed.net/iqfeed_client_5_2_5_0.exe
wine iqfeed_client_5_2_5_0.exe
# interactive brokers Java for linux
wget https://download2.interactivebrokers.com/installers/tws/stable/tws-stable-linux-x64.sh
sh tws-stable-linux-x64.sh
# netbeans ide and build env
wget http://download.netbeans.org/netbeans/8.2/final/bundles/netbeans-8.2-cpp-linux-x64.sh
sh netbeans-8.2-cpp-linux-x64.sh
# initial library install
git clone https://github.com/rburkholder/libs-build.git
libs-build/build.sh tradeframe
# main trade-frame code
git clone https://github.com/rburkholder/trade-frame.git
# pre-build some of the example apps
pushd trade-frame
cd IQFeedMarketSymbols
make
cd ../IQFeedGetHistory
make
cd ../Hdf5Chart
make
cd ../LiveChart
make
cd ../ComboTrading
make
cd ../StickShift2
make
cd ../HedgedBollinger
make
popd

```

## Starting Up

* Start the IQFeed daemon by starting Apps -> Wine -> Programs -> IQFeed -> IQWatchQuote (may need to start twice, and then watch a symbol)
* Start Interactive Brokers TWS and connect to a paper trading account 
.* do not use an active account for testing
.* when connecting via an application, you will need to go into the settings to enable the API, and to activate the port
* Start NetBeans
,* File -> Project Groups -> New Group -> Folder of Projects -> [browse to trade-frame]
* IQFeedMarketSymbols project:
  * run the app:
    * Actions -> New Symbol List Remote
    * the program will spend a few minutes downloading, parsing, and saving the latest IQFeed symbol list
    * lots of messages will be generated, no need to evaluate them, other than the summary stats at the end
    * File -> Exit
* IQFeedGetHistory project:
  * obtains daily ohlc values, used to refresh data
  * repeats the symbol download, which was done with IQFeedMarketSymbols 
  * run the app:
    * ensure the iqfeed daemon is running (you should see active updates in the quote monitor)
    * 'turn on' IQF
    * Actions -> download n # of days:
      * 0 to download full history of symbol, can take a while
      * 10 to try a test
      * a symbol download will commence
      * there will be a couple of minutes of no activity while the symbol list is scanned
      * data for a series of symbols will start
      * once the message 'Process Complete' shows, the download is complete
    * 'turn off' IQF
    * File -> Exit 
* more apps to be described here ...

## Background

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

* IQFeedMarketSymbols - automatically download and decompress the latest mkt_symbol.txt file from dtn,iq
* IQFeedGetHistory - load up with historical data for looking for trading ideas
* Hdf5Chart - view the contents of the hdf5 data set
* LiveChart - view an instrument in real time
* ComboTrading - basics of trading multiple securities, such as various options strategies
* StickShift2 - some rough code for some options ideas
* HedgedBollinger - some experiements in futures

The announcement on my blog:  http://blog.raymond.burkholder.net/index.php?/archives/679-trade-frame-c++-securities-trading-software-development-framework.html

Some other, possibly, related entries:  http://blog.raymond.burkholder.net/index.php?/categories/23-Trading

