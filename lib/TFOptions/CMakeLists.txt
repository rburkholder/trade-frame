# trade-frame/lib/TFOptions
cmake_minimum_required (VERSION 3.13)

PROJECT(TFOptions)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    Aggregate.h
    Binomial.h
    Bundle.h
    CalcExpiry.h
    Chain.h
    Chains.h
    Engine.hpp
    Formula.h
    GatherOptions.h
    IvAtm.h
    Margin.h
    NoRiskInterestRateSeries.h
    Option.h
    OptionDelegates.hpp
    PopulateWithIBOptions.h
    Strike.h
  )

set(
  file_cpp
    Aggregate.cpp
    Binomial.cpp
    Bundle.cpp
    CalcExpiry.cpp
    Chain.cpp
    Chains.cpp
    Engine.cpp
    Formula.cpp
    IvAtm.cpp
    Margin.cpp
    NoRiskInterestRateSeries.cpp
    Option.cpp
    PopulateWithIBOptions.cpp
    Strike.cpp
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
