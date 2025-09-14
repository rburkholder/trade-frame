/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    TrackOrderBase.cpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: August 17, 2025 16:45:42
 */

#include <boost/log/trivial.hpp>

#include <boost/lexical_cast.hpp>

#include <TFInteractiveBrokers/IBTWS.h>

#include "TrackOrderBase.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe

TrackOrderBase::TrackOrderBase()
: m_fCancelled( nullptr )
, m_fOrderCancelled( nullptr ), m_fOrderFilled( nullptr )
{}

TrackOrderBase::TrackOrderBase( pPosition_t pPosition, ou::ChartDataView& cdv, int slot )
: m_fCancelled( nullptr )
, m_fOrderCancelled( nullptr ), m_fOrderFilled( nullptr )
{
  Set( pPosition, cdv, slot );
}

TrackOrderBase::~TrackOrderBase() {}

void TrackOrderBase::Set( pPosition_t pPosition, ou::ChartDataView& cdv, int slot ) {

  assert( pPosition );
  assert( !m_pPosition );

  m_pPosition = pPosition;

  cdv.Add( slot, &m_ceEntrySubmit );
  cdv.Add( slot, &m_ceEntryFill );

  cdv.Add( slot, &m_ceExitSubmit );
  cdv.Add( slot, &m_ceExitFill );

}

void TrackOrderBase::QueryStats( double& unrealized, double& realized, double& commission, double& total ) const {
  m_pPosition->QueryStats( unrealized, realized, commission, total );
}

// see TFTrading/MonitorOrder.cpp
double TrackOrderBase::PriceInterval( const double price ) const {
  assert( m_pPosition );
  double interval {};
  auto pProvider( m_pPosition->GetExecutionProvider() );
  if ( ou::tf::keytypes::EProviderIB == pProvider->ID() ) {
    auto idRule = m_pPosition->GetInstrument()->GetExchangeRule();
    interval = ou::tf::ib::TWS::Cast( pProvider )->GetInterval( price, idRule );
  }
  else {
    interval = m_pPosition->GetInstrument()->GetMinTick();
  }
  assert( 0.0 < interval );
  return interval;
}

// TODO: save exchange rules with instrument in hdf5 when possible
//    for order simulation

// see TFTrading/MonitorOrder.cpp
double TrackOrderBase::Normalize( double price ) const {
  double interval = PriceInterval( price );
  return m_pPosition->GetInstrument()->NormalizeOrderPrice( price, interval );
}

void TrackOrderBase::SetGoodTill( const OrderArgs& args, pOrder_t& pOrder ) {
  // submit GTC limit order (for Interactive Brokers)
  if ( 0 < args.duration ) {
    // strip off fractional seconds
    const boost::posix_time::ptime dtNormalized
      = args.dt
      - boost::posix_time::time_duration( 0, 0, 0, args.dt.time_of_day().fractional_seconds() );

    const boost::posix_time::ptime dtGTD( dtNormalized + boost::posix_time::time_duration( 0, 0, args.duration ) );
    pOrder->SetGoodTillDate( dtGTD );
    pOrder->SetTimeInForce( ou::tf::ETimeInForce::GoodTillDate );
    //BOOST_LOG_TRIVIAL(trace)
    //  << "SetGoodTill:"
    //  << " normal=" << dtNormalized
    //  << ",adjusted=" << dtGTD
    //  ;
  }
}

void TrackOrderBase::Common( const OrderArgs& args, pOrder_t& pOrder ) {

  m_fOrderCancelled = std::move( args.fOrderCancelled );
  m_fOrderFilled = std::move( args.fOrderFilled );

  pOrder->SetSignalPrice( args.signal );
  pOrder->OnOrderCancelled.Add( MakeDelegate( this, &TrackOrderBase::HandleOrderCancelled ) );
  pOrder->OnOrderFilled.Add( MakeDelegate( this, &TrackOrderBase::HandleOrderFilled ) );
  //assert( !m_pOrderPending );
  m_pOrderPending = pOrder;
  m_pPosition->PlaceOrder( pOrder );
  //ShowOrder( pOrder );
  // handle stop, and on a fill callback there may an immediate exit order
  // which ever gets hit, then cancel the other
  // will need to watch the state machine
  // the stop probably isn't part of any state, except the running state
  // stop will be a market order?  or can we install a bracket order?
  //   effectively one cancels the other (OCA)
  // TWS:  PlaceOrder, PlaceComboOrder, PlaceBracketOrder
}

void TrackOrderBase::EnterCommon( const OrderArgs& args, pOrder_t& pOrder ) {
  Common( args, pOrder );
}

void TrackOrderBase::EnterLongLmt( const OrderArgs& args ) { // enter with long limit
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, args.quantity, Normalize( args.limit ) );
  assert( pOrder );
  SetGoodTill( args, pOrder );
  m_ceEntrySubmit.AddLabel( args.dt, args.signal, "LeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade.Set( ETradeState::EntrySubmittedUp, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  EnterCommon( args, pOrder );
}

void TrackOrderBase::EnterLongMkt( const OrderArgs& args ) { // enter with long market
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, args.quantity );
  assert( pOrder );
  m_ceEntrySubmit.AddLabel( args.dt, args.signal, "LeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade.Set( ETradeState::EntrySubmittedUp, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  EnterCommon( args, pOrder );
}

void TrackOrderBase::EnterShortLmt( const OrderArgs& args ) { // enter with short limit
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, args.quantity, Normalize( args.limit ) );
  assert( pOrder );
  SetGoodTill( args, pOrder );
  m_ceEntrySubmit.AddLabel( args.dt, args.signal, "SeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade.Set( ETradeState::EntrySubmittedDn, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  EnterCommon( args, pOrder );
}

void TrackOrderBase::EnterShortMkt( const OrderArgs& args ) { // enter with short market
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, args.quantity );
  assert( pOrder );
  m_ceEntrySubmit.AddLabel( args.dt, args.signal, "SeS-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  m_stateTrade.Set( ETradeState::EntrySubmittedDn, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  EnterCommon( args, pOrder );
}

