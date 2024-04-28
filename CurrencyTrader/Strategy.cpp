/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    Strategy.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 14, 2024 21:32:07
 */

// see ATR trading notes at bottom
// TODO: load daily bars to calculate TR over last number of days
// TODO: change some of the screeners to use TR rather than bar ranges

#include <boost/log/trivial.hpp>

#include "Strategy.hpp"

Strategy::Strategy()
: DailyTradeTimeFrame<Strategy>()
, m_bfQuotes01Sec( 1 )
, m_bfTrading( 60 )
, m_stateTrade( ETradeState::Init )
, m_ceSwingHi( ou::ChartEntryShape::EShape::Long,  ou::Colour::Purple )
, m_ceSwingLo( ou::ChartEntryShape::EShape::Short, ou::Colour::HotPink )
, m_nLo {}, m_nNet {}, m_nHi {}
, m_fResetSoftware( nullptr )
{
  Init();
}
/*
Strategy::Strategy(  boost::gregorian::date date )
: DailyTradeTimeFrame<Strategy>( date )
, m_bfQuotes01Sec( 1 )
, m_bfTrading( 60 )
, m_stateTrade( ETradeState::Init )
, m_ceSwingHi( ou::ChartEntryShape::EShape::Long,  ou::Colour::Purple )
, m_ceSwingLo( ou::ChartEntryShape::EShape::Short, ou::Colour::HotPink )
, m_nLo {}, m_nNet {}, m_nHi {}
, m_fResetSoftware( nullptr )
{
  Init();
}
*/
void Strategy::Init() {

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName(    "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceBarsTrade.SetName( "Trades" );

  m_ceVolume.SetName(   "Volume" );

  m_ceRealized.SetName( "Realized" );
  m_ceUnRealized.SetName( "Unrealized" );

  m_ceRealized.SetColour( ou::Colour::DarkCyan );
  m_ceUnRealized.SetColour( ou::Colour::Purple );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );

  m_cdv.Add( EChartSlot::Price, &m_ceLongEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceLongExit );

  m_cdv.Add( EChartSlot::Price, &m_ceShortEntry );
  m_cdv.Add( EChartSlot::Price, &m_ceShortExit );

  m_cdv.Add( EChartSlot::Price, &m_ceSwingLo );
  m_cdv.Add( EChartSlot::Price, &m_ceSwingHi );

  m_cdv.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_cdv.Add( EChartSlot::Price, &m_ceTrade );
  m_cdv.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_cdv.Add( EChartSlot::Price, &m_ceBarsTrade );

  m_cdv.Add( EChartSlot::Volume, &m_ceVolume );

  m_cdv.Add( EChartSlot::PL, &m_ceRealized );
  m_cdv.Add( EChartSlot::PL, &m_ceUnRealized );

  // supplied by 1 second mid-quote
  m_pEmaCurrency = std::make_unique<EMA>( 3 * 60, m_cdv, EChartSlot::Price );
  m_pEmaCurrency->Set( ou::Colour::Purple, "Price EMA" );

  // supplied by 1 minute trade bar
  m_pATRFast = std::make_unique<EMA>( 3, m_cdv, EChartSlot::ATR );
  m_pATRFast->Set( ou::Colour::Blue, "ATR Fast" );

  // supplied by 1 minute trade bar
  m_pATRSlow = std::make_unique<EMA>( 14, m_cdv, EChartSlot::ATR );
  m_pATRSlow->Set( ou::Colour::Crimson, "ATR Slow" );

  m_bfQuotes01Sec.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleBarQuotes01Sec ) );
  m_bfTrading.SetOnBarComplete( MakeDelegate( this, &Strategy::HandleMinuteBar ) );
}

Strategy::~Strategy() {

  if ( m_pWatch ) {
    m_pWatch->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuote ) );
    m_pWatch->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTrade ) );
    //m_pWatch->RecordSeries( true );
  }

  m_bfQuotes01Sec.SetOnBarComplete( nullptr );
  m_bfTrading.SetOnBarComplete( nullptr );

  m_pATRFast.reset();
  m_pATRSlow.reset();
  m_pEmaCurrency.reset();

  m_cdv.Clear();
}

void Strategy::SetInstrument( pInstrument_t pInstrument, fConstructPosition_t&& f ) {
  assert( pInstrument );

  pPosition_t pPosition = f( pInstrument, pInstrument->GetInstrumentName() +":up" );
  assert( pPosition );

  m_pPosition = pPosition;
  m_pWatch = m_pPosition->GetWatch();

  m_cdv.SetNames( "Currency Trader", m_pPosition->GetInstrument()->GetInstrumentName() );

  //m_pWatch->RecordSeries( false );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTrade ) );

}

void Strategy::HandleQuote( const ou::tf::Quote& quote ) {

  m_quote = quote;

  const ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );

  //m_quotes.Append( quote );

  //TimeTick( quote );

  m_bfQuotes01Sec.Add( dt, quote.Midpoint(), 1 ); // provides a 1 sec pulse for checking the algorithm

}

void Strategy::HandleTrade( const ou::tf::Trade& trade ) {

  const ptime dt( trade.DateTime() );
  //const ou::tf::Trade::price_t  price(  trade.Price() );
  const ou::tf::Trade::volume_t volume( trade.Volume() );

  // m_ceTrade.Append( dt, price ); // for iqfeed, is same as bid price for fxcm
  const double fake_price( m_quote.Midpoint() );
  m_ceTrade.Append( dt, fake_price );
  //m_ceVolume.Append( dt, volume ); // iqfeed shows as 1 for fxcm

  m_bfTrading.Add( dt, fake_price, volume );

}

void Strategy::HandleRHTrading( const ou::tf::Bar& bar ) { // once a second

  const auto dt( bar.DateTime() );
  const double mid( m_quote.Midpoint() );

  double unrealized, realized, commission, total;
  m_pPosition->QueryStats( unrealized, realized, commission, total );
  m_ceUnRealized.Append( dt, unrealized );
  m_ceRealized.Append( dt, realized );
  m_ceProfitLoss.Append( dt, commission );
  m_ceCommission.Append( dt, total );

  switch ( m_state.swing ) {
    case State::Swing::up:
      m_state.sum += m_state.last - mid;
      m_state.last = mid;
      break;
    case State::Swing::none:
      break;
    case State::Swing::down:
      m_state.sum += mid - m_state.last;
      m_state.last = mid;
      break;
  }

  switch ( m_stateTrade ) {
    case ETradeState::Init: // Strategy starts in this state
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::Search:
      switch ( m_state.swing ) {
        case State::Swing::up:
          EnterLong( m_quote );
          break;
        case State::Swing::none:
          break;
        case State::Swing::down:
          break;
      }
      break;
    case ETradeState::LongSubmitted:
      // wait for exectuion
      break;
    case ETradeState::LongExitSignal:
      switch ( m_state.swing ) {
        case State::Swing::up:
          break;
        case State::Swing::none:
          break;
        case State::Swing::down:
          ExitLong( m_quote );
          break;
      }
      break;
    case ETradeState::ShortSubmitted:
      break;
    case ETradeState::ShortExitSignal:
      break;
    case ETradeState::LongExitSubmitted:
      // wait for execution
      break;
    case ETradeState::ShortExitSubmitted:
      break;
    case ETradeState::NoTrade:
      break;
    case ETradeState::EndOfDayCancel:
      break;
    case ETradeState::EndOfDayNeutral:
      break;
    case ETradeState::Done:
      break;
    default:
      assert( false );
  }

  m_state.swing = State::Swing::none;
}

const int c_quantity( 100000 );

