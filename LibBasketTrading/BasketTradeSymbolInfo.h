#pragma once

#include <string>
#include <sstream>
#include <map>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "BarFactory.h"
#include "TimeSeries.h"
#include "ProviderInterface.h"
#include "Instrument.h"
#include "Order.h"
#include "OrderManager.h"
#include "Delegate.h"


class CBasketTradeSymbolInfo {
public:
  explicit CBasketTradeSymbolInfo( 
    const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy,
    CProviderInterface *pExecutionProvider );
  explicit CBasketTradeSymbolInfo( std::stringstream *pStream, CProviderInterface *pExecutionProvider );
  ~CBasketTradeSymbolInfo( void );

  Delegate<CBasketTradeSymbolInfo *> OnBasketTradeSymbolInfoChanged;

  void CalculateTrade( ptime dtTradeDate, double dblFunds, bool bRTHOnly );
  double GetProposedEntryCost() { return m_dblProposedEntryCost; };
  int GetQuantityForEntry() { return m_nQuantityForEntry; };
  void HandleQuote( const CQuote &quote );
  void HandleTrade( const CTrade &trade );
  void HandleOpen( const CTrade &trade );
  const std::string &GetSymbolName( void ) { return m_status.sSymbolName; };
  void StreamSymbolInfo( std::ostream *pStream );
  void WriteTradesAndQuotes( const std::string &sPathPrefix );

  struct structFieldsForDialog { // used for updating dialog
    std::string sSymbolName;
    double dblCurrentPrice;
    double dblHigh;
    double dblOpenRangeHigh;  // ACD Opening Range
    double dblOpen;
    double dblOpenRangeLow;   // ACD Opening Range
    double dblLow;
    double dblFilledPrice;
    double dblStop;
    double dblUnRealizedPL;
    double dblRealizedPL;
    std::string sHit;  // 0/1 for long, 0/1 for short (two characters here)
    structFieldsForDialog( void ) : 
      dblHigh( 0 ), dblOpenRangeHigh( 0 ), dblOpen( 0 ),
      dblOpenRangeLow( 0 ), dblLow( 0 ), dblFilledPrice( 0 ), dblCurrentPrice( 0 ), dblStop( 0 ),
      dblUnRealizedPL( 0 ), dblRealizedPL( 0 ) {};
    structFieldsForDialog( const std::string &sSymbolName_ ) : sSymbolName( sSymbolName_ ),
      dblHigh( 0 ), dblOpenRangeHigh( 0 ), dblOpen( 0 ),
      dblOpenRangeLow( 0 ), dblLow( 0 ), dblFilledPrice( 0 ), dblCurrentPrice( 0 ), dblStop( 0 ),
      dblUnRealizedPL( 0 ), dblRealizedPL( 0 ) {};
  };
  const structFieldsForDialog &GetDialogFields( void ) { return m_status; };  // needs come after structure definition

protected:
  void Initialize( void );
  structFieldsForDialog m_status;
  std::string m_sPath;
  std::string m_sStrategy;
  ptime m_dtTimeMarker;
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
  enum enumPositionState { 
    Init, WaitingForOpen, 
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
  bool m_bOpenFound;
  double m_dblStartLevel;
  double m_dblAveBarHeight;
  double m_dblTrailingStopDistance;

  bool m_bDoneTheLong, m_bDoneTheShort;

  size_t m_nBarsInSequence;
  size_t m_nOpenCrossings;
  static const size_t m_nMaxCrossings = 2;  
  static const size_t m_nBarWidth = 30;  //seconds

  structFieldsForDialog m_FieldsForDialog;

  bool m_bRTHOnly;

  CProviderInterface *m_pExecutionProvider;

  CInstrument *m_pInstrument;
  COrderManager m_OrderManager;

  CBarFactory m_1MinBarFactory;
  CQuotes m_quotes;
  CTrades m_trades;
  CBars m_bars;

  void HandleBarFactoryBar( const CBar &bar );
  void HandleOrderFilled( COrder *pOrder );

  std::map<unsigned long, COrder*> m_mapActiveOrders;
  std::map<unsigned long, COrder*> m_mapCompletedOrders;

  int m_nCurrentPosition;


private:
  CBasketTradeSymbolInfo( const CBasketTradeSymbolInfo & );  // disallow copy construction
};
