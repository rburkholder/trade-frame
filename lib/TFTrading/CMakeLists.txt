# trade-frame/lib/TFTrading
cmake_minimum_required (VERSION 3.13)

PROJECT(TFTrading)

#set(CMAKE_EXE_LINKER_FLAGS "--trace --verbose")
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(
  file_h
    AccountAdvisor.h
    Account.h
    AccountManager.h
    AccountOwner.h
    AcquireFundamentals.h
    AlternateInstrumentNames.h
    BuildInstrument.hpp
    CashAccount.h
    CashManager.h
    CashTransaction.h
    ComposeInstrument.hpp
    Currency.h
    DailyTradeTimeFrames.h
    Database.h
    DBOps.h
    DBWrapper.h
    Exchange.h
    Execution.h
    InstrumentData.h
    Instrument.h
#    InstrumentInformation.h
    InstrumentManager.h
    KeyTypes.h
    MacroStrand.h
    Managers.h
    MonitorOrder.h
    Order.h
    Order_Combo.hpp
    Order_Bracket.hpp
    OrderManager.h
    OrdersOutstanding.h
    PortfolioGreek.h
    Portfolio.h
    PortfolioManager.h
    PositionGreek.h
    Position.h
    ProviderInterface.h
    ProviderManager.h
    RiskManager.h
    SpreadCandidate.h
    SpreadValidation.h
    Symbol.h
    TradingEnumerations.h
    Watch.h
  )

set(
  file_cpp
    AccountAdvisor.cpp
    Account.cpp
    AccountManager.cpp
    AccountOwner.cpp
    AcquireFundamentals.cpp
    AlternateInstrumentNames.cpp
    BuildInstrument.cpp
    CashAccount.cpp
    CashManager.cpp
    CashTransaction.cpp
    ComposeInstrument.cpp
    Currency.cpp
    DailyTradeTimeFrames.cpp
    Database.cpp
    DBOps.cpp
    DBWrapper.cpp
    Exchange.cpp
    Execution.cpp
    Instrument.cpp
    InstrumentData.cpp
#    InstrumentInformation.cpp
    InstrumentManager.cpp
    KeyTypes.cpp
    Managers.cpp
    MonitorOrder.cpp
    Order.cpp
    Order_Combo.cpp
    Order_Bracket.cpp
    OrderManager.cpp
    OrdersOutstanding.cpp
    Portfolio.cpp
    PortfolioGreek.cpp
    PortfolioManager.cpp
    Position.cpp
    PositionGreek.cpp
    ProviderManager.cpp
    RiskManager.cpp
    SpreadCandidate.cpp
    SpreadValidation.cpp
    Symbol.cpp
    TradingEnumerations.cpp
    Watch.cpp
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

target_link_libraries(
  ${PROJECT_NAME}
    TFInteractiveBrokers
    TFSimulation
    TFIQFeed
    TFOptions
    OUSQL
    OUSqlite
    OUCommon
  )

