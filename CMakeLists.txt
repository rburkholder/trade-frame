# trade-frame (solution root)
cmake_minimum_required (VERSION 3.13)
# cmake_policy(CMP0167) need 3.30 at some point for boost
# cmake --help-policy CMP0167

project(trade-frame)

# see https://blog.raymond.burkholder.net/index.php?/archives/1183-GCC-Optimization-for-Native-Architecture.html
add_definitions(-march=native)
add_definitions(-pthread)

# can't use 20 yet, seems to be many errors created
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)   # don't know if this is needed
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x64")

set(WX_VERSION_MAJOR "3.2")
# TF prefix prevents clash with similar names
set(TF_BOOST_VERSION "1.90.0" )

# look in /usr/local/lib/cmake/ for cmake 'find' entries
# currently has vmime, boost, wt, telegram

add_subdirectory(Alpaca)
add_subdirectory(ArmsIndex)
add_subdirectory(AutoTrade)
add_subdirectory(BasketTrading)
#add_subdirectory(BookTrader)
add_subdirectory(BarChart)
add_subdirectory(Collector)
add_subdirectory(ComboTrading)
add_subdirectory(CurrencyTrader)
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
add_subdirectory(MarketTrader)
add_subdirectory(MultipleFutures)
add_subdirectory(OptionTrader)
add_subdirectory(Phemex)
add_subdirectory(Scanner)
add_subdirectory(Weeklies)

add_subdirectory(lib)

if(EXISTS /usr/local/share/libtorch)
  add_subdirectory(SP500)
endif()

if(EXISTS /usr/local/lib/libwt.so)
  add_subdirectory(TableTrader)
  add_subdirectory(WebTrader)
endif()

if(EXISTS /usr/local/lib/rdaf)
  add_subdirectory(rdaf)
endif()

# https://kubasejdak.com/modern-cmake-is-like-inheritance:
# include directories property is automatically added to the compilation flags when the target is being compiled. 
# if other libraries call only target_link_libraries() to both get include paths and link with library, 
#   then no private header will ever leak unintentionally outside the containing library.
# target_link_libraries() means in Modern CMake two things: 
#   use library (get its properties) at compilation stage and link with it at linking stage. 

