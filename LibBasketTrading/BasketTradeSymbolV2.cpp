#include "StdAfx.h"
#include "BasketTradeSymbolV2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PivotGroup.h"
#include "Log.h"

#include "HDF5TimeSeriesContainer.h"
#include "HDF5WriteTimeSeries.h"

const double CBasketTradeSymbolV2::m_dblHysterisis = 0.02;

CBasketTradeSymbolV2::CBasketTradeSymbolV2( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy ) 
: CBasketTradeSymbolBase( sSymbolName, sPath, sStrategy ),
  m_TradeSideState( UnknownTradeSide ), m_dblTradeMovingSum( 0 ), m_cntMovingAverageTrades( 0 ),
  m_ixRemovalTrade( 0 ), m_bFoundOpeningTrade( false ), m_bFirstOrder( true ),
  m_pQuoteSW0256( new CTimeSeriesSlidingWindowStatsQuote( &m_quotes,  256 ) ),
  m_pQuoteSW0768( new CTimeSeriesSlidingWindowStatsQuote( &m_quotes,  768 ) ),
  m_pQuoteSW2048( new CTimeSeriesSlidingWindowStatsQuote( &m_quotes, 2048 ) ),
  m_pQuoteSW20( new CTimeSeriesSlidingWindowStatsQuote( &m_quotes, 0, 20 ) ),
  m_pTradesSW20( new CTimeSeriesSlidingWindowStatsTrade( &m_trades, 0, 20 ) )
{
  Initialize();
}

CBasketTradeSymbolV2::~CBasketTradeSymbolV2(void)  {
}

void CBasketTradeSymbolV2::Initialize() {
  m_ceBarVolume.SetColour( Colour::Black );
  m_ceQuoteAsks.SetColour( Colour::Red );
  m_ceQuoteBids.SetColour( Colour::Blue );
  m_ceTrades.SetColour( Colour::Green );
  m_ceOrdersBuy.SetColour( Colour::Blue ); m_ceOrdersBuy.SetShape( CChartEntryShape::EBuy );
  m_ceOrdersSell.SetColour( Colour::Red ); m_ceOrdersSell.SetShape( CChartEntryShape::ESell );

  m_ceAvg.SetColour( Colour::Gold );
  m_ceBBUpper.SetColour( Colour::Brown );
  m_ceBBLower.SetColour( Colour::Brown );

  //m_pdvChart->Add( 0, &m_ceBars );
  m_pdvChart->Add( 1, &m_ceBarVolume );
  m_pdvChart->Add( 0, &m_ceQuoteAsks );
  m_pdvChart->Add( 0, &m_ceQuoteBids );
  m_pdvChart->Add( 0, &m_ceTrades );
  //m_pdvChart->Add( 1, &m_ceTradeVolume );
  m_pdvChart->Add( 0, &m_ceLevels );
  m_pdvChart->Add( 0, &m_ceOrdersBuy );
  m_pdvChart->Add( 0, &m_ceOrdersSell );

  m_pdvChart->Add( 0, &m_ceAvg );
  m_pdvChart->Add( 0, &m_ceBBUpper );
  m_pdvChart->Add( 0, &m_ceBBLower );
}

void CBasketTradeSymbolV2::ModelReady( CBars *pBars ) {
  CPivotGroup pivots( pBars );
  for ( CPivotGroup::const_iterator iter = pivots.begin(); iter != pivots.end(); ++iter ) {
    m_ceLevels.AddMark( iter->first, iter->second.colour, iter->second.sName.c_str() );
  }
}

void CBasketTradeSymbolV2::StartTrading() {
  if ( NULL != m_pModelParameters ) {
    m_pModelParameters->pTreeView->Add( "BasketV2", m_status.sSymbolName, m_pdvChart );

    m_1MinBarFactory.SetBarWidth( m_nBarWidth );  
    m_1MinBarFactory.SetOnBarComplete( MakeDelegate( this, &CBasketTradeSymbolV2::HandleBarFactoryBar ) );

    AddTradeHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleTrade ) );
    AddQuoteHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleQuote ) );
    AddOpenHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleOpen ) );
  }
}

