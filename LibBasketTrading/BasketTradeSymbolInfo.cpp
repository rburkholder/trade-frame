#include "StdAfx.h"

#include "BasketTradeSymbolInfo.h"

#include "TimeSeries.h"
#include "HDF5TimeSeriesContainer.h"
#include "HDF5WriteTimeSeries.h"
#include "InstrumentFile.h"
#include "PivotGroup.h"

// 
// CSymbolInfo
//

// Is this disabled when model decides it shouldn't trade?
// m_status.symbol is no showing for certain of these objects

CBasketTradeSymbolInfo::CBasketTradeSymbolInfo( 
  const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy
  ) 
: m_status( sSymbolName ), m_sPath( sPath ),  m_sStrategy( sStrategy ),
  m_dblAveBarHeight( 0 ), m_dblTrailingStopDistance( 0 ),
  m_PositionState( Init ), m_TradingState( WaitForFirstTrade ),
  m_bDoneTheLong( false ), m_bDoneTheShort( false ),
  m_nBarsInSequence( 0 ), m_nOpenCrossings( 0 ),
  m_OpeningRangeState( WaitForRangeStart ), m_RTHRangeState( WaitForRangeStart ),
  m_pdvChart( NULL ), m_pInstrument( NULL )
{
  Initialize();
}

CBasketTradeSymbolInfo::CBasketTradeSymbolInfo( 
  std::stringstream *pStream )
: m_dblAveBarHeight( 0 ), m_dblTrailingStopDistance( 0 ),
  m_PositionState( Init ), m_TradingState( WaitForOpeningTrade ),
  m_bDoneTheLong( false ), m_bDoneTheShort( false ),
  m_nBarsInSequence( 0 ), m_nOpenCrossings( 0 ),
  m_OpeningRangeState( WaitForRangeStart ), m_RTHRangeState( WaitForRangeStart ),
  m_pdvChart( NULL ), m_pInstrument( NULL )
{
  *pStream >> m_status.sSymbolName >> m_sPath >> m_sStrategy;
  Initialize();
}

CBasketTradeSymbolInfo::~CBasketTradeSymbolInfo( void ) {
  if ( NULL != m_pInstrument ) {
    delete m_pInstrument;
    m_pInstrument = NULL;
  }
  if ( NULL != m_pdvChart ) {
    m_pdvChart->Close();
    delete m_pdvChart;
    m_pdvChart = NULL;
  }
}

void CBasketTradeSymbolInfo::Initialize( void ) {  // constructors only call this
  m_1MinBarFactory.SetBarWidth( m_nBarWidth );  
  m_1MinBarFactory.SetOnBarComplete( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleBarFactoryBar ) );

  CInstrumentFile file;
  file.OpenIQFSymbols();
  try {
    m_pInstrument = file.CreateInstrumentFromIQFeed( m_status.sSymbolName, m_status.sSymbolName );  // todo:  need to verify proper symbol usage
  }
  catch (...) {
    std::cout << "CBasketTradeSymbolInfo::Initialize problems" << std::endl;
  }
  file.CloseIQFSymbols();

  m_ceBars.SetColour( Colour::Orange );
  m_ceBarVolume.SetColour( Colour::Black );
  m_ceQuoteAsks.SetColour( Colour::Red );
  m_ceQuoteBids.SetColour( Colour::Blue );
  m_ceTrades.SetColour( Colour::Green );
  m_ceOrdersBuy.SetColour( Colour::Blue ); m_ceOrdersBuy.SetShape( CChartEntryShape::EBuy );
  m_ceOrdersSell.SetColour( Colour::Red ); m_ceOrdersSell.SetShape( CChartEntryShape::ESell );

  m_pdvChart = new CChartDataView( "Basket", m_status.sSymbolName );
  m_pdvChart->Add( 0, &m_ceBars );
  m_pdvChart->Add( 1, &m_ceBarVolume );
  m_pdvChart->Add( 0, &m_ceQuoteAsks );
  m_pdvChart->Add( 0, &m_ceQuoteBids );
  m_pdvChart->Add( 0, &m_ceTrades );
  //m_pdvChart->Add( 1, &m_ceTradeVolume );
  m_pdvChart->Add( 0, &m_ceLevels );
  m_pdvChart->Add( 0, &m_ceOrdersBuy );
  m_pdvChart->Add( 0, &m_ceOrdersSell );
}

void CBasketTradeSymbolInfo::StartTrading() {
  m_pModelParameters->pTreeView->Add( "Basket", m_status.sSymbolName, m_pdvChart );

  m_pModelParameters->pDataProvider->AddTradeHandler( m_status.sSymbolName, MakeDelegate( this, &CBasketTradeSymbolInfo::HandleTrade ) );
  m_pModelParameters->pDataProvider->AddQuoteHandler( m_status.sSymbolName, MakeDelegate( this, &CBasketTradeSymbolInfo::HandleQuote ) );
  m_pModelParameters->pDataProvider->AddOnOpenHandler( m_status.sSymbolName, MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOpen ) );
}

void CBasketTradeSymbolInfo::StopTrading() {
  m_pModelParameters->pDataProvider->RemoveTradeHandler( m_status.sSymbolName, MakeDelegate( this, &CBasketTradeSymbolInfo::HandleTrade ) );
  m_pModelParameters->pDataProvider->RemoveQuoteHandler( m_status.sSymbolName, MakeDelegate( this, &CBasketTradeSymbolInfo::HandleQuote ) );
  m_pModelParameters->pDataProvider->RemoveOnOpenHandler( m_status.sSymbolName, MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOpen ) );

  m_pModelParameters->pTreeView->Remove( "Basket", m_status.sSymbolName );
}

void CBasketTradeSymbolInfo::StreamSymbolInfo(std::ostream *pStream) {
  *pStream << m_status.sSymbolName << "," << m_sPath << "," << m_sStrategy;
  //int i = pStream->gcount();
  //pStream->rdbuf()->pubsync
}

void CBasketTradeSymbolInfo::CalculateTrade( structCommonModelInformation *pParameters ) {

  m_pModelParameters = pParameters;
  m_dblProposedEntryCost = 0;
  CHDF5TimeSeriesContainer<CBar> barRepository( m_sPath );
  CHDF5TimeSeriesContainer<CBar>::iterator begin, end;
  ptime dt3MonthsAgo = pParameters->dtTradeDate - date_duration( 365 / 4 );
  ptime dt6MonthsAgo = pParameters->dtTradeDate - date_duration( 365 / 2 );
  begin = lower_bound( barRepository.begin(), barRepository.end(), dt6MonthsAgo );
  end = lower_bound( begin, barRepository.end(), pParameters->dtTradeDate );
  hsize_t cnt = end - begin;
  CBars bars;
  bars.Resize( cnt );
  barRepository.Read( begin, end, &bars );

  std::cout << "Entry for " << m_status.sSymbolName;
  if ( 20 < cnt ) {
    /*
    double range = 0;
    CBar *pBar;
    for ( int ix = cnt - 20; ix < cnt; ++ix ) {
      pBar = bars[ ix ];
      range += pBar->m_dblHigh - pBar->m_dblLow;
    }
    */

    double dblClose = bars.Last()->m_dblClose;  
    m_nQuantityForEntry = ( ( (int) ( m_pModelParameters->dblFunds / dblClose ) ) / 100 ) * 100;
    m_dblProposedEntryCost = m_nQuantityForEntry * bars.Last()->m_dblClose;
    std::cout  
      << ": " << m_nQuantityForEntry << "@" << m_dblProposedEntryCost 
      << ", " << dblClose
      << std::endl;

    if ( structCommonModelInformation::Final == m_pModelParameters->nCalcStep ) {
      CPivotGroup pivots( &bars );
      for ( CPivotGroup::const_iterator iter = pivots.begin(); iter != pivots.end(); ++iter ) {
        m_ceLevels.AddMark( iter->first, iter->second.colour, iter->second.sName.c_str() );
      }
    }
  }
  else {
    m_nQuantityForEntry = 0;
    m_dblProposedEntryCost = 0;
    std::cout << m_status.sSymbolName << " didn't have enough bars" << std::endl;
  }
}