void TrackOrderBase::ExitCommon( const OrderArgs& args, pOrder_t& pOrder ) {
  m_stateTrade.Set( ETradeState::ExitSubmitted, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  Common( args, pOrder );
}

void TrackOrderBase::ExitLongLmt( const OrderArgs& args ) { // exit short with long limit
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, args.quantity, Normalize( args.limit ) );
  assert( pOrder );
  SetGoodTill( args, pOrder );
  m_ceExitSubmit.AddLabel( args.dt, args.signal, "LxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  ExitCommon( args, pOrder );
}

void TrackOrderBase::ExitLongMkt( const OrderArgs& args ) { // exit short with long market
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Sell, args.quantity );
  assert( pOrder );
  m_ceExitSubmit.AddLabel( args.dt, args.signal, "LxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  ExitCommon( args, pOrder );
}

void TrackOrderBase::ExitShortLmt( const OrderArgs& args ) { // exit long with short limit
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, args.quantity, Normalize( args.limit ) );
  assert( pOrder );
  SetGoodTill( args, pOrder );
  m_ceExitSubmit.AddLabel( args.dt, args.signal, "SxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  ExitCommon( args, pOrder );
}

void TrackOrderBase::ExitShortMkt( const OrderArgs& args ) { // exit long with short market
  assert( 0 < args.quantity );
  pOrder_t pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Market, ou::tf::OrderSide::Buy, args.quantity );
  assert( pOrder );
  m_ceExitSubmit.AddLabel( args.dt, args.signal, "SxS1-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
  ExitCommon( args, pOrder );
}

void TrackOrderBase::HandleOrderCancelled( const ou::tf::Order& order ) {
  assert( m_pOrderPending );
  m_pOrderPending->OnOrderCancelled.Remove( MakeDelegate( this, &TrackOrderBase::HandleOrderCancelled ) );
  m_pOrderPending->OnOrderFilled.Remove( MakeDelegate( this, &TrackOrderBase::HandleOrderFilled ) );
  switch ( m_stateTrade() ) {
    case ETradeState::EndOfDayCancel:
    case ETradeState::EndOfDayNeutral:
      BOOST_LOG_TRIVIAL(info)
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << " order " << order.GetOrderId() << " cancelled - end of day";
      break;
    case ETradeState::EntrySubmittedUp:
    case ETradeState::EntrySubmittedDn:
      // cancels will happen due to limit time out
      BOOST_LOG_TRIVIAL(info)
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << " order " << order.GetOrderId() << " entry cancelled";
      m_stateTrade.Set( ETradeState::Search, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
      break;
    case ETradeState::Cancelling:
      if ( m_fCancelled ) {
        m_fCancelled();
        m_fCancelled = nullptr;
      }
      m_stateTrade.Set( ETradeState::Cancelled, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
      break;
    case ETradeState::ExitSubmitted:
      //assert( false );  // TODO: need to figure out a plan to retry exit
      BOOST_LOG_TRIVIAL(error)
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << " order " << order.GetOrderId() << " exit cancelled - state machine needs fixes";
      m_stateTrade.Set( ETradeState::Done, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
      break;
    default:
      m_stateTrade.Set( ETradeState::Search, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  }

  if ( m_fOrderCancelled ) {
    m_fOrderCancelled( m_pOrderPending->GetQuanFilled(), m_pOrderPending->GetAverageFillPrice() );
  }
  m_fCancelled = nullptr;
  m_fOrderCancelled = nullptr;
  m_fOrderFilled = nullptr;

  m_pOrderPending.reset();
}

void TrackOrderBase::HandleOrderFilled( const ou::tf::Order& order ) {
  assert( m_pOrderPending );
  m_pOrderPending->OnOrderCancelled.Remove( MakeDelegate( this, &TrackOrderBase::HandleOrderCancelled ) );
  m_pOrderPending->OnOrderFilled.Remove( MakeDelegate( this, &TrackOrderBase::HandleOrderFilled ) );

  const double price = order.GetAverageFillPrice();
  const auto quantity = order.GetQuanFilled();

  const auto commission = order.GetIncrementalCommission();

  switch( order.GetOrderSide() ) {
    case ou::tf::OrderSide::Buy:
      // TODO: confirm against gui
      BOOST_LOG_TRIVIAL(info)
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << ",order=" << order.GetOrderId()
        << ",buy,de,"
        << "," << quantity << "@" << price
        << "+" << commission
        ;
      break;
    case ou::tf::OrderSide::Sell:
      // TODO: confirm against gui
      BOOST_LOG_TRIVIAL(info)
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << ",order=" << order.GetOrderId()
        << ",sell,de,"
        << "," << quantity << "@" << price
        << "+" << commission
        ;
      break;
    default:
      assert( false );
  }

  switch ( m_stateTrade() ) {
    case ETradeState::EntrySubmittedUp:
      //m_ceEntryFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Entry Fill" );
      m_stateTrade.Set( ETradeState::ExitSignalUp, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
      break;
    case ETradeState::EntrySubmittedDn:
      //m_ceEntryFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Entry Fill" );
      m_stateTrade.Set( ETradeState::ExitSignalDn, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
      break;
    case ETradeState::ExitSubmitted:
      //m_ceExitFill.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "Exit Fill" );
      m_stateTrade.Set( ETradeState::Search, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
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

  if ( m_fOrderFilled ) {
    m_fOrderFilled( m_pOrderPending->GetQuanFilled(), m_pOrderPending->GetAverageFillPrice() );
  }
  m_fCancelled = nullptr;
  m_fOrderCancelled = nullptr;
  m_fOrderFilled = nullptr;

  m_pOrderPending.reset();

}

void TrackOrderBase::Cancel( fCancelled_t&& fCancelled ) { // may need something if nothing to cancel
  assert( nullptr == m_fCancelled );
  if ( m_pPosition ) {
    m_stateTrade.Set( ETradeState::Cancelling, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
    m_fCancelled = std::move( fCancelled );
    m_pPosition->CancelOrders();
    // what happens if no orders to cancel?
  }
  else {
    m_stateTrade.Set( ETradeState::Cancelled, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ ); // might need to be ::Search
  }
}

void TrackOrderBase::Close() {
  if ( m_pPosition ) {
    m_stateTrade.Set( ETradeState::EndOfDayNeutral, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
    m_pPosition->ClosePosition();
  }
  else {
    m_stateTrade.Set( ETradeState::Done, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  }
}

void TrackOrderBase::HandleCancel( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  m_stateTrade.Set( ETradeState::EndOfDayCancel, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  if ( m_pPosition ) {
    m_pPosition->CancelOrders();
  }
}

void TrackOrderBase::HandleGoNeutral( boost::gregorian::date, boost::posix_time::time_duration ) { // one shot
  switch ( m_stateTrade() ) {
    case ETradeState::NoTrade:
      // do nothing
      break;
    default:
      m_stateTrade.Set( ETradeState::EndOfDayNeutral, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
      if ( m_pPosition ) {
        m_pPosition->ClosePosition();
      }
      break;
  }
}

} // namespace tradeframe
} // namespace oneunified

