# trade-frame (solution root)
cmake_minimum_required (VERSION 3.13)

project(trade-frame)

# see https://blog.raymond.burkholder.net/index.php?/archives/1183-GCC-Optimization-for-Native-Architecture.html
add_definitions(-march=native)

add_subdirectory(ArmsIndex)
add_subdirectory(AutoTrade)
add_subdirectory(BasketTrading)
#add_subdirectory(BookTrader)
add_subdirectory(ComboTrading)
add_subdirectory(DepthOfMarket)
add_subdirectory(Dividend)
add_subdirectory(ESBracketOrder)
add_subdirectory(Hdf5Chart)
add_subdirectory(HedgedBollinger)
add_subdirectory(IndicatorTrading)
add_subdirectory(IntervalSampler)
add_subdirectory(IntervalTrader)
add_subdirectory(IQFeedMarketSymbols)
add_subdirectory(IQFeedGetHistory)
add_subdirectory(LiveChart)
add_subdirectory(MultipleFutures)
add_subdirectory(Scanner)
add_subdirectory(Weeklies)

add_subdirectory(lib)

if(EXISTS /usr/local/lib/rdaf)
  add_subdirectory(rdaf)
endif()
