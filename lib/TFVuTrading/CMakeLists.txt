# trade-frame/lib/TFVuTrading
cmake_minimum_required (VERSION 3.13)

PROJECT(TFVuTrading)

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
    FrameControls.h
    FrameMain.h
    FrameOrderEntry.h
    GridColumnSizer.h
    GridIBAccountValues.h
    GridIBAccountValues_impl.h
    GridIBPositionDetails.h
    GridIBPositionDetails_impl.h
    GridOptionChain.hpp
    GridOptionChain_impl.hpp
    GridOptionComboOrder.hpp
    GridOptionComboOrder_impl.hpp
    InterfaceBookOptionChain.hpp
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
    Mouse.hpp
    NotebookOptionChains.h
    PanelChartHdf5.hpp
    PanelComboOrder.hpp
    PanelDividenNotes.hpp
    PanelFinancialChart.hpp
    PanelLogging.h
    PanelManualOrder.h
    PanelOptionCombo.h
    PanelOptionCombo_impl.h
    PanelOrderButtons.h
    PanelOrderButtons_structs.h
    PanelPortfolioPosition.h
    PanelPortfolioPosition_impl.h
    PanelPortfolioPositionOrderExecution.h
    PanelProviderControl.h
    PanelProviderControlv2.hpp
    TreeItem.hpp
    TreeItem_ItemData.hpp
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
    FrameControls.cpp
    FrameMain.cpp
    FrameOrderEntry.cpp
    GridColumnSizer.cpp
    GridIBAccountValues.cpp
    GridIBAccountValues_impl.cpp
    GridIBPositionDetails.cpp
    GridIBPositionDetails_impl.cpp
    GridOptionChain.cpp
    GridOptionChain_impl.cpp
    GridOptionComboOrder.cpp
    GridOptionComboOrder_impl.cpp
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
    PanelComboOrder.cpp
    PanelDividenNotes.cpp
    PanelFinancialChart.cpp
    PanelLogging.cpp
    PanelManualOrder.cpp
    PanelOptionCombo.cpp
    PanelOptionCombo_impl.cpp
    PanelOrderButtons.cpp
    PanelOrderButtons_structs.cpp
    PanelPortfolioPosition.cpp
    PanelPortfolioPosition_impl.cpp
    PanelPortfolioPositionOrderExecution.cpp
    PanelProviderControl.cpp
    PanelProviderControlv2.cpp
    TreeItem.cpp
    ValidatorInstrumentName.cpp
#    VisibleItemAttributes.cpp
    VuBase.cpp
    VuExecutions.cpp
    VuOrders.cpp
    VuPortfolios.cpp
    VuPositions.cpp
    VuTreePortfolioPositionOrder.cpp
    WinChartView.cpp
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
    "/usr/local/lib/wx/include/gtk3-unicode-${WX_VERSION_MAJOR}"
    "/usr/local/include/wx-${WX_VERSION_MAJOR}"
  )

