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

Currently designed to run strangles on weekly options.

Option Strategy Descriptions:
https://www.fidelity.com/learning-center/investment-products/options/options-strategy-guide/overview

To run, you'll need to:

* Approximately weekly, Sunday night is best, is to use IQFeedMarketSymbols to obtain latest market symbols, including OPRA symbols
* When just starting out, run IQFeedGetHistory with 0 for daily bars, this will load all available daily bars for the common US exchanges
* On a daily basis, run IQFeedGetHistory late at night, or first thing in the morning with 10 for daily bars, to load the latest
* BasketTrading can then be started prior to market hours
* Use menu Symbols -> Load List to load symbol list, it may take a little while
* Click 'Turn On' for IQF and IB, both need to be running, IQF for the data stream, and IB for execution (be sure to use paper trading to start)

If you have problems, let me know.  I need to re-run to be sure I havn't broken anything.