void CBasketTradeSymbolV2::StopTrading() {
  if ( NULL != m_pModelParameters ) {
    RemoveTradeHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleTrade ) );
    RemoveQuoteHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleQuote ) );
    RemoveOpenHandler( MakeDelegate( this, &CBasketTradeSymbolV2::HandleOpen ) );

    m_pModelParameters->pTreeView->Remove( "BasketV2", m_status.sSymbolName );
  }
}

void CBasketTradeSymbolV2::HandleOpen( const CTrade &trade ) {
  m_status.dblOpen = trade.m_dblTrade; // official open
  m_dblBaseLinePrice = trade.m_dblTrade;
  m_ceLevels.AddMark( trade.m_dblTrade, Colour::Plum, "Open" );
  m_bFoundOpeningTrade = true;
}

void CBasketTradeSymbolV2::HandleQuote( const CQuote &quote ) {
  bool b = ( ( 0 < quote.m_dblAsk ) && (  0 < quote.m_dblBid ) );
  //assert( 0 < quote.m_dblAsk );
  //assert( 0 < quote.m_dblBid );
  m_quotes.AppendDatum( quote );
  m_pQuoteSW0256->Update();
  m_pQuoteSW0768->Update();
  m_pQuoteSW2048->Update();
  m_pQuoteSW20->Update();
  if ( b && ( quote.m_dt >= m_pModelParameters->dtRTHBgn ) && ( quote.m_dt < m_pModelParameters->dtRTHEnd ) ) {
    m_ceQuoteAsks.Add( quote.m_dt, quote.m_dblAsk );
    m_ceQuoteBids.Add( quote.m_dt, quote.m_dblBid );
    m_ceAvg.Add( quote.m_dt, m_pQuoteSW0768->MeanY() );
    m_ceBBUpper.Add( quote.m_dt, m_pQuoteSW0768->BBUpper() );
    m_ceBBLower.Add( quote.m_dt, m_pQuoteSW0768->BBLower() );
    m_pdvChart->SetChanged();
  }
}

