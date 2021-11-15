## ToDo

* ComboTrading
  * convert lib/TFVuTrading/DialogPickSymbol.cpp to make use of BuildInstrument capability introduced in BasketTrading

* BasketTrading
  * implement gamma aggregation to guide detection of reversal events -- see notes
    * this should remedy premature vertical rolls
  * track bid-ask spreads and use lower end of median/mode to initiate trades (minimize loss due to spread entry)
  * add in-play options to instrument tree
  * menu item to add additional strategy instances

* DepthOfMarket
  * show L1 data on grid
  * show L2 data on grid
  * introduce indicators to grid

* InteractiveBrokers
  * bag the combo orders to help improve mid-spread fills
    * applicable to ComboTrading and BasketTrading

* BracketOrder
  * new project
  * manual trading style
  * show stochastic, manually initiate trade
  * software creates the bounding box to implement tight stop and let the run fly
  * minimize multiple losses, allow gains run

* Implement GARCH calcs for volatility, does this work?