void Strategy::EnterLong( const ou::tf::Quote& quote ) { // limit orders, in real, will need to be normalized
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  m_dblProfitMax = m_dblUnRealized = m_dblProfitMin = 0.0;
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, c_quantity );
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, m_quote.Bid() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongEntry.AddLabel( quote.DateTime(), dblMidPoint, "LeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = ETradeState::LongSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Strategy::EnterShort( const ou::tf::Quote& quote ) { // limit orders, in real, will need to be normalized
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  m_dblProfitMax = m_dblUnRealized = m_dblProfitMin = 0.0;
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, c_quantity );
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, m_quote.Ask() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortEntry.AddLabel( quote.DateTime(), dblMidPoint, "SeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = ETradeState::ShortSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Strategy::ExitLong( const ou::tf::Quote& quote ) {
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, c_quantity );
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, 1, m_quote.Ask() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceLongExit.AddLabel( quote.DateTime(), dblMidPoint, "LxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = ETradeState::LongExitSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Strategy::ExitShort( const ou::tf::Quote& quote ) {
  double dblMidPoint( quote.Midpoint() );
  //assert( nullptr == m_pOrderPending.get() );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, c_quantity );
  //pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, m_quote.Bid() );
  assert( pOrder );
  pOrder->SetSignalPrice( dblMidPoint );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  m_ceShortExit.AddLabel( quote.DateTime(), dblMidPoint, "SxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade = ETradeState::ShortExitSubmitted;
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
}

void Strategy::ShowOrder( pOrder_t pOrder ) {
  //m_pTreeItemOrder = m_pTreeItemSymbol->AppendChild(
  //    "Order "
  //  + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() )
  //  );
}

void Strategy::HandleOrderCancelled( const ou::tf::Order& order ) {
  m_pOrderPending->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrderPending->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(info)
        << m_pWatch->GetInstrumentName()
        << " order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case ETradeState::LongExitSubmitted:
    case ETradeState::ShortExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(error)
        << m_pWatch->GetInstrumentName()
        << " order " << order.GetOrderId() << " cancelled - state machine needs fixes";
      m_stateTrade = ETradeState::Done;
      break;
    default:
      m_stateTrade = ETradeState::Search;
  }
  m_pOrderPending.reset();
}

void Strategy::HandleOrderFilled( const ou::tf::Order& order ) {
  m_pOrderPending->OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleOrderCancelled ) );
  m_pOrderPending->OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleOrderFilled ) );
  switch ( m_stateTrade ) {
    case ETradeState::LongSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Fill" );
      m_stateTrade = ETradeState::LongExitSignal;
      break;
    case ETradeState::ShortSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Fill" );
      m_stateTrade = ETradeState::ShortExitSignal;
      break;
    case ETradeState::LongExitSubmitted:
      m_ceShortFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Long Exit Fill" );
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::ShortExitSubmitted:
      m_ceLongFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Short Exit Fill" );
      m_stateTrade = ETradeState::Search;
      break;
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      // figure out what labels to apply
      break;
    case ETradeState::Done:
      break;
    default:
       assert( false ); // TODO: unravel the state mess if we get here
  }
  m_pOrderPending.reset();
}

void Strategy::ExitPosition( const ou::tf::Quote& quote ) {
  pOrder_t pOrder;
  double dblMidPoint( quote.Midpoint() );

  if ( m_pPosition->IsActive() ) {
    assert( 1 == m_pPosition->GetActiveSize() );
    switch ( m_pPosition->GetRow().eOrderSideActive ) {
      case ou::tf::OrderSide::EOrderSide::Buy:
        pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, 1 );
        pOrder->SetSignalPrice( dblMidPoint );
        pOrder->SetDescription(
            m_sProfitDescription + ","
          + boost::lexical_cast<std::string>( m_dblProfitMin ) + ","
          + boost::lexical_cast<std::string>( m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax - m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax )
          );
        pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );
        m_ceLongExit.AddLabel( quote.DateTime(), dblMidPoint, "LxS2-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
        m_stateTrade = ETradeState::LongExitSubmitted;
        m_pPosition->PlaceOrder( pOrder );
        ShowOrder( pOrder );
        break;
      case ou::tf::OrderSide::EOrderSide::Sell:
        pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, 1 );
        pOrder->SetSignalPrice( dblMidPoint );
        pOrder->SetDescription(
            m_sProfitDescription + ","
          + boost::lexical_cast<std::string>( m_dblProfitMin ) + ","
          + boost::lexical_cast<std::string>( m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax - m_dblUnRealized ) + ","
          + boost::lexical_cast<std::string>( m_dblProfitMax )
          );
        pOrder->OnOrderCancelled.Add( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );
        m_ceShortExit.AddLabel( quote.DateTime(), dblMidPoint, "SxS2-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
        m_stateTrade = ETradeState::ShortExitSubmitted;
        m_pPosition->PlaceOrder( pOrder );
        ShowOrder( pOrder );
        break;
      default:
        assert( false ); // maybe check for unknown
    }
  }
  else {
    m_stateTrade = ETradeState::Search;
  }
}

