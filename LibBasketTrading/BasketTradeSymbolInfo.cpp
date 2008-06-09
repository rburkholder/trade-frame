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
  m_dtToday( not_a_date_time ), m_dblOpen( 0 ), m_bOpenFound( false ), m_PositionState( Init ),
  m_pExecutionProvider( pExecutionProvider ),
  m_bDoneTheLong( false ), m_bDoneTheShort( false )
{
  m_1MinBarFactory.SetBarWidth( 60 );
  m_1MinBarFactory.SetOnBarComplete( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleBarFactoryBar ) );

  CInstrumentFile file;
  file.OpenIQFSymbols();
  try {
    m_pInstrument = file.CreateInstrumentFromIQFeed( sSymbolName, sSymbolName );  // todo:  need to verify proper symbol usage
  }
  catch (...) {
    std::cout << "CBasketTradeSymbolInfo::CBasketTradeSymbolInfo problems" << std::endl;
  }
  file.CloseIQFSymbols();

  //m_pInstrument = new CInstrument( sSymbolName, InstrumentType::Stock );
}

CBasketTradeSymbolInfo::~CBasketTradeSymbolInfo( void ) {
  if ( NULL != m_pInstrument ) {
    delete m_pInstrument;
    m_pInstrument = NULL;
  }
}

void CBasketTradeSymbolInfo::CalculateTrade(ptime dtTradeDate, double dblFunds) {
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
  //CBar bar = *bars.Last();
  double dblClose = bars.Last()->m_dblClose;
  m_nQuantityForEntry = ( ( (int) ( dblFunds / dblClose ) ) / 100 ) * 100;
  m_dblProposedEntryCost = m_nQuantityForEntry * bars.Last()->m_dblClose;
  std::cout << "Entry for " << m_sSymbolName 
    << ": " << m_nQuantityForEntry << "@" << m_dblProposedEntryCost 
    << ", " << dblClose
    << std::endl;
}

void CBasketTradeSymbolInfo::HandleTrade(const CTrade &trade) {
  if ( m_dtToday.is_not_a_date_time() ) {
    m_dtToday = ptime( trade.m_dt.date(), time_duration( 10, 30, 0 ) );
  }
  if ( trade.m_dt >= m_dtToday ) {
    if ( !m_bOpenFound ) {
      m_bOpenFound = true;
      m_dblOpen = trade.m_dblTrade;
    }
    m_1MinBarFactory.Add( trade );
  }
}

void CBasketTradeSymbolInfo::HandleBarFactoryBar(const CBar &bar) {
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
      break;
      //case WaitingForOpen:
      //  break;
    case WaitingForThe3Bars:
      if ( m_bDoneTheLong && m_bDoneTheShort ) {
        m_PositionState = Exited;  // don't do any more trading
        break;
      }
      if ( cnt >= 3 ) {
        CBar bar1, bar2, bar3;
        bar1 = *m_bars[ cnt - 3 ];
        bar2 = *m_bars[ cnt - 2 ];
        bar3 = *m_bars[ cnt - 1 ];
        if ( !m_bDoneTheLong ) {
          if ( ( bar1.m_dblLow > m_dblOpen ) && ( bar2.m_dblLow > m_dblOpen ) && ( bar3.m_dblLow > m_dblOpen ) ) {
            if ( ( bar1.m_dblClose < bar2.m_dblClose ) && ( bar2.m_dblClose < bar3.m_dblClose ) ) {
              // do a market long on three rising bars
              m_dblStop = bar1.m_dblLow;
              m_dblStop = min( m_dblStop, bar2.m_dblLow );
              m_dblStop = min( m_dblStop, bar3.m_dblLow );
              std::cout << "Enter LONG now:  " << m_sSymbolName << ", Stop at " << m_dblStop << std::endl;
              COrder order( m_pInstrument, OrderType::Market, OrderSide::Buy, m_nQuantityForEntry );
              order.OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_pExecutionProvider->PlaceOrder( &order );  // need to keep the order around somewhere
              m_PositionState = WaitingForOrderFulfillmentLong;
            }
          }
        }
        if ( !m_bDoneTheShort ) {
          if ( ( bar1.m_dblHigh < m_dblOpen ) && ( bar2.m_dblHigh < m_dblOpen ) && ( bar3.m_dblHigh < m_dblOpen ) ) {
            if ( ( bar1.m_dblClose > bar2.m_dblClose ) && ( bar2.m_dblClose > bar3.m_dblClose ) ) {
              // do a market shrot on three falling bars
              m_dblStop = bar1.m_dblHigh;
              m_dblStop = max( m_dblStop, bar2.m_dblHigh );
              m_dblStop = max( m_dblStop, bar3.m_dblHigh );
              std::cout << "Enter SHORT Now: " << m_sSymbolName << ", Stop at " << m_dblStop << std::endl;
              COrder order( m_pInstrument, OrderType::Market, OrderSide::Sell, m_nQuantityForEntry );
              order.OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_pExecutionProvider->PlaceOrder( &order ); // need to keep the order around somewhere
              m_PositionState = WaitingForOrderFulfillmentShort;
            }
          }
        }
      }
      break;
    case WaitingForOrderFulfillmentLong:
    case WaitingForOrderFulfillmentShort:
      // state will exit within HandleOrderFilled
      break;
    case WaitingForLongExit:
      if ( bar.m_dblClose < m_dblStop ) {
        COrder order( m_pInstrument, OrderType::Market, OrderSide::Buy, m_nQuantityForEntry );
        m_pExecutionProvider->PlaceOrder( &order );
        m_PositionState = WaitingForThe3Bars;
      }
      break;
    case WaitingForShortExit:
      if ( bar.m_dblClose > m_dblStop ) {
        COrder order( m_pInstrument, OrderType::Market, OrderSide::Sell, m_nQuantityForEntry );
        m_pExecutionProvider->PlaceOrder( &order );
        m_PositionState = WaitingForThe3Bars;
      }
      break;
    case Exited:
      break;
  }
}

void CBasketTradeSymbolInfo::HandleOrderFilled(COrder *pOrder) {
  // make the assumption that the order arriving is the order we are expecting, ie no multiple or cancelled orders
  switch ( m_PositionState ) {
    case WaitingForOrderFulfillmentLong:
      m_PositionState = WaitingForLongExit;
      m_bDoneTheLong = true;
      break;
    case WaitingForOrderFulfillmentShort:
      m_PositionState = WaitingForShortExit;
      m_bDoneTheShort = true;
      break;
    default:
      std::cout << "We've got problems" << std::endl;
  }
}

void CBasketTradeSymbolInfo::HandleOpen( const CTrade &trade ) {
}