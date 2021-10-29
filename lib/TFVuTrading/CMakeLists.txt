# trade-frame/lib/TFVuTrading
cmake_minimum_required (VERSION 3.13)

PROJECT(TFVuTrading)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(MarketDepth)

set(
  file_h
    ControllerPortfolioPositionOrderExecution.h
    DataViewItem.h
    DialogBase.h
    DialogInstrumentSelect.h
    DialogNewPortfolio.h
    DialogPickSymbol.h
    DialogSimpleOneLineOrder.h
    DragDropInstrument.h
    DragDropInstrumentTarget.h
    EventDrawChart.h
    FrameMain.h
    FrameOrderEntry.h
    GridIBAccountValues.h
    GridIBAccountValues_impl.h
    GridIBPositionDetails.h
    GridIBPositionDetails_impl.h
    GridOptionChain.h
    GridOptionChain_impl.h
    InterfaceBoundEvents.h
    ModelBase.h
    ModelCell.h
    ModelCell_macros.h
    ModelCell_ops.h
    ModelChartHdf5.h
    ModelExecution.h
    ModelOrder.h
    ModelPortfolio.h
    ModelPortfolioPositionOrderExecution.h
    ModelPosition.h
    NotebookOptionChains.h
    PanelChartHdf5.h
    PanelFinancialChart.h
    PanelLogging.h
    PanelManualOrder.h
    PanelOptionCombo.h
    PanelOptionCombo_impl.h
    PanelPortfolioPosition.h
    PanelPortfolioPosition_impl.h
    PanelPortfolioPositionOrderExecution.h
    PanelProviderControl.h
    ValidatorInstrumentName.h
#    VisibleItemAttributes.h
    VisibleItemInDevContext.h
    VuBase.h
    VuExecutions.h
    VuOrders.h
    VuPortfolios.h
    VuPositions.h
    VuTreePortfolioPositionOrder.h
    WinChartView.h
    wxETKBaseValidator.h
  )

set(
  file_cpp
    ControllerPortfolioPositionOrderExecution.cpp
    DialogBase.cpp
    DialogInstrumentSelect.cpp
    DialogNewPortfolio.cpp
    DialogPickSymbol.cpp
    DialogSimpleOneLineOrder.cpp
    DragDropInstrument.cpp
    DragDropInstrumentTarget.cpp
    EventDrawChart.cpp
    FrameMain.cpp
    FrameOrderEntry.cpp
    GridIBAccountValues.cpp
    GridIBAccountValues_impl.cpp
    GridIBPositionDetails.cpp
    GridIBPositionDetails_impl.cpp
    GridOptionChain.cpp
    GridOptionChain_impl.cpp
    InterfaceBoundEvents.cpp
    ModelBase.cpp
    ModelCell.cpp
    ModelChartHdf5.cpp
    ModelExecution.cpp
    ModelOrder.cpp
    ModelPortfolio.cpp
    ModelPortfolioPositionOrderExecution.cpp
    ModelPosition.cpp
    NotebookOptionChains.cpp
    PanelChartHdf5.cpp
    PanelFinancialChart.cpp
    PanelLogging.cpp
    PanelManualOrder.cpp
    PanelOptionCombo.cpp
    PanelOptionCombo_impl.cpp
    PanelPortfolioPosition.cpp
    PanelPortfolioPosition_impl.cpp
    PanelPortfolioPositionOrderExecution.cpp
    PanelProviderControl.cpp
    ValidatorInstrumentName.cpp
#    VisibleItemAttributes.cpp
    VuBase.cpp
    VuExecutions.cpp
    VuOrders.cpp
    VuPortfolios.cpp
    VuPositions.cpp
    VuTreePortfolioPositionOrder.cpp
    WinChartView.cpp
    wxETKBaseValidator.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
# need to figure out how to make this work
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
    "/usr/local/lib/wx/include/gtk3-unicode-3.0"
    "/usr/local/include/wx-3.0"
  )

