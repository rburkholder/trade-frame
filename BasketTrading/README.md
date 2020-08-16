# BasketTrading

## Introduction

This is an environment for running an algorithm over a collection of instruments.  Positions
are maintained day to day.

* BasketTrading - main file for gui and starting the process
* PanelBasketTradingMain - GUI container for buttons, BasketTrading assigns the event handlers
* PanelPortfolioStats - GUI container for presenting elementary P/L during session
* MasterPortfolio - Maintains instances of ManageStrategy
* ManageStrategy - Instance created for each primary instrument chosen - header & 'using' chooses combo style
* SymbolSelection - Determines instruments to be traded by MasterPortfolio

Currently designed to test a collar strategy (not yet complete) on weekly options.

Option Strategy Descriptions:
https://www.fidelity.com/learning-center/investment-products/options/options-strategy-guide/overview

To run, you'll need to:

* Approximately weekly, Sunday night is best:
  * _IQFeedMarketSymbols_: obtain latest market symbols, which includes OPRA option symbols
  * _Weeklies_ (weekly options list):
    * download [link to a spreadsheet](https://www.cboe.com/publish/weelkysmf/weeklysmf.xls) and place into ./x64 directory
    * run Weeklies to load and test the file
* When just starting out, run _IQFeedGetHistory_ with 0 for daily bars, this will load all available daily bars for the common US exchanges
* On a daily basis, run _IQFeedGetHistory_ late at night, or first thing in the morning with 10 for daily bars, to load the latest
* _BasketTrading_ can then be started prior to market hours
  * Use Symbols -> Load List to load symbol list, it may take a little while, and wait for it to complete
  * Click 'Turn On' for IQF and IB, both need to be running, IQF for the data stream, and IB for execution (be sure to use paper trading to start)
  * Use Manage -> Load to select and load symbols 
  * When playing around, delete ./x64/debug/BasketTrading.db to reset trades and lists

If you have problems, let me know.  I need to re-run to be sure I havn't broken anything.

