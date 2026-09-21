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

### Commonly-seen codes and why they matter for bar-building

The live table above is the source of truth, but a few codes come up often enough in practice to be
worth naming directly - reported from field experience with the feed, not from this codebase, so treat
the specific hex/decimal values as a starting point to confirm against your own connection's table
rather than a guarantee:

A trade's condition field can carry more than one flag concatenated as pairs of hex digits - e.g. a raw
`3D87` splits into `3D` (61 decimal, Intramarket Sweep) and `87` (135 decimal, Odd Lot Trade).

| Hex | Decimal | Condition | Typical impact on OHLC bar-building |
|---|---|---|---|
| `87` | 135 | Odd Lot Trade | Small-size execution; usually excluded from standard historical bars. |
| `89` | 137 | Qualified Contingent Trade (QCT) | Can produce artificial price spikes; usually excluded from OHLC calculation. |
| `44` | 68 | Stock Option Trade | Often tied to a multi-leg execution package rather than a standalone print. |
| `06` | 6 | Cash Trade | Same-day clearing execution. |
| - | - | Average Price Trade | Price is calculated over a period rather than at a single moment; can distort real-time order-flow tracking if treated as a normal print. |

This is exactly the kind of thing that shows up as unexplained spikes or odd lots polluting an OHLC bar
or a chart if a consumer of this library isn't filtering on the condition field at all - background
reading on the practical symptom: [Too many spikes with IQFeed data](https://forum.amibroker.com/t/too-many-spike-with-iqfeed-data/16973),
[IQFeed plugin bad-tick filter](https://forum.amibroker.com/t/iqfeed-plugin-7-xx-with-bad-tick-filter/40778).

Two reasonable filtering strategies, depending on what a consumer of this library actually wants:

- **Standard/default filtering** - mirrors IQFeed's own historical-bar generation: drop all "Other"-type
  trades, including odd lots, before bar-building.
- **Advanced/custom filtering** - keep odd lots (useful for microstructure analysis) but still drop
  Average Price and QCT trades, since those are the two most likely to produce a spurious vertical spike
  on a chart.

Either way, the filtering decision belongs in the consumer of `CTradeConditions`/`sTradeConditions`, not
in this library - matching the same "IQFeed's table is the source of truth, trade-frame just carries the
code through" design as the rest of this section.
