# trade-frame/lib/TFBitsNPieces
cmake_minimum_required (VERSION 3.13)

PROJECT(TFBitsNPieces)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
#    CalcAboveBelow.h
    BollingerTransitions.h
    DailyHistory.hpp
    FirstOrDefaultCombiner.h
    FrameWork01.h
    FrameWork02.hpp
    InstrumentFilter.h
    InstrumentSelection.h
    IQFeedInstrumentBuild.h
    IQFeedSymbolFileToSqlite.h
    IQFeedSymbolListOps.h
    LiborFromIQFeed.h
    MovingAverage.hpp
    ReadCboeWeeklyOptionsCsv.h
    ReadCboeWeeklyOptionsXls.h
    Stochastic.hpp
    TrackCurrencyOrder.hpp
    TradeState.hpp
    TreeOps.h
    TreeOpsItems.h
  )

set(
  file_cpp
#    CalcAboveBelow.cpp
    BollingerTransitions.cpp
    DailyHistory.cpp
    FrameWork01.cpp
    InstrumentSelection.cpp
    IQFeedInstrumentBuild.cpp
    IQFeedSymbolListOps.cpp
    LiborFromIQFeed.cpp
    MovingAverage.cpp
    ReadCboeWeeklyOptionsCsv.cpp
    ReadCboeWeeklyOptionsXls.cpp
    Stochastic.cpp
    TrackCurrencyOrder.cpp
    TradeState.cpp
    TreeOps.cpp
    TreeOpsItems.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
target_compile_definitions(${PROJECT_NAME} PUBLIC wxUSE_GUI )
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
    "/usr/local/lib/wx/include/gtk3-unicode-${WX_VERSION_MAJOR}"
    "/usr/local/include/wx-${WX_VERSION_MAJOR}"
  )

