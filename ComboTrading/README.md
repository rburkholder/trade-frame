A backspread:  sell one At The Money (delta ~0.50), buy two about Out Of The Money (delta ~0.25):
![QGC and GLD Backspread with credit](/notes/pictures/QGC_GLD_backspread_2018-08-17_12-47-23.png)
Gamma shows approximately where ATM (At The Money) options are:
![GLD option chain with live greeks](/notes/pictures/gld_option_chain_2018-08-17_12-46-29.png)
GLD was trading around 111.74 at the time of the snapshots:
![GLD CHART](/notes/pictures/GLD_2018-08-17_13-06-10.png)

ComboTrading is work in progress, and a bit rough around the edges.  At this point, it is evolving towards monitoring 
option combos.  More work is required.

In the meantime, to see the direction in which it is heading:

* Ensure IQFeed client is up and running
* Ensure the Interactive Brokers TWS software is running, that the API is active, and that the listening port is 7496.  You should _preferably_ be running in paper trading mode.
* Startup ComboTrading from within NetBeans.  It should be starting from within x64/debug directory.  A bunch of windows may startup.  Spread them across your screen, looking for the 'Combo Trading' window.
* Use from the menu:  File -> Save Config [this saves the window arrangement to make it easy to recover on the next run]
* Use from the menu:  Symbol List -> New Symbol List Remote  [this will take some time to download and process, many errors may be seen, but they can be ignored, it is the nature of the beast]
* Once the window has stopped scrolling, you should see a message at the bottom that is saving.  Wait for the saved confirmation.  This loads a set of symbols.  Loading the IQFeed market symbol file is needed for determining symbol types.  
* Each subsequent time ComboTrading is started, use Menu: Symbol List -> Load Symbol List.  This loads the symbols from the local copy.  This list is required to be loaded in order for instruments to be selected in subsequent steps.
* To the left of 'IQF', click 'Turn On'.  There should be a bunch of 'Start Watching ' entries in the log window.  These are symbols used for determining Libor (zero risk) rates in option greek calculations.  If they don't show, then somehow the app is not communicating with the IQFeed client.
* To the left of 'IB', click 'Turn On'.    You should get four or so 'HMDS data farm connection is OK' messages.  Yes, they are proceeded by 'error -1', which in this case, is benign.  The window labelled as 'Interactive Brokers' should reflect current account statistics.  There is a window slider over at the left which can be dragged right-wards to show the numbers.
* In the 'Instrument Management' window, you can right click on 'Instruments' to pop up a menu.  Not all menu items are functional.
* Choose add instrument.  Select something like GLD in the IQFeed window.  The symbol should also be mirrored in the IB window. If IQFeed and TWS are properly running, a contract id, in this case, 51529211 will show up.  Click OK.  The instrument should show in the list.  Clicking on the instrument, will show the start of a graph.  If this is outside of trading hours, it will be blank.  During trading hours, updates should start occurring.
* To add an instrument, such as a future, it is a bit more complicated.  For example, for gold futures, QGC needs to be entered in the IQFeed space, and the IB space needs to be manually changed to GC.  You have to have to know the expiry date. The expiry calendar drop down can be used to find the expiry.  Once an expiry is found, the IB contract number should appear.  At this point, the OK is available.
* Option chains can be viewed by right clicking on the instrument and select 'Option Chains'.  There are too many to update in real time, so find the expiry data of interest in the tabs, and then find the strike of interest.  control click on it.  A watch will start on the Put and Call.  Control click again to remove the watch.  Too far out of the money will not yield immediate results as the calcuations require active quotes to arrive.
* To see the chart for an option, right click on the instrument in the left column, and choose 'New Option'.  You'll need to choose Put/Call, the strike, the root symbol, and the proper expiry.  Once a contract number shows, Ok can be clicked to add it to the list.
