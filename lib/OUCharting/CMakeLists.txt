# trade-frame/lib/OUCharting
cmake_minimum_required(VERSION 3.13)

PROJECT(OUCharting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
#    ChartControls.h
    ChartDataView.h
    ChartDVBasics.h
    ChartEntryBars.h
    ChartEntryBase.h
    ChartEntryIndicator.h
    ChartEntryMark.h
    ChartEntryPrice.h
    ChartEntrySegments.h
    ChartEntryShape.h
    ChartEntryVolume.h
#    ChartingContainer.h
#    ChartInstrumentTree.h
    ChartMaster.h
#    ChartRealTimeContainer.h
#    ChartRealTimeController.h
#    ChartRealTimeModel.h
#    ChartRealTimeTreeView.h
#    ChartRealTimeView.h
#    ChartViewPort.h
    ChartDirector64/bchartdir.h
    ChartDirector64/chartdir.h
    ChartDirector64/FinanceChart.h
    ChartDirector64/memblock.h
  )

set(
  file_cpp
#    ChartControls.cpp
    ChartDataView.cpp
    ChartDVBasics.cpp
    ChartEntryBars.cpp
    ChartEntryBase.cpp
    ChartEntryIndicator.cpp
    ChartEntryMark.cpp
    ChartEntryPrice.cpp
    ChartEntrySegments.cpp
    ChartEntryShape.cpp
    ChartEntryVolume.cpp
#    ChartingContainer.cpp
#    ChartInstrumentTree.cpp
    ChartMaster.cpp
#    ChartRealTimeContainer.cpp
#    ChartRealTimeController.cpp
#    ChartRealTimeModel.cpp
#    ChartRealTimeTreeView.cpp
#    ChartRealTimeView.cpp
#    ChartViewPort.cpp
  )

add_library(
  ${PROJECT_NAME} STATIC
  ${file_h}
  ${file_cpp}
  )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
  )