void Strategy::HandleExitOrderCancelled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );

  switch ( order.GetOrderSide() ) {
    case ou::tf::OrderSide::EOrderSide::Buy: // is dt filled at 'internal' time?
      //m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "LxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = ETradeState::Cancelled;  // or use cancelled for custom processing
      break;
    case ou::tf::OrderSide::EOrderSide::Sell: // is dt filled at 'internal' time?
      //m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "SxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade = ETradeState::Cancelled;  // or use cancelled for custom processing
      break;
    default:
      assert( false );
  }
}

void Strategy::HandleExitOrderFilled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &Strategy::HandleExitOrderFilled ) );

  switch ( order.GetOrderSide() ) {
    case ou::tf::OrderSide::EOrderSide::Buy:
      //m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "LxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      switch( m_stateTrade ) {
        case ETradeState::ShortExitSubmitted:
          m_stateTrade = ETradeState::Search;
          break;
      }
      break;
    case ou::tf::OrderSide::EOrderSide::Sell:
      //m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "SxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      switch( m_stateTrade ) {
        case ETradeState::LongExitSubmitted:
          m_stateTrade = ETradeState::Search;
          break;
      }
      break;
    default:
      assert( false );
  }
}

void Strategy::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_stateTrade = ETradeState::EndOfDayCancel;
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void Strategy::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  switch ( m_stateTrade ) {
    case ETradeState::NoTrade:
      // do nothing
      break;
    default:
      m_stateTrade = ETradeState::EndOfDayNeutral;
      if ( m_pPosition ) {
        m_pPosition->ClosePosition();
      }
      break;
  }
}

void Strategy::HandleAtRHClose( boost::gregorian::date date, boost::posix_time::time_duration time ) { // one shot
  BOOST_LOG_TRIVIAL(info)
    << m_pWatch->GetInstrumentName()
    << " swing delta " << m_nNet
    << ", sum " << m_state.sum
    << ", " << date << 'T' << time
    ;
}

void Strategy::HandleBarQuotes01Sec( const ou::tf::Bar& bar ) {
  assert( m_pEmaCurrency );
  m_pEmaCurrency->Update( bar.DateTime(), bar.Close() );
  TimeTick( bar );
}

void Strategy::HandleMinuteBar( const ou::tf::Bar& bar ) {

  const ptime dt( bar.DateTime() );
  m_ceBarsTrade.AppendBar( bar );

  m_ceVolume.Append( dt, bar.Volume() ); // iqfeed shows as 1 for fxcm

  m_pATRFast->Update( dt, m_TRFast.Update( bar ) );
  m_pATRSlow->Update( dt, m_TRSlow.Update( bar ) );

  // calculate swing points

  Swing& a( m_swing[ 0 ] );
  Swing& b( m_swing[ 1 ] );
  Swing& c( m_swing[ 2 ] );
  Swing& d( m_swing[ 3 ] );
  Swing& e( m_swing[ 4 ] );

  a = b; b = c; c = d; d = e; e.Update( bar );

  {
    const double x = a.hi > b.hi ? a.hi : b.hi;
    const double y = d.hi > e.hi ? d.hi : e.hi;
    const double z = x > y ? x : y;
    if ( c.hi > z ) {
      m_ceSwingLo.AddLabel( c.dt, c.hi, "Swing Dn" );
      m_vSwingTrack.emplace_back( SwingTrack(
        bar.DateTime(),
        c.dt, c.hi,
        e.dt, e.hi ) );
      m_nHi++;
      m_nNet++;
      m_state.swing = State::Swing::down;
      //std::cout
      //         << m_pWatch->GetInstrumentName()
      //  << ',' << "hi"
      //  << ',' << c.hi << ',' << e.hi
      //  << std::endl;
    }
  }

  {
    const double x = a.lo < b.lo ? a.lo : b.lo;
    const double y = d.lo < e.lo ? d.lo : e.lo;
    const double z = x < y ? x : y;
    if ( c.lo < z ) {
      m_ceSwingHi.AddLabel( c.dt, c.lo, "Swing Up" );
      m_vSwingTrack.emplace_back( SwingTrack(
        bar.DateTime(),
        c.dt, c.lo,
        e.dt, e.lo ) );
      m_nLo++;
      m_nNet--;
      m_state.swing = State::Swing::up;
      //std::cout
      //         << m_pWatch->GetInstrumentName()
      //  << ',' << "lo"
      //  << ',' << c.lo << ',' << e.lo
      //  << std::endl;
    }
  }
}