void CBasketTradeSymbolV2::HandleTrade(const CTrade &trade) {

  //assert( 0 < trade.m_dblTrade );
  //assert( 0 < trade.m_nTradeSize );

  m_status.dblCurrentPrice = trade.m_dblTrade;
  m_trades.AppendDatum( trade );

  m_pTradesSW20->Update();

  if ( ( trade.m_dt >= m_pModelParameters->dtRTHBgn ) && ( trade.m_dt < m_pModelParameters->dtRTHEnd ) ) {
    if ( !m_bFoundOpeningTrade ) {
      HandleOpen( trade );
      m_bFoundOpeningTrade = true;
      LOG << "Open for " << m_status.sSymbolName << " @ " << m_status.dblOpen;
    }
    m_ceTrades.Add( trade.m_dt, trade.m_dblTrade );
    m_ceTradeVolume.Add( trade.m_dt, trade.m_nTradeSize );
    m_1MinBarFactory.Add( trade );
    m_pdvChart->SetChanged();
  }

  /*
  if ( trade.m_dt >= m_pModelParameters->dtBgnNoMoreTrades ) {
    m_TradeSideState = NoMoreTrades;
    if ( trade.m_dt >= m_pModelParameters->dtBgnCancelTrades ) {
      m_TradeSideState = CancelTrades;
      if ( trade.m_dt >= m_pModelParameters->dtBgnCloseTrades ) {
        m_TradeSideState = ClosePositions;
      }
    }
  }
  */

  if ( m_bFoundOpeningTrade ) {

    double avg;

    if ( NoMoreTrading != m_TradeSideState ) {
      if ( trade.m_dt >= m_pModelParameters->dtBgnCloseTrades ) {
        m_TradeSideState = ClosePositions;
      }
      else {
        m_dblTradeMovingSum += trade.m_dblTrade;
        if ( m_cntMovingAverageTrades < m_nMovingAverageValues ) { // init moving average
          ++m_cntMovingAverageTrades;
          if ( m_cntMovingAverageTrades == m_nMovingAverageValues ) {
            m_ixRemovalTrade = m_trades.Count() - m_nMovingAverageValues;
            avg = m_dblTradeMovingSum / m_cntMovingAverageTrades;
            if ( avg > ( m_dblBaseLinePrice + m_dblHysterisis ) ) m_TradeSideState = TransitionLong;
            if ( avg < ( m_dblBaseLinePrice - m_dblHysterisis ) ) m_TradeSideState = TransitionShort;
          }
        }
        else { // cycle the moving average
          m_dblTradeMovingSum -= m_trades[ m_ixRemovalTrade ]->m_dblTrade;
          ++m_ixRemovalTrade;
          avg = m_dblTradeMovingSum / m_cntMovingAverageTrades;
          switch ( m_TradeSideState ) {
            case UnknownTradeSide:
              if ( avg > ( m_dblBaseLinePrice + m_dblHysterisis ) ) m_TradeSideState = TransitionLong;
              if ( avg < ( m_dblBaseLinePrice - m_dblHysterisis ) ) m_TradeSideState = TransitionShort;
              break;
            case GoingLong:
              if ( avg < ( m_dblBaseLinePrice - m_dblHysterisis ) ) m_TradeSideState = TransitionShort;
              break;
            case GoingShort:
              if ( avg > ( m_dblBaseLinePrice + m_dblHysterisis ) ) m_TradeSideState = TransitionLong;
              break;
          }
        }
      }
    }

    COrder *pOrder;
    switch ( m_TradeSideState ) { // use Boost state library?
      case GoingLong: // check for profit
        if ( m_status.dblRunningPL < -300.0 ) {
          pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_status.nPositionSize );
          LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Long Failure Exit, " << m_status.nPositionSize;
          m_ceOrdersSell.AddLabel( trade.m_dt, trade.m_dblTrade, "Long Failure Exit" );
          PlaceOrder( pOrder );
          m_TradeSideState = NoMoreTrading;
        }
        if ( m_status.dblUnRealizedPL > 150.0 ) {
          pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_status.nPositionSize );
          LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Long Profit Exit, " << m_status.nPositionSize;
          m_ceOrdersSell.AddLabel( trade.m_dt, trade.m_dblTrade, "Long Profit Exit" );
          PlaceOrder( pOrder );
          m_TradeSideState = SearchForShort;
        }
        break;
      case GoingShort:  // check for profit
        if ( m_status.dblRunningPL < -300.0 ) {
          pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, -m_status.nPositionSize );
          LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Short Failure Exit, " << -m_status.nPositionSize;
          m_ceOrdersBuy.AddLabel( trade.m_dt, trade.m_dblTrade, "Short Failure Exit" );
          PlaceOrder( pOrder );
          m_TradeSideState = NoMoreTrading;
        }
        if ( m_status.dblUnRealizedPL > 150.0 ) {
          pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, -m_status.nPositionSize );
          LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Short Profit Exit, " << -m_status.nPositionSize;
          m_ceOrdersBuy.AddLabel( trade.m_dt, trade.m_dblTrade, "Short Profit Exit" );
          PlaceOrder( pOrder );
          m_TradeSideState = SearchForLong;
        }
        break;
      case SearchForLong:
        if ( avg > ( m_dblBaseLinePrice + m_dblHysterisis ) ) m_TradeSideState = TransitionLong;
        break;
      case SearchForShort:
        if ( avg < ( m_dblBaseLinePrice - m_dblHysterisis ) ) m_TradeSideState = TransitionShort;
        break;
      case TransitionLong:  // reverse trade direction
        assert( m_status.nPositionSize <= 0 );
        if ( m_status.nPositionSize < 0 ) {
          pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, -m_status.nPositionSize );
          LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Short Stop, " << -m_status.nPositionSize;
          //m_ceOrdersBuy.AddLabel( trade.m_dt, trade.m_dblTrade, "Short Stop" );
          PlaceOrder( pOrder );
        }
        pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy,  m_nQuantityForEntry );
        LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Long, " << m_nQuantityForEntry;
        m_ceOrdersBuy.AddLabel( trade.m_dt, trade.m_dblTrade, "Entry Long" );
        PlaceOrder( pOrder );
        m_TradeSideState = ConfirmLong;
        break;
      case TransitionShort:  // reverse trade direction
        assert( m_status.nPositionSize >= 0 );
        if ( m_status.nPositionSize > 0 ) {
          pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_status.nPositionSize );
          LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Long Stop, " << m_status.nPositionSize ;
          //m_ceOrdersSell.AddLabel( trade.m_dt, trade.m_dblTrade, "Long Stop" );
          PlaceOrder( pOrder );
        }
        pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell,  m_nQuantityForEntry );
        LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Placing Short, " << m_nQuantityForEntry;
        m_ceOrdersSell.AddLabel( trade.m_dt, trade.m_dblTrade, "Short Entry" );
        PlaceOrder( pOrder );
        m_TradeSideState = ConfirmShort;
        break;
      case ConfirmLong:
        //m_TradeSideState = GoingLong;
        break;
      case ConfirmShort:
        //m_TradeSideState = GoingShort;
        break;
      case NoMoreTrades:
        m_TradeSideState = NoMoreTrading;
        break;
      case CancelTrades:
        m_TradeSideState = NoMoreTrading;
        break;
      case ClosePositions:
        if ( 0 == m_status.nPositionSize ) {
          // nothing to do
        }
        else {
          LOG << "Closing " << m_status.sSymbolName;
          if ( 0 < m_status.nPositionSize ) {  // clear out long
            COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Sell, m_status.nPositionSize );
            //pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
            m_ceOrdersSell.AddLabel( trade.m_dt, trade.m_dblTrade, "Close Sell" );
            PlaceOrder( pOrder );
          }
          else { // clear out short
            COrder *pOrder = new COrder( m_pInstrument, OrderType::Market, OrderSide::Buy, -m_status.nPositionSize );
            //pOrder->OnOrderFilled.Add( MakeDelegate( this, &CBasketTradeSymbolInfo::HandleOrderFilled ) );
            m_ceOrdersBuy.AddLabel( trade.m_dt, trade.m_dblTrade, "Close Buy" );
            PlaceOrder( pOrder );
          }
        }
        m_TradeSideState = NoMoreTrading;
        std::cout << m_status.sSymbolName << " Entering NoMoreTrading" << std::endl;
        break;
      case NoMoreTrading:
        break;
    }
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
  m_status.dblRunningPL = m_status.dblUnRealizedPL + m_status.dblRealizedPL;
  m_status.dblMaxRunningPL = max( m_status.dblMaxRunningPL, m_status.dblRunningPL );
  m_status.dblMinRunningPL = min( m_status.dblMinRunningPL, m_status.dblRunningPL );

  OnBasketTradeSymbolChanged( this );
}

