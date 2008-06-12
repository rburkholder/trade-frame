#pragma once

#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "BarFactory.h"
#include "TimeSeries.h"
#include "ProviderInterface.h"
#include "Instrument.h"
#include "Order.h"
#include "OrderManager.h"

#include <sstream>

class CBasketTradeSymbolInfo {
public:
  explicit CBasketTradeSymbolInfo( 
    const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy,
    CProviderInterface *pExecutionProvider );
  explicit CBasketTradeSymbolInfo( std::stringstream *pStream, CProviderInterface *pExecutionProvider );
  ~CBasketTradeSymbolInfo( void );

  void CalculateTrade( ptime dtTradeDate, double dblFunds, bool bRTHOnly );
  double GetProposedEntryCost() { return m_dblProposedEntryCost; };
  int GetQuantityForEntry() { return m_nQuantityForEntry; };
  void HandleTrade( const CTrade &trade );
  void HandleOpen( const CTrade &trade );
  const std::string &GetSymbolName( void ) { return m_sSymbolName; };
  void StreamSymbolInfo( std::stringstream *pStream );

  struct structFieldsForDialog { // used for updating dialog
    std::string sSymbol;
    double dblHigh;
    double dblOpenRangeHigh;
    double dblOpen;
    double dblOpenRangeLow;
    double dblLow;
    double dblFilledPrice;
    double dblCurrentPrice;
    double dblStop;
    double dblUnRealizedPL;
    double dblRealizedPL;
    std::string sHit;  // 0/1 for long, 0/1 for short (two characters here)
  } ;
protected:
  void Initialize( void );
  std::string m_sSymbolName;
  std::string m_sPath;
  std::string m_sStrategy;
  CInstrument *m_pInstrument;
  ptime m_dtTradeDate;
  double m_dblMaxAllowedFunds;
  double m_dblDayOpenPrice;
  double m_dblPriceForEntry;
  double m_dblAveragePriceOfEntry;
  double m_dblMarketValueAtEntry;
  double m_dblCurrentMarketPrice;
  double m_dblCurrentMarketValue;
  int m_nQuantityForEntry;
  int m_nWorkingQuantity;
  double m_dblAllocatedWorkingFunds;
  double m_dblExitPrice;
  double m_dblProposedEntryCost;
  //bool m_bEntryInitiated;
  enum enumPositionState { Init, WaitingForOpen, 
    WaitingForThe3Bars, 
    WaitingForOrderFulfillmentLong, WaitingForOrderFulfillmentShort,
    WaitingForLongExit, WaitingForShortExit,
    WaitingForExit, Exited } m_PositionState;
  enum enumTradingState {
    WaitForOpeningTrade, 
    WaitForOpeningBell, // 9:30 exchange time
    SetOpeningRange,  // spend 5 minutes here
    ActiveTrading,  // through the day
    NoMoreTrades, // 15:40 exchange time
    CancelTrades, // 15:45 exchange time
    CloseTrades,  // 15:50 exchange time
    DoneTrading   // 15:50 exchange time
  } m_TradingState;
  ptime m_dtToday;
  bool m_bOpenFound;
  double m_dblOpen;
  double m_dblOpenLow;  // ACD Opening Range
  double m_dblOpenHigh; // ACD Opening Range
  double m_dblStop;
  double m_dblStartLevel;
  double m_dblAveBarHeight;
  double m_dblTrailingStopDistance;

  structFieldsForDialog m_FieldsForDialog;

  bool m_bRTHOnly;

  CProviderInterface *m_pExecutionProvider;

  CBarFactory m_1MinBarFactory;
  CBars m_bars;
  void HandleBarFactoryBar( const CBar &bar );

  void HandleOrderFilled( COrder *pOrder );
  bool m_bDoneTheLong, m_bDoneTheShort;

  COrderManager m_OrderManager;

private:
  CBasketTradeSymbolInfo( const CBasketTradeSymbolInfo & );  // disallow copy construction
};