bool Strategy::HandleSoftwareReset( const ou::tf::Bar& ) {
  if ( nullptr != m_fResetSoftware ) {
    return m_fResetSoftware();
  }
  else {
    return false;
  }
}

void Strategy::SaveWatch( const std::string& sPrefix ) {
  if ( m_pWatch ) {
    m_pWatch->SaveSeries( sPrefix );
  }
}

  // The ATR is commonly used as an exit method that can be applied
  // no matter how the entry decision is made. One popular technique
  // is known as the "chandelier exit" and was developed by Chuck LeBeau:
  //   place a trailing stop under the highest high the stock has reached
  //   since you entered the trade. The distance between the highest high
  //   and the stop level is defined as some multiple multiplied by the ATR.

  // While the ATR doesn't tell us in which direction the breakout will occur,
  // it can be added to the closing price, and the trader can buy whenever
  // the next day's price trades above that value.

  // ATR can be used to set up entry signals.
  // ATR breakout systems are commonly used by short-term traders to time entries.
  // This system adds the ATR, or a multiple of the ATR, to the next day's open
  // and buys when prices move above that level. Short trades are the opposite;
  // the ATR or a multiple of the ATR is subtracted from the open and entries
  // occur when that level is broken.

  // maybe ATR could be connected with Darvis for timing entries.

  // To adjust for volatility you can take 10 000 (or whatever number you set as default)
  // and divide by the Average True Range over the last 100 days.
  // This way you allocate more capital to the least volatile stocks compared to the ones more volatile.

  // https://tradeciety.com/how-to-use-the-atr-indicator-guide for Kelner bands
  // ATR can be used to calculate typical PIP movement per day for trading volatility
  // use to calculate daily ATR for an idea of daily trading ranges

  // Timing Trades: ATR is crucial in deciding when to execute trades.
  // A high ATR, signaling high volatility, suggests larger price fluctuations and may be suitable
  // for breakout strategies or anticipating major market shifts.
  // A low ATR, indicating a quieter market, may favor strategies suited to a range-bound environment.

  // Setting Stop Losses: In determining stop-loss orders, ATR is invaluable.
  // It helps traders set stop losses that reflect current market volatility.
  // In a high-volatility (high ATR) scenario, a larger stop loss might be necessary
  // to avoid early exits due to market volatility. In contrast, in low-volatility (low ATR) markets,
  // tighter stop losses could be more effective, minimizing losses while
  // preventing unnecessary position exits.

  // Risk Management: ATR assists in risk management by providing insights into market volatility.
  // Traders can adjust their position sizes in response to this.
  // In volatile markets, reducing position sizes helps manage risk,
  // while in more stable conditions, larger positions might be beneficial.

  // Market Sentiment: ATR can also offer indirect indications of market sentiment.
  // A rapid increase in ATR may signal growing market tension or excitement,
  // potentially indicating upcoming significant market movements.
  // A gradual decrease in ATR can suggest a stabilizing market,
  // often leading to a consolidation phase.

  // ATR can be instrumental in weighing a limit or a market order.
  // The volatility measured by ATR can guide traders in deciding
  // whether to opt for a limit order, seeking a specific price point,
  // or a market order, executing a trade at the current market price,
  // depending on the market’s perceived stability or volatility.