void CBasketTradeSymbolV2::HandleBarFactoryBar(const CBar &bar) {
  m_bars.AppendDatum( bar );
  m_ceBars.AddBar( bar );
  m_ceBarVolume.Add( bar.m_dt, bar.m_nVolume );
}

void CBasketTradeSymbolV2::HandleOrderFilled(COrder *pOrder) {

  CBasketTradeSymbolBase::HandleOrderFilled( pOrder );

  LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName
    << " " << pOrder->GetQuanFilled() << "@" << pOrder->GetAverageFillPrice()
    << " Submitted " << pOrder->GetDateTimeOrderSubmitted()
    << " Filled " << ( pOrder->GetDateTimeOrderFilled() - pOrder->GetDateTimeOrderSubmitted() );

  if ( m_bFirstOrder ) {
    m_bFirstOrder = false;
    //m_status.dblOpen = pOrder->GetAverageFillPrice();
    //m_dblBaseLinePrice = m_status.dblOpen;
  }

  switch ( m_TradeSideState ) {
    case ConfirmLong:
      if ( m_status.nPositionSize > 0 ) {
        m_TradeSideState = GoingLong;
        //LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Filled, Going Long";
      }
      else {
        //LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Filled, Exit";
      }
      break;
    case ConfirmShort:
      if ( m_status.nPositionSize < 0 ) {
        m_TradeSideState = GoingShort;
        //LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Filled, Going Short";
      }
      else {
        //LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Filled, Exit";
      }
      break;
    default:
      //LOG << "Order " << pOrder->GetOrderId() << " " << m_status.sSymbolName << " Filled, Exiting";
      break;
  }
}

