# trade-frame/lib/TFIndicators
cmake_minimum_required (VERSION 3.13)

PROJECT(TFIndicators)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
#    CalcAboveBelow.h
    Crossing.h
    Darvas.h
    PivotGroup.h
    Pivots.h
    RunningMinMax.h
    RunningStats.h
    SlidingWindow.h
    StatsInSlidingWindow.h
    TimeSeriesSlidingWindow.h
    TSDifferential.h
    TSEMA.h
    TSHomogenization.h
    TSMA.h
    TSNorm.h
    TSReturns.h
    TSSWDonchianChannel.h
    TSSWEfficiencyRatio.h
    TSSWMinMax.hpp
    TSSWRateOfChange.h
    TSSWRealizedVolatility.h
    TSSWRunningTally.h
    TSSWStats.h
    TSSWStochastic.h
    TSSWTickFrequency.h
    TSVariance.h
    TSVolatility.h
    ZigZag.h
  )

set(
  file_cpp
#    CalcAboveBelow.cpp
    Crossing.cpp
    PivotGroup.cpp
    Pivots.cpp
    RunningMinMax.cpp
    RunningStats.cpp
    SlidingWindow.cpp
    StatsInSlidingWindow.cpp
    TSDifferential.cpp
    TSEMA.cpp
    TSHomogenization.cpp
    TSMA.cpp
    TSNorm.cpp
    TSReturns.cpp
    TSSWDonchianChannel.cpp
    TSSWEfficiencyRatio.cpp
    TSSWMinMax.cpp
    TSSWRateOfChange.cpp
    TSSWRealizedVolatility.cpp
    TSSWRunningTally.cpp
    TSSWStats.cpp
    TSSWStochastic.cpp
    TSSWTickFrequency.cpp
    TSVariance.cpp
    TSVolatility.cpp
    ZigZag.cpp
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
