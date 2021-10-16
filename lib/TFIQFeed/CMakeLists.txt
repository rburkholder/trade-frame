# trade-frame/lib/TFIQFeed
cmake_minimum_required (VERSION 3.13)

PROJECT(TFIQFeed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    BuildInstrument.h
    BuildSymbolName.h
    CurlGetMktSymbols.h
    InMemoryMktSymbolList.h
    IQFeed.h
    IQFeedHistoryBulkQuery.h
    IQFeedHistoryBulkQueryMsgShim.h
#    IQFeedHistoryCollector.h
    IQFeedHistoryQuery.h
    IQFeedHistoryQueryMsgShim.h
#    IQFeedInstrumentFile.h
    Messages.h
    IQFeedMsgShim.h
    IQFeedNewsQuery.h
    IQFeedNewsQueryMsgShim.h
    IQFeedProvider.h
#    IQFeedSymbolFile.h
    IQFeedSymbol.h
    LoadMktSymbols.h
    MarketSymbol.h
    MarketSymbols.h
    OptionChainQuery.h
    Option.h
    ParseFOptionDescription.h
    ParseMktSymbolDiskFile.h
    ParseMktSymbolLine.h
    ParseOptionDescription.h
    ParseOptionSymbol.h
    UnzipMktSymbols.h
    ValidateMktSymbolLine.h
  )

set(
  file_cpp
    BuildInstrument.cpp
    BuildSymbolName.cpp
    CurlGetMktSymbols.cpp
    InMemoryMktSymbolList.cpp
    IQFeed.cpp
#    IQFeedHistoryCollector.cpp
#    IQFeedInstrumentFile.cpp
    Messages.cpp
    IQFeedProvider.cpp
    IQFeedSymbol.cpp
#    IQFeedSymbolFile.cpp
    LoadMktSymbols.cpp
    MarketSymbol.cpp
    MarketSymbols.cpp
    OptionChainQuery.cpp
    Option.cpp
    ParseMktSymbolDiskFile.cpp
    ParseMktSymbolLine.cpp
    UnzipMktSymbols.cpp
    ValidateMktSymbolLine.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )

