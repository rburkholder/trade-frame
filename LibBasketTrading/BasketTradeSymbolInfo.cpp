#include "StdAfx.h"

#include "BasketTradeSymbolInfo.h"

#include "TimeSeries.h"
#include "HDF5TimeSeriesContainer.h"
#include "InstrumentFile.h"

// 
// CSymbolInfo
//

CBasketTradeSymbolInfo::CBasketTradeSymbolInfo( 
  const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy,
  CProviderInterface *pExecutionProvider
  ) 
: m_sSymbolName( sSymbolName ), m_sPath( sPath ),  m_sStrategy( sStrategy ),
  m_pExecutionProvider( pExecutionProvider ),
  m_dtToday( not_a_date_time ), 
  m_bOpenFound( false ), m_dblOpen( 0 ), m_dblOpenLow( 0 ), m_dblOpenHigh( 0 ),
  m_dblAveBarHeight( 0 ), m_dblTrailingStopDistance( 0 ),
  m_PositionState( Init ), m_TradingState( WaitForOpeningTrade ),
  m_bDoneTheLong( false ), m_bDoneTheShort( false ), m_bRTHOnly( true )
{
  Initialize();
}

CBasketTradeSymbolInfo::CBasketTradeSymbolInfo( std::stringstream *pStream, CProviderInterface *pExecutionProvider )
: m_pExecutionProvider( pExecutionProvider ),
  m_dtToday( not_a_date_time ), 
  m_bOpenFound( false ), m_dblOpen( 0 ), m_dblOpenLow( 0 ), m_dblOpenHigh( 0 ),
  m_dblAveBarHeight( 0 ), m_dblTrailingStopDistance( 0 ),
  m_PositionState( Init ), m_TradingState( WaitForOpeningTrade ),
  m_bDoneTheLong( false ), m_bDoneTheShort( false ), m_bRTHOnly( true )
{
  *pStream >> m_sSymbolName >> m_sPath >> m_sStrategy;
  Initialize();
}

CBasketTradeSymbolInfo::~CBasketTradeSymbolInfo( void ) {
  if ( NULL != m_pInstrument ) {
    delete m_pInstrument;
    m_pInstrument = NULL;
  }
}

void CBasketTradeSymbolInfo::Initialize( void ) {
  m_1MinBarFactory.SetBarWidth( 30 );  // 30 seconds, 10 bars in 5 minutes, 5 bars for A confirmation
  m_1MinBarFactory.SetOnBarComplete( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleBarFactoryBar ) );

  CInstrumentFile file;
  file.OpenIQFSymbols();
  try {
    m_pInstrument = file.CreateInstrumentFromIQFeed( m_sSymbolName, m_sSymbolName );  // todo:  need to verify proper symbol usage
  }
  catch (...) {
    std::cout << "CBasketTradeSymbolInfo::CBasketTradeSymbolInfo problems" << std::endl;
  }
  file.CloseIQFSymbols();
}

void CBasketTradeSymbolInfo::StreamSymbolInfo(std::ostream *pStream) {
  *pStream << m_sSymbolName << "," << m_sPath << "," << m_sStrategy;
  //int i = pStream->gcount();
  //pStream->rdbuf()->pubsync
}

void CBasketTradeSymbolInfo::CalculateTrade(ptime dtTradeDate, double dblFunds, bool bRTHOnly) {

  m_bRTHOnly = bRTHOnly;
  m_dtTradeDate = dtTradeDate;
  m_dblMaxAllowedFunds = dblFunds;
  CHDF5TimeSeriesContainer<CBar> barRepository( m_sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dt3MonthsAgo = dtTradeDate - date_duration( 365 / 4 );
  ptime dt6MonthsAgo = dtTradeDate - date_duration( 365 / 2 );
  begin = lower_bound( barRepository.begin(), barRepository.end(), dt6MonthsAgo );
  end = lower_bound( begin, barRepository.end(), dtTradeDate );
  hsize_t cnt = end - begin;
  CBars bars;
  bars.Resize( cnt );
  barRepository.Read( begin, end, &bars );

  std::cout << "Entry for " << m_sSymbolName;
  if ( 20 < cnt ) {
    double range = 0;
    CBar *pBar;
    for ( int ix = cnt - 20; ix < cnt; ++ix ) {
      pBar = bars[ ix ];
      range += pBar->m_dblHigh - pBar->m_dblLow;
    }
    m_dblStartLevel = ( range / cnt ) * 0.05;

    double dblClose = bars.Last()->m_dblClose;
    m_nQuantityForEntry = ( ( (int) ( dblFunds / dblClose ) ) / 100 ) * 100;
    m_dblProposedEntryCost = m_nQuantityForEntry * bars.Last()->m_dblClose;
    std::cout  
      << ": " << m_nQuantityForEntry << "@" << m_dblProposedEntryCost 
      << ", " << dblClose
      << std::endl;
  }
  else {
    m_nQuantityForEntry = m_dblProposedEntryCost = 0;
    std::cout << m_sSymbolName << " didn't have enough bars" << std::endl;
  }
}

void CBasketTradeSymbolInfo::HandleTrade(const CTrade &trade) {
  //std::cout << "Trade " << m_sSymbolName << ", " << trade.m_nTradeSize << "@" << trade.m_dblTrade << std::endl;
  switch ( m_TradingState ) {
    case WaitForOpeningTrade:
      if ( m_bRTHOnly ) {
        m_dtToday = ptime( trade.m_dt.date(), time_duration( 10, 30, 0 ) );
        m_TradingState = WaitForOpeningBell;
        std::cout << m_sSymbolName << " Entering WaitForOpeningBell" << std::endl;
      }
      else {
        m_dtToday = ptime( trade.m_dt.date(), time_duration( 16, 40, 0 ) );
        m_TradingState = ActiveTrading;
        std::cout << m_sSymbolName << " Entering ActiveTrading" << std::endl;  // need to fix how this operates without RTHOnly set
        if ( !m_bOpenFound ) {
          m_bOpenFound = true;
          m_dblOpen = m_dblOpenLow = m_dblOpenHigh = trade.m_dblTrade;
        }
      }
      break;
    case WaitForOpeningBell:
      if ( trade.m_dt >= m_dtToday ) {
        if ( !m_bOpenFound ) {
          m_bOpenFound = true;
          m_dblOpen = m_dblOpenLow = m_dblOpenHigh = trade.m_dblTrade;
        }
        m_dtToday = ptime( trade.m_dt.date(), time_duration( 10, 35, 0 ) );
        m_TradingState = SetOpeningRange;
        std::cout << m_sSymbolName << " Entering SetOpeningRange" << std::endl;
      }
      break;
    case SetOpeningRange:
      if ( trade.m_dt >= m_dtToday ) {
        m_dtToday = ptime( trade.m_dt.date(), time_duration( 16, 40, 0 ) );
        m_TradingState = ActiveTrading;
        std::cout << m_sSymbolName << " Entering ActiveTrading" << std::endl;
      }
      else {
        if ( trade.m_dblTrade > m_dblOpenHigh ) m_dblOpenHigh = trade.m_dblTrade;
        if ( trade.m_dblTrade < m_dblOpenLow ) m_dblOpenLow = trade.m_dblTrade;
      }
      break;
    case ActiveTrading:
      if ( trade.m_dt >= m_dtToday ) {
        m_dtToday = ptime( trade.m_dt.date(), time_duration( 16, 45, 0 ) );
        m_TradingState = NoMoreTrades;
        std::cout << m_sSymbolName << " Entering NoMoreTrades" << std::endl;
      }
      else {
        // these states are here in order to handle stops at the trade level rather than bar level
        switch ( m_PositionState ) {
          case WaitingForLongExit:
            if ( trade.m_dblTrade < m_dblStop ) {
              m_PositionState = WaitingForThe3Bars;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_nQuantityForEntry );
              m_OrderManager.PlaceOrder( m_pExecutionProvider, pOrder );
            }
            break;
          case WaitingForShortExit:
            if ( trade.m_dblTrade > m_dblStop ) {
              m_PositionState = WaitingForThe3Bars;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, m_nQuantityForEntry );
              m_OrderManager.PlaceOrder( m_pExecutionProvider, pOrder );
            }
            break;
          default:
            // all other states handled in bar mode
            break;
        }
      }
      break;
    case NoMoreTrades:
      if ( trade.m_dt >= m_dtToday ) {
        m_dtToday = ptime( trade.m_dt.date(), time_duration( 16, 50, 0 ) );
        m_TradingState = CancelTrades;
        std::cout << m_sSymbolName << " Entering CancelTrades" << std::endl;
      }
      else {
      }
      break;
    case CancelTrades:
      if ( trade.m_dt >= m_dtToday ) {
        m_dtToday = ptime( trade.m_dt.date(), time_duration( 16, 55, 0 ) );
        m_TradingState = CloseTrades;
        std::cout << m_sSymbolName << " Entering CloseTrades" << std::endl;
      }
      else {
      }
      break;
    case CloseTrades:
      if ( trade.m_dt >= m_dtToday ) {
        m_TradingState = DoneTrading;
        std::cout << m_sSymbolName << " Entering DoneTrading" << std::endl;
      }
      else {
      }
      break;
    case DoneTrading:
      // do nothing
      break;
  }

  m_1MinBarFactory.Add( trade );

}

void CBasketTradeSymbolInfo::HandleBarFactoryBar(const CBar &bar) {
  double dblTemp = 0;
  static double dblPercentage = 2.0 / ( 10 + 1 );  // Exponential Moving Average on 10 bars (30 sec bars in 5 minutes)
  m_dblAveBarHeight = // 10 bar ema used for calculating A
    ( dblPercentage * ( bar.m_dblHigh - bar.m_dblLow ) ) + ( ( 1 - dblPercentage ) * m_dblAveBarHeight );
  m_bars.AppendDatum( bar );
  size_t cnt = m_bars.Count();
  switch ( m_PositionState ) {
    case Init:
      if ( 0 == m_nQuantityForEntry ) {
        m_PositionState = Exited;
      }
      else {
        m_PositionState = WaitingForThe3Bars;
      }
      break;  // some strategies may require a fall through here
      //case WaitingForOpen:
      //  break;
    case WaitingForThe3Bars:
      if ( m_bDoneTheLong && m_bDoneTheShort ) {
        m_PositionState = Exited;  // don't do any more trading
        break;
      }
      if ( ( cnt >= 5 ) && ( 0 < m_dblOpen ) && ( ActiveTrading == m_TradingState ) ) {
        CBar bar1, bar2, bar3, bar4, bar5;
        bar1 = *m_bars[ cnt - 5 ];
        bar2 = *m_bars[ cnt - 4 ];
        bar3 = *m_bars[ cnt - 3 ];
        bar4 = *m_bars[ cnt - 2 ];
        bar5 = *m_bars[ cnt - 1 ];
        if ( !m_bDoneTheLong ) {
          double dblAboveThisLevel = m_dblOpenHigh + m_dblAveBarHeight;
          if ( ( bar1.m_dblLow > dblAboveThisLevel ) 
            && ( bar2.m_dblLow > dblAboveThisLevel ) 
            && ( bar3.m_dblLow > dblAboveThisLevel ) 
            && ( bar4.m_dblLow > dblAboveThisLevel ) 
            && ( bar5.m_dblLow > dblAboveThisLevel ) ) {
              m_dblStop = m_dblOpenLow;
              m_dblTrailingStopDistance = bar.m_dblClose - m_dblOpenLow;
              std::cout << m_sSymbolName << " Enter LONG now: " << bar.m_dblClose << ", Stop at " << m_dblStop << std::endl;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, m_nQuantityForEntry );
              pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_PositionState = WaitingForOrderFulfillmentLong;
              m_bDoneTheLong = true;
              m_OrderManager.PlaceOrder( m_pExecutionProvider, pOrder );
          }
        }
        if ( !m_bDoneTheShort ) {
          double dblBelowThisLevel = m_dblOpenLow - m_dblAveBarHeight;
          if ( ( bar1.m_dblHigh < dblBelowThisLevel ) 
            && ( bar2.m_dblHigh < dblBelowThisLevel ) 
            && ( bar3.m_dblHigh < dblBelowThisLevel ) 
            && ( bar4.m_dblHigh < dblBelowThisLevel ) 
            && ( bar5.m_dblHigh < dblBelowThisLevel ) ) {
              m_dblStop = m_dblOpenHigh;
              m_dblTrailingStopDistance = m_dblOpenHigh - bar.m_dblClose;
              std::cout << " Enter SHORT Now: " << m_sSymbolName << ", Stop at " << m_dblStop << std::endl;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_nQuantityForEntry );
              pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_PositionState = WaitingForOrderFulfillmentShort;
              m_bDoneTheShort = true;
              m_OrderManager.PlaceOrder( m_pExecutionProvider, pOrder );
            }
        }
      }
      break;
    case WaitingForOrderFulfillmentLong:
    case WaitingForOrderFulfillmentShort:
      // state will exit within HandleOrderFilled
      break;
    case WaitingForLongExit:   // adjust stop after each bar
      dblTemp = bar.m_dblClose - m_dblTrailingStopDistance;
      if ( dblTemp > m_dblStop ) {
        m_dblStop = dblTemp;
      }
      break;
    case WaitingForShortExit:   // adjust stop after each bar
      dblTemp = bar.m_dblClose + m_dblTrailingStopDistance;
      if ( dblTemp < m_dblStop ) {
        m_dblStop = dblTemp;
      }
      break;
    case Exited:
      break;
    default:
      // some states handled elsewhere
      break;
  }
}

void CBasketTradeSymbolInfo::HandleOrderFilled(COrder *pOrder) {
  // make the assumption that the order arriving is the order we are expecting, ie no multiple or cancelled orders
  pOrder->OnOrderFilled.Remove( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
  switch ( m_PositionState ) {
    case WaitingForOrderFulfillmentLong:
      m_PositionState = WaitingForLongExit;
      break;
    case WaitingForOrderFulfillmentShort:
      m_PositionState = WaitingForShortExit;
      break;
    default:
      std::cout << "We've got problems" << std::endl;
  }
}

void CBasketTradeSymbolInfo::HandleOpen( const CTrade &trade ) {
}