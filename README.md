# trade-frame

## Introduction

This is an application I use for automated trading securities.  
C++ is used throughout for building high-capacity, low-latency trading applications.

A C++17 compiler is used to build the libraries and code. It was built on Windows a number of years ago, 
but the focus changed towards supporting a Linux environment.  Some work will be needed to make it build on Windows again.

CMake is used for build management.

## Example

Picture from the ComboTrading project showing an instrument list, with a chart for one of the listed instruments.  Bid, ask, spread, and greeks are charted for the option (charted real-time).

![Option Greeks Timeline](notes/pictures/qgc-20180925-c-1195_2018-08-16_12-55-40.png)

More pictures under ![ComboTrading](ComboTrading)

## Building

Scripts are library version specific.  Build notes are as of 2021/11/22.
There are some wxWidget requirements for using a GTK variation of video drivers (I've used Nvidia and Radeon cards successfully).  

You'll need to have about 10G drive space free to build the project, the related libraries, 
as well as the installs (from my libs-build repository).

Debian Bullseye is used as the platform.  The library installer is specific to this distribution.  The installer may or may not work with other 
distributions or flavours.

DTN/IQFeed requires Wine to run.  Starting with the 6.2 release of IQFeed, wine32 is no longer required.  
The installation of wine may generate some wine32 messages and errors, but they can be ignored.


```
# run with bash
# create working directory
mkdir rburkholder
cd rburkholder
# git for latest code, wine for IQFeed daemon
sudo apt-get update && apt-get install git wine wget
wget http://www.iqfeed.net/iqfeed_client_6_2_0_25.exe
wine iqfeed_client_6_2_0_25.exe

# interactive brokers Java for linux
wget https://download2.interactivebrokers.com/installers/tws/stable/tws-stable-linux-x64.sh
sh tws-stable-linux-x64.sh

# install and build initial environment and libraries
git clone https://github.com/rburkholder/libs-build.git
cd libs-build
./build.sh tradeframe
# rdaf is required for the project in the rdaf subdirectory
./build.sh rdaf
cd ..

# main trade-frame code
git clone https://github.com/rburkholder/trade-frame.git
# if you have access to the up-to-date private library:
# git clone https://github.com/rburkholder/tf2.git

# if you build manually (this is not required if you load the folder into vscode):
cd trade-frame
mkdir build
cd build
cmake ..
cmake --build . --config Debug
# cmake --build . --config Release   # alternate build flavour

```

I use Visual Studio Code as my IDE.  I have the following extensions installed:
* C/C++ [Microsoft]
* clangd [LLVM Extensions]
* CMake [twxs]
* CMake Tools [Microsoft]

I have notes for this combination at 
  [Visual Studio Code with CMake and Clangd](https://blog.raymond.burkholder.net/index.php?/archives/1037-Visual-Studio-Code-with-CMake-and-Clangd.html)


## Starting Up

* Start the IQFeed daemon by starting Apps -> Wine -> Programs -> IQFeed -> Watchlist [or from the command line: wine ~/.wine/drive_c/Program\ Files/DTN/IQFeed/iqconnect.exe -autoconnect & ] Login with your credentials and checkbox the Save Username/Password and Autoconnect the first time
* Start Interactive Brokers TWS and connect to a paper trading account 
.* do not use an active account for testing
.* when connecting via an application, you will need to go into the settings to enable the API, and to activate the port
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
* BasketTrading project:
  * runs a Darvas selection process on daily bars retrieved by IQFeedGetHistory
  * runs an automated buy/sell/stop process from market open to market close on a basket of instruments
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
* sqlite (included in source)
* exelformat (included in source)

The code started out on Windows using Visual Studio, and is now predominately tested on Linux Debian.  Some work is required 
to port back to Windows.

The lib directory has a series of libraries I use throughout the various applications.  Primary libraries include:

* TFTimeSeries - manage trades, quotes, greeks
* TFSimulation - simulation engine
* TFIQFeed - engine to talk to DTNIQ Feed (ask me for a referral)
* TFInteractiveBrokers - engine to talk to IB
* TFIndicators - some indicators
* TFHDF5TimeSeries - wraps the HDF5 library for storing time series
* TFOptions - options calculations
* TFTrading - manages orders, executions, portfolios, positions, accounts, 
* TFVuTrading - provides a number of forms, panels, and related user-interface elements
* OUCharting - wrapper around ChartDirector for plots and charts
* OUSQL - which is an ORM wrapper around a sqlite database for maintaining trading records

During its infancy, the code used MFC (Microsoft Foundation Classes), some Berkeley DB code, and various other modules, 
which I now no longer support.  The code remains in the repository for historical value, and for the time it might be 
re-written for current use.

These are some of the currently supported applications:

* IQFeedMarketSymbols - automatically download and decompress the latest mkt_symbol.txt file from dtn,iq
* IQFeedGetHistory - load up with historical data for looking for trading ideas
* Hdf5Chart - view the contents of the hdf5 data set
* LiveChart - view an instrument in real time
* ComboTrading - basics of trading multiple securities, such as various options strategies
* StickShift2 - some rough code for some option trading ideas
* HedgedBollinger - some experiments in futures, mostly tracking at the money implied volatility

The announcement on my blog:  http://blog.raymond.burkholder.net/index.php?/archives/679-trade-frame-c++-securities-trading-software-development-framework.html

Some other, possibly, related entries:  http://blog.raymond.burkholder.net/index.php?/categories/23-Trading

## Testing

* IQFeed testing: you can utilize the symbol TST$Y, this symbol sends a loop of data 24/7. (2019/03/12)

