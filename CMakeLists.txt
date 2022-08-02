# trade-frame (solution root)
cmake_minimum_required (VERSION 3.13)

project(trade-frame)

# see https://blog.raymond.burkholder.net/index.php?/archives/1183-GCC-Optimization-for-Native-Architecture.html
add_definitions(-march=native)
add_definitions(-pthread)

# can't use 20 yet, seems to be many errors created
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")

add_subdirectory(Alpaca)
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
add_subdirectory(Phemex)
add_subdirectory(Scanner)
add_subdirectory(TableTrader)
add_subdirectory(Weeklies)

add_subdirectory(lib)

if(EXISTS /usr/local/lib/rdaf)
  add_subdirectory(rdaf)
endif()
