# Market Depth aka Level II

IQFeed provides Market Depth information in two styles:

* four character market maker names (does not have order ids) -- limited to nasdaq equities -- the top of book for each market maker is provided.  This does mean that we don't see the full level II information 
* order ids (does not have market maker names): -- provided in some futures feeds ICE and CME (which has ES) -- this is a full order book represented down to the order level:  orders arrive, are updated (for quantity), or deleted

The MsgPriceLevelArrival.* and MsgPriceLevelDelete.* have not been fully developed, nor tested.

The Market Depth data arrives in messages which are parsed in MsgOrderArrival.* and MsgOrderDelete.*.  

Dispatcher.* connects to IQFeed's Level II data, issues the commands, recieves the messages, runs the parsers, and then issues the callback.

Symbols.* handles the bulk of managing the structures associated with the messages.  Four classes are represented here:

* Symbols: First entry point of messages from Dispatcher.  As each message contains a SymbolName, the streams can be segregated by SymbolName.  Each symbol is identified as using a MarketMaker type format or an Order type format.  A Carrier object is used as a form of indirect jump table for subsequent messages.
* L2Base: this is a base class which maintains volume@price_level information for each of bid and ask
* MarketMaker: inherits L2Base, and is used when messages contain a market maker field.  MarketMaker contains its own maps to track the quantities for each Market Maker at each price level (one price level per market maker), which is used to update the aggregate price levels in L2Base
* OrderBased: inherits L2Base, and is used when messages contain an order id.  It holds the maps with active limit orders, one for bid and one for ask.  These active orders are used update the aggregate volume@price-levels in the L2Base tables.

On each update, whether it be a bid or an ask, a callback is called so application can make use of the changes.

A future enhancement will turn, at minimum, the L2Base class into a template so that additional application layer statistics can be associated with each price level (rather than maintaining a separate but similar data strcture which be searched and traversed in a similar manner.

The message handlers OnMBOUpdate and OnMBODelete in the MarketMaker class have an early bypass when m_fMarketDepth has been defined.  A TradeFrame MarketDepth object is constructed and passed along via the callback in m_fMarketDepth.

Thos allows an application to create custom thread and structure processing.

The project rdaf/at/AppAutoTrade uses this ability to construct it's own MarketMaker object for local use.  Once turned into a template version, an application may add additional fields into the data structures for integrated local use.

When m_fMarketDepth is not defined, the m_fAskVolumeAtPrice and m_fBidVolumeAtPrice in L2Base are used to callback with updates to price levels.  The volume is absolute volume at price, not the delta.  This can be changed once usage patterns are determined.


