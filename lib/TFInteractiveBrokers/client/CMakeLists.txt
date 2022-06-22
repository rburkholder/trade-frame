# trade-frame/lib/TFInteractiveBrokers/client
cmake_minimum_required (VERSION 3.13)

PROJECT(ib_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID, "x64")
#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    bar.h
    CommissionReport.h
    CommonDefs.h
    ContractCondition.h
    Contract.h
    Decimal.h
    DefaultEWrapper.h
    DepthMktDataDescription.h
    EClientException.h
    EClient.h
    EClientMsgSink.h
    EClientSocket.h
    EDecoder.h
    EMessage.h
    EMutex.h
    EOrderDecoder.h
    EPosixClientSocketPlatform.h
    EReader.h
    EReaderOSSignal.h
    EReaderSignal.h
    ESocket.h
    ETransport.h
    EWrapper.h
    EWrapper_prototypes.h
    executioncondition.h
    Execution.h
    FamilyCode.h
    HistogramEntry.h
    HistoricalTickBidAsk.h
    HistoricalTick.h
    HistoricalTickLast.h
    IExternalizable.h
    MarginCondition.h
    NewsProvider.h
    OperatorCondition.h
    OrderCondition.h
    Order.h
    OrderState.h
    PercentChangeCondition.h
    platformspecific.h
    PriceCondition.h
    PriceIncrement.h
    resource.h
    ScannerSubscription.h
    SoftDollarTier.h
    TagValue.h
    TickAttribBidAsk.h
    TickAttrib.h
    TickAttribLast.h
    TimeCondition.h
    TwsSocketClientErrors.h
    VolumeCondition.h
  )

set(
  file_cpp
    ContractCondition.cpp
    DefaultEWrapper.cpp
    EClient.cpp
    EClientSocket.cpp
    EDecoder.cpp
    EMessage.cpp
    EMutex.cpp
    EOrderDecoder.cpp
    EReader.cpp
    ESocket.cpp
    EReaderOSSignal.cpp
    executioncondition.cpp
    MarginCondition.cpp
    OperatorCondition.cpp
    OrderCondition.cpp
    PercentChangeCondition.cpp
    PriceCondition.cpp
    SoftDollarTier.cpp
    TimeCondition.cpp
    VolumeCondition.cpp
  )

add_library(
  ${PROJECT_NAME}
  ${file_h}
  ${file_cpp}
  )

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )
# need to figure out how to make this work
#add_compile_options(`/usr/local/bin/wx-config --cxxflags`)
# used in EventIBInstrument.cpp - redo this somehow?
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_FILE_OFFSET_BITS=64 )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DWXUSINGDLL )
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__WXGTK__ )

target_include_directories(
  ${PROJECT_NAME} PUBLIC
    ".."
    "/usr/local/lib/wx/include/gtk3-unicode-3.1"
    "/usr/local/include/wx-3.1"
  )

