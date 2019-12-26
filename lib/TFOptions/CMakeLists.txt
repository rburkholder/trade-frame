# trade-frame/lib/TFOptions
cmake_minimum_required (VERSION 3.13)

PROJECT(TFOptions)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    BearCallSpread.h
    Binomial.h
    BullPutSpread.h
    Bundle.h
    CalcExpiry.h
    CallBackSpread.h
    Chain.h
    Combo.h
    Condor.h
    Engine.h
    Exceptions.h
    Formula.h
    IvAtm.h
    LegDef.h
    Leg.h
    Margin.h
    NoRiskInterestRateSeries.h
    Option.h
    PopulateWithIBOptions.h
    PutBackSpread.h
    Straddle.h
    Strangle.h
    Strike.h
    SyntheticLong.h
    SyntheticShort.h
    VerticalSpread.h
  )

set(
  file_cpp
    BearCallSpread.cpp
    Binomial.cpp
    BullPutSpread.cpp
    Bundle.cpp
    CalcExpiry.cpp
    CallBackSpread.cpp
    Chain.cpp
    Combo.cpp
    Condor.cpp
    Engine.cpp
    Formula.cpp
    IvAtm.cpp
    Leg.cpp
    Margin.cpp
    NoRiskInterestRateSeries.cpp
    Option.cpp
    PopulateWithIBOptions.cpp
    PutBackSpread.cpp
    Straddle.cpp
    Strangle.cpp
    Strike.cpp
    SyntheticLong.cpp
    SyntheticShort.cpp
    VerticalSpread.cpp
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
