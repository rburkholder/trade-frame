# trade-frame/lib/TFOptionCombols
cmake_minimum_required (VERSION 3.13)

PROJECT(TFOptionCombos)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    BackSpread.hpp
    Collar.hpp
    Combo.hpp
    ComboTraits.hpp
#    Condor.h
    Leg.h
    LegDef.h
    LegNote.h
    SpreadSpecs.h
#    Straddle.h
#    Strangle.h
#    SyntheticLong.h
#    SyntheticShort.h
    RiskReversal.hpp
    Tracker.h
    VerticalSpread.hpp
  )

set(
  file_cpp
    BackSpread.cpp
    Collar.cpp
    Combo.cpp
#    Condor.cpp
    Leg.cpp
    LegNote.cpp
    SpreadSpecs.cpp
#    Straddle.cpp
#    Strangle.cpp
#    SyntheticLong.cpp
#    SyntheticShort.cpp
    RiskReversal.cpp
    Tracker.cpp
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
