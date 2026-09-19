# TFIQFeed

Engine to talk to DTN IQFeed for Level1 & Level2 data.

## Trade Conditions

Answers [#4](https://github.com/rburkholder/trade-frame/issues/4): where to find the meaning of the
trade condition codes that show up in market data.

There is no static table of trade condition codes in this codebase, and there isn't meant to be one -
IQFeed treats the code set as data, not a fixed spec, and this library follows that design.

A trade message's `CTradeConditions` field (see [Messages.h](Messages.h)) carries the condition as a
2-digit hex value. Historical bar data carries the same thing as a hex string in `sTradeConditions` (see
[HistoryQuery.h](HistoryQuery.h)). Neither file says what a given code means - that mapping isn't fixed,
and isn't shipped with IQFeed's client software either.

IQFeed publishes the code-to-meaning mapping itself, over the same socket connection, as reference data
rather than as documentation. [SymbolLookup](SymbolLookup.h) requests it and parses the wire format
IQFeed sends back (`TC,LS,<numeric id>,<short name>,<long name>,` - see the `TradeConditionParser`
grammar in [SymbolLookup.cpp](SymbolLookup.cpp)) into a `TradeCondition{ sShortName, sLongName }` per
code, keyed by that numeric id.

`IQFeed<T>::OnNetworkConnected()` (see [IQFeed.h](IQFeed.h)) already triggers this lookup automatically
on every connect - you'll see a line like `IQF Lookup Tables: ListedMarkets=N, SecurityTypes=N,
TradeConditions=N` on stdout once it completes, confirming the table came down. Practically: connect,
then consult whatever your IQFeed subscription returns, rather than a table in this repo - the set of
codes and their names is defined by IQFeed's feed version, not by this library.

There's currently no accessor exposing `m_mapTradeCondition` back out of `IQFeed<T>` at runtime (unlike
`SymbolList`, which is public) - raised in #4 and left out for now pending a concrete use case, since the
right shape (a lookup-by-id method vs. exposing the map directly, and how short/long names get used)
depends on that use case.