void CBasketTradeSymbolInfo::HandleQuote( const CQuote &quote ) {
  m_quotes.AppendDatum( quote );
  if ( ( quote.m_dt >= m_pModelParameters->dtRTHBgn ) && ( quote.m_dt < m_pModelParameters->dtRTHEnd ) ) {
    m_ceQuoteAsks.Add( quote.m_dt, quote.m_dblAsk );
    m_ceQuoteBids.Add( quote.m_dt, quote.m_dblBid );
    m_pdvChart->SetChanged();
  }
}

void CBasketTradeSymbolInfo::HandleTrade(const CTrade &trade) {

  m_status.dblCurrentPrice = trade.m_dblTrade;
  m_trades.AppendDatum( trade );
  if ( ( trade.m_dt >= m_pModelParameters->dtRTHBgn ) && ( trade.m_dt < m_pModelParameters->dtRTHEnd ) ) {
    m_ceTrades.Add( trade.m_dt, trade.m_dblTrade );
    m_ceTradeVolume.Add( trade.m_dt, trade.m_nTradeSize );
    m_pdvChart->SetChanged();
  }

  switch ( m_OpeningRangeState ) {
    case DoneCalculatingRange: 
      break;
    case CalculatingRange:
      if ( trade.m_dt >= m_pModelParameters->dtOpenRangeEnd ) {
        m_OpeningRangeState = DoneCalculatingRange;
      }
      else {
        m_status.dblOpenRangeHigh = max( m_status.dblOpenRangeHigh, trade.m_dblTrade );
        m_status.dblOpenRangeLow  = min( m_status.dblOpenRangeLow, trade.m_dblTrade );
      }
      break;
    case WaitForRangeStart:
      if( trade.m_dt >= m_pModelParameters->dtOpenRangeBgn ) {
        m_status.dblOpenRangeHigh = trade.m_dblTrade;
        m_status.dblOpenRangeLow = trade.m_dblTrade;
        m_OpeningRangeState = CalculatingRange;
      }
      break;
  }

  switch ( m_RTHRangeState ) {
    case CalculatingRange:  // most often encountered case listed first
      if ( trade.m_dt >= m_pModelParameters->dtRTHEnd ) {
        m_RTHRangeState = DoneCalculatingRange;
      }
      else {
        m_status.dblHigh = max( m_status.dblHigh, trade.m_dblTrade );
        m_status.dblLow  = min( m_status.dblLow, trade.m_dblTrade );
      }
      break;
    case DoneCalculatingRange:
      break;
    case WaitForRangeStart:
      if( trade.m_dt >= m_pModelParameters->dtRTHBgn ) {
        m_status.dblHigh = trade.m_dblTrade;
        m_status.dblLow = trade.m_dblTrade;
        m_RTHRangeState = CalculatingRange;
      }
      break;
  }

  switch ( m_TradingState ) {
    case WaitForFirstTrade:  
      if ( m_pModelParameters->bRTH ) {
        m_TradingState = WaitForOpeningBell;
        std::cout << m_status.sSymbolName << " Entering WaitForOpeningBell" << std::endl;
      }
      else {
        m_TradingState = ActiveTrading;
        std::cout << m_status.sSymbolName << " Entering ActiveTrading" << std::endl;  
      }
      break;
    case WaitForOpeningBell:
      if ( trade.m_dt >= m_pModelParameters->dtRTHBgn ) {
        m_TradingState = ActiveTrading;
        std::cout << m_status.sSymbolName << " Entering ActiveTrading" << std::endl;
      }
      // first trade of day isn't put into bars, is/was part of strategy, which may change
      break;
    case ActiveTrading:

      if ( trade.m_dt >= m_pModelParameters->dtBgnNoMoreTrades ) {
        m_TradingState = NoMoreTrades;
        std::cout << m_status.sSymbolName << " Entering NoMoreTrades" << std::endl;
      }
      else {
        // these states are here in order to handle stops at the trade level rather than bar level
        switch ( m_PositionState ) {
          case WaitingForLongExit:
            if ( trade.m_dblTrade < m_status.dblStop ) {
              ++m_nOpenCrossings;
              m_PositionState = WaitingForThe3Bars;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_nQuantityForEntry );
              pOrder->SetSignalPrice( m_status.dblStop );
              pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_ceOrdersSell.AddLabel( trade.m_dt, trade.m_dblTrade, "Long Stop" );
              m_OrderManager.PlaceOrder( m_pModelParameters->pExecutionProvider, pOrder );
            }
            break;
          case WaitingForShortExit:
            if ( trade.m_dblTrade > m_status.dblStop ) {
              ++m_nOpenCrossings;
              m_PositionState = WaitingForThe3Bars;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, m_nQuantityForEntry );
              pOrder->SetSignalPrice( m_status.dblStop );
              pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_ceOrdersBuy.AddLabel( trade.m_dt, trade.m_dblTrade, "Short Stop" );
              m_OrderManager.PlaceOrder( m_pModelParameters->pExecutionProvider, pOrder );
            }
            break;
          default:
            // all other states handled in bar mode
            break;
        }
        m_1MinBarFactory.Add( trade );
        
      }
      break;
    case NoMoreTrades:
      if ( trade.m_dt >= m_pModelParameters->dtBgnCancelTrades ) {
        m_TradingState = CancelTrades;
        std::cout << m_status.sSymbolName << " Entering CancelTrades" << std::endl;
      }
      else {
      }
      break;
    case CancelTrades:
      if ( trade.m_dt >= m_pModelParameters->dtBgnCloseTrades ) {
        m_TradingState = CloseTrades;
        std::cout << m_status.sSymbolName << " Entering CloseTrades" << std::endl;
      }
      else {
      }
      break;
    case CloseTrades:
      if ( trade.m_dt >= m_pModelParameters->dtRTHEnd ) {
        m_TradingState = DoneTrading;
        std::cout << m_status.sSymbolName << " Entering DoneTrading" << std::endl;
      }
      else {
        if ( 0 == m_status.nPositionSize ) {
          // nothing to do
        }
        else {
          std::cout << m_status.sSymbolName << " closing position" << std::endl;
          if ( 0 < m_status.nPositionSize ) {  // clear out long
            COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_status.nPositionSize );
            pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
            m_ceOrdersSell.AddLabel( trade.m_dt, trade.m_dblTrade, "Close Sell" );
            m_OrderManager.PlaceOrder( m_pModelParameters->pExecutionProvider, pOrder );
          }
          else { // clear out short
            COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, -m_status.nPositionSize );
            pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
            m_ceOrdersBuy.AddLabel( trade.m_dt, trade.m_dblTrade, "Close Buy" );
            m_OrderManager.PlaceOrder( m_pModelParameters->pExecutionProvider, pOrder );
          }
        }
        m_TradingState = DoneTrading;
        std::cout << m_status.sSymbolName << " Entering DoneTrading" << std::endl;
      }
      break;
    case DoneTrading:
      // do nothing
      break;
  }

  if ( 0 == m_status.nPositionSize ) {
    m_status.dblUnRealizedPL = 0;
  }
  else {
    if ( 0 < m_status.nPositionSize ) { // long
      m_status.dblUnRealizedPL = ( trade.m_dblTrade - m_status.dblAverageCost ) * m_status.nPositionSize;
    }
    else { // short
      m_status.dblUnRealizedPL = ( m_status.dblAverageCost - trade.m_dblTrade ) * -m_status.nPositionSize;
    }
  }

  OnBasketTradeSymbolInfoChanged( this );
}

