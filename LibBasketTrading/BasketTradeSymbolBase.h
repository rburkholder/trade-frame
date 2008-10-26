#pragma once

#include <string>
#include <sstream>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "ProviderInterface.h"
#include "ChartRealTimeTreeView.h"
#include "Instrument.h"
#include "TimeSeries.h"
#include "ChartDataView.h"
#include "Order.h"
#include "OrderManager.h"

class CBasketTradeSymbolBase {
public:
  CBasketTradeSymbolBase( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );
  CBasketTradeSymbolBase( std::stringstream *pStream ); // probably isn't working properly, updated when streaming stuff implemented
  virtual ~CBasketTradeSymbolBase(void);
  void StreamSymbolInfo( std::ostream *pStream );

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
    int    nPositionSize; //+ or -
    double dblPositionSize;  
    double dblAverageCost;
    double dblUnRealizedPL;
    double dblRealizedPL;
    double dblRunningPL;
    double dblMaxRunningPL;
    double dblMinRunningPL;
    double dblAvgDailyRange;
    std::string sHit;  // 0/1 for long, 0/1 for short (two characters here)
    structFieldsForDialog( void ) : 
      dblHigh( 0 ), dblOpenRangeHigh( 0 ), dblOpen( 0 ),
      dblOpenRangeLow( 0 ), dblLow( 0 ), dblFilledPrice( 0 ), dblCurrentPrice( 0 ), dblStop( 0 ),
      nPositionSize( 0 ), dblPositionSize( 0 ), dblAverageCost( 0 ),
      dblUnRealizedPL( 0 ), dblRealizedPL( 0 ), dblAvgDailyRange( 0 ),
      dblRunningPL( 0 ), dblMaxRunningPL( 0 ), dblMinRunningPL( 0) {};
    structFieldsForDialog( const std::string &sSymbolName_ ) : sSymbolName( sSymbolName_ ),
      dblHigh( 0 ), dblOpenRangeHigh( 0 ), dblOpen( 0 ),
      dblOpenRangeLow( 0 ), dblLow( 0 ), dblFilledPrice( 0 ), dblCurrentPrice( 0 ), dblStop( 0 ),
      nPositionSize( 0 ), dblPositionSize( 0 ), dblAverageCost( 0 ),
      dblUnRealizedPL( 0 ), dblRealizedPL( 0 ), dblAvgDailyRange( 0 ),
      dblRunningPL( 0 ), dblMaxRunningPL( 0 ), dblMinRunningPL( 0 ) {};
  };

  struct structCommonModelInformation {
    enum enumCalcStep { Prelim, Final } nCalcStep;
    bool bRTH;  // regular trading hours only
    ptime dtRTHBgn;
    ptime dtRTHEnd;
    ptime dtOpenRangeBgn;
    ptime dtOpenRangeEnd;
    ptime dtEndActiveTrading;
    ptime dtBgnNoMoreTrades;
    ptime dtBgnCancelTrades;
    ptime dtBgnCloseTrades;
    ptime dtTradeDate; // date of trading, previous days provide the history
    double dblFunds;  // funds available for use
    CProviderInterface *pDataProvider;
    CProviderInterface *pExecutionProvider;
    CChartRealTimeTreeView *pTreeView;
  };

  const std::string &GetSymbolName( void ) { return m_status.sSymbolName; };
  structFieldsForDialog *GetDialogFields( void ) { return &m_status; };  // needs to come after structure definition
  double GetProposedEntryCost() { return m_dblProposedEntryCost; };

  void CalculateTrade( structCommonModelInformation *pParameters  );

  virtual void StartTrading( void ) {};
  virtual void StopTrading( void ) {};

  void AddTradeHandler( CSymbol::tradehandler_t handler );
  void AddQuoteHandler( CSymbol::quotehandler_t handler );
  void AddOpenHandler( CSymbol::tradehandler_t handler );
  void RemoveTradeHandler( CSymbol::tradehandler_t handler );
  void RemoveQuoteHandler( CSymbol::quotehandler_t handler );
  void RemoveOpenHandler( CSymbol::tradehandler_t handler );

  void PlaceOrder( COrder *pOrder );

protected:
  double m_dblProposedEntryCost;
  int m_nQuantityForEntry;


  structFieldsForDialog m_status;
  std::string m_sPath;
  std::string m_sStrategy;

  structFieldsForDialog m_FieldsForDialog;
  structCommonModelInformation *m_pModelParameters;

  CInstrument *m_pInstrument;
  COrderManager m_OrderManager;
  CChartDataView *m_pdvChart;

  void Initialize( void );
  virtual void ModelReady( CBars *pBars ) {};
  virtual void HandleOrderFilled( COrder *pOrder );

private:
  CBasketTradeSymbolBase( void );  // don't use a default constructor
};
