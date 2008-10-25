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

class CBasketTradeSymbolBase {
public:
  CBasketTradeSymbolBase( void ); // probably isn't working properly, updated when streaming stuff implemented
  CBasketTradeSymbolBase( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );
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

protected:
  structFieldsForDialog m_status;
  std::string m_sPath;
  std::string m_sStrategy;

  structFieldsForDialog m_FieldsForDialog;
  structCommonModelInformation *m_pModelParameters;

  CInstrument *m_pInstrument;

  CChartDataView *m_pdvChart;

  void Initialize( void );

private:
};
