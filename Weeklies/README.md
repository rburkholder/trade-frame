# Weeklies

## Introduction

There is a book 'out there' by Jeff Augen about trading weekly options.

This application performs some simple statistics on the weekly option listings.

The CBOE maintains a web page about [Weekly Options](http://www.cboe.com/micro/weeklys/availableweeklys.aspx) with a
[link to a spreadsheet](https://www.cboe.com/publish/weelkysmf/weeklysmf.xls) listing the options.

* Download the spreadsheet and place it in the ./x64 directory.
* Use the _IQFeedMarketSymbols_ app to download/update the ./x64/mktsymbols_v2.txt file from IQFeed, parse it, and 
write the result to the ./x64/symbols.ser binary file.  The symbols.ser file is used by most other applications to generate 
symbol/instrument lists for lookup and identification.
* Use the _IQFeedGetHistory_ application to download/update the latest end-of-data data set.
* In the Weekies source code file SignalGenerator.cpp, around line 43, update the date to be the date of the last day of the daily eod downloaded in the previous step.
* Startup up the Weeklies app, and run menu: 'Actions' -> 'Run Scan'.  No need to Turn On IQF.  A bunch of symbols and numbers 
should scroll by.
* A file called ./x64/debug/weeklies.xls has been created which captures those numbers, and can be opened in any spreadsheet app.  A few elementary statistics and probability calculations are presented for each symbol in the list which could 
be used for generating trade ideas.  The file needs to be manually deleted between runs.