void CBasketTradeSymbolInfo::HandleBarFactoryBar(const CBar &bar) {
  double dblTemp = 0;
  static double dblPercentage = 2.0 / ( 10 + 1 );  // Exponential Moving Average on 10 bars (30 sec bars in 5 minutes)
  m_dblAveBarHeight = // 10 bar ema used for calculating A
    ( dblPercentage * ( bar.m_dblHigh - bar.m_dblLow ) ) + ( ( 1 - dblPercentage ) * m_dblAveBarHeight );
  m_bars.AppendDatum( bar );
  m_ceBars.AddBar( bar );
  m_ceBarVolume.Add( bar.m_dt, bar.m_nVolume );
  size_t cnt = m_bars.Count();
  switch ( m_PositionState ) {
    case Init:
      if ( 0 == m_nQuantityForEntry ) {
        m_PositionState = Exited;
      }
      else {
        m_PositionState = WaitingForThe3Bars;
      }
      //break;  // some strategies may require a fall through here
      //case WaitingForOpen:
      //  break;
    case WaitingForThe3Bars:
      //if ( m_bDoneTheLong && m_bDoneTheShort ) {
      if ( m_nMaxCrossings < m_nOpenCrossings ) {
        m_PositionState = Exited;  // don't do any more trading
        break;
      }
      //if ( ( cnt >= 5 ) && ( 0 < m_status.dblOpen ) ) {  // 5 being 2x30 second bars at beginning, then the three bars we need
      if ( cnt >= 5 ) {  // 5 being 2x30 second bars at beginning, then the three bars we need
        CBar bar1, bar2, bar3;
        bar1 = *m_bars[ cnt - 3 ];
        bar2 = *m_bars[ cnt - 2 ];
        bar3 = *m_bars[ cnt - 1 ];
        if ( !m_bDoneTheLong ) {
          double dblAboveThisLevel = m_status.dblOpen;
          if ( ( bar1.m_dblLow > dblAboveThisLevel ) 
            && ( bar2.m_dblLow > dblAboveThisLevel ) 
            && ( bar3.m_dblLow > dblAboveThisLevel ) 
            && ( bar2.m_dblClose > bar1.m_dblClose )
            && ( bar3.m_dblClose > bar2.m_dblClose )
            ) {
              m_status.dblStop = bar1.m_dblLow;
              m_status.dblStop = min( bar2.m_dblLow, m_status.dblStop );
              m_status.dblStop = min( bar3.m_dblLow, m_status.dblStop );
              m_dblTrailingStopDistance = bar.m_dblClose - m_status.dblStop;
              std::cout << m_status.sSymbolName << " Enter LONG now: " << bar.m_dblClose << ", Stop at " << m_status.dblStop << std::endl;
              m_PositionState = WaitingForOrderFulfillmentLong;
              m_bDoneTheLong = true;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, m_nQuantityForEntry );
              pOrder->SetSignalPrice( bar.m_dblClose );
              pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_ceOrdersBuy.AddLabel( bar.m_dt, bar.m_dblClose, "Long Entry" );
              m_OrderManager.PlaceOrder( m_pModelParameters->pExecutionProvider, pOrder );
          }
        }
        if ( !m_bDoneTheShort ) {
          double dblBelowThisLevel = m_status.dblOpen;
          if ( ( bar1.m_dblHigh < dblBelowThisLevel ) 
            && ( bar2.m_dblHigh < dblBelowThisLevel ) 
            && ( bar3.m_dblHigh < dblBelowThisLevel ) 
            && ( bar2.m_dblClose < bar1.m_dblClose )
            && ( bar3.m_dblClose < bar2.m_dblClose )
            ) {
              m_status.dblStop = bar1.m_dblHigh;
              m_status.dblStop = max( bar2.m_dblHigh, m_status.dblStop );
              m_status.dblStop = max( bar3.m_dblHigh, m_status.dblStop );
              m_dblTrailingStopDistance = m_status.dblStop - bar.m_dblClose;
              std::cout << " Enter SHORT Now: " << m_status.sSymbolName << ", Stop at " << m_status.dblStop << std::endl;
              m_PositionState = WaitingForOrderFulfillmentShort;
              m_bDoneTheShort = true;
              COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_nQuantityForEntry );
              pOrder->SetSignalPrice( bar.m_dblClose );
              pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
              m_ceOrdersSell.AddLabel( bar.m_dt, bar.m_dblClose, "Short Entry" );
              m_OrderManager.PlaceOrder( m_pModelParameters->pExecutionProvider, pOrder );
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
      if ( dblTemp > m_status.dblStop ) {
      //  m_dblStop = dblTemp;
      }
      break;
    case WaitingForShortExit:   // adjust stop after each bar
      dblTemp = bar.m_dblClose + m_dblTrailingStopDistance;
      if ( dblTemp < m_status.dblStop ) {
      //  m_dblStop = dblTemp;
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
  // at some point, possibly in different basket algorithm, will need to handle cancelled orders
  pOrder->OnOrderFilled.Remove( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
  m_status.dblFilledPrice = pOrder->GetAverageFillPrice();
  double dblPreviousAverageCost;
  bool bClosing;

  switch ( pOrder->GetOrderSide() ) {
    case OrderSide::Buy:
      bClosing = m_status.nPositionSize < 0;  // we are short, with a buy, closing all or part of position, => realized PL
      dblPreviousAverageCost = m_status.dblAverageCost;
      m_status.nPositionSize += pOrder->GetQuanFilled();
      m_status.dblPositionSize += pOrder->GetQuanFilled() * pOrder->GetAverageFillPrice();
      m_status.dblAverageCost = ( 0 == m_status.nPositionSize ) ? 0 : m_status.dblPositionSize / m_status.nPositionSize;
      if ( bClosing ) {
        m_status.dblRealizedPL += ( dblPreviousAverageCost - pOrder->GetAverageFillPrice() ) * pOrder->GetQuanFilled();
      }
      break;
    case OrderSide::Sell:
      bClosing = m_status.nPositionSize > 0;  // we are long, with a sell, closing all or part of postion, => realized PL
      dblPreviousAverageCost = m_status.dblAverageCost;
      m_status.nPositionSize -= pOrder->GetQuanFilled();
      m_status.dblPositionSize -= pOrder->GetQuanFilled() * pOrder->GetAverageFillPrice();
      m_status.dblAverageCost = ( 0 == m_status.nPositionSize ) ? 0 : m_status.dblPositionSize / m_status.nPositionSize;
      if ( bClosing ) {
        m_status.dblRealizedPL += ( pOrder->GetAverageFillPrice() - dblPreviousAverageCost ) * pOrder->GetQuanFilled();
      }
      break;
  }

  switch ( m_PositionState ) {
    case WaitingForOrderFulfillmentLong:
      m_PositionState = WaitingForLongExit;
      break;
    case WaitingForOrderFulfillmentShort:
      m_PositionState = WaitingForShortExit;
      break;
    case WaitingForThe3Bars:
      // ok, but do nothing
      break;
    default:
      std::cout << "Basket " << m_status.sSymbolName << " has problems" << std::endl;
  }
}

void CBasketTradeSymbolInfo::HandleOpen( const CTrade &trade ) {
  m_status.dblOpen = trade.m_dblTrade; // official open
  m_ceLevels.AddMark( trade.m_dblTrade, Colour::Plum, "Open" );
}

void CBasketTradeSymbolInfo::WriteTradesAndQuotes(const std::string &sPathPrefix) {
  if ( 0 != m_trades.Count() ) {
    CHDF5WriteTimeSeries<CTrades, CTrade> wts;
    wts.Write( sPathPrefix + "/trades/" + m_status.sSymbolName, &m_trades );
  }
  if ( 0 != m_quotes.Count() ) {
    CHDF5WriteTimeSeries<CQuotes, CQuote> wts;
    wts.Write( sPathPrefix + "/quotes/" + m_status.sSymbolName, &m_quotes );
  }
}