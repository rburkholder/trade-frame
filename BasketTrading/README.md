# BasketTrading

## Introduction

This is an environment for running an algorithm over a collection of instruments.  Positions are opened at the beginning
of the trading day/session, and closed by the end of the session, if not already stopped out.

* BasketTrading - main file for gui and starting the process
* PanelBasketTradingMain - GUI container for buttons, BasketTrading assigns the event handlers
* PanelPortfolioStats - GUI container for presenting elementary P/L during session
* MasterPortfolio - Maintains instances of ManageStrategy
* ManageStrategy - Instance created for each primary instrument chosen
* SymbolSelection - Determines instruments to be traded by MasterPortfolio
