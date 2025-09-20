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
 * File:    TrackBracketOrder.cpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: September 13, 2025 12:52:21
 */

#include "TrackBracketOrder.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe

TrackBracketOrder::TrackBracketOrder()
: TrackOrderBase()
{}

TrackBracketOrder::TrackBracketOrder( pPosition_t pPosition, ou::ChartDataView& cdv, int slot )
: TrackOrderBase( pPosition, cdv, slot )
{}

TrackBracketOrder::~TrackBracketOrder() {}

// unused
void TrackBracketOrder::EnterLongBracket( const BracketOrderArgs& args ) {
  // unused, as Position has counters which don't support a set of orders with multiple directions and times
  // will need to simulate equivalent of Bracket Order in the state machine
  assert( 0 < args.quantity );
  m_dblProfitMax = m_dblUnRealized = m_dblProfitMin = 0.0;
  m_stateTrade.Set( ETradeState::EntrySubmittedUp, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );

  assert( 0.0 < args.limit );
  pOrder_t pOrderEntry = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, args.quantity, Normalize( args.limit ) );
  assert( pOrderEntry );
  SetGoodTill( args, pOrderEntry );
  pOrderEntry->SetTransmit( false );
  pOrderEntry->SetSignalPrice( args.signal );
  pOrderEntry->OnOrderCancelled.Add( MakeDelegate( this, &TrackOrderBase::HandleOrderCancelled ) );
  pOrderEntry->OnOrderFilled.Add( MakeDelegate( this, &TrackOrderBase::HandleOrderFilled ) );
  assert( !m_pOrderPending );
  m_pOrderPending = pOrderEntry;
  m_pPosition->PlaceOrder( pOrderEntry );

  assert( 0.0 < args.profit );
  pOrder_t pOrderProfit = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Sell, args.quantity, Normalize( args.profit ) );
  assert( pOrderProfit );
  pOrderProfit->SetTransmit( false );
  pOrderProfit->SetParentOrderId( pOrderEntry->GetOrderId() );
  m_pPosition->PlaceOrder( pOrderProfit );

  assert( 0.0 < args.stop );
  pOrder_t pOrderStop = m_pPosition->ConstructOrder( ou::tf::OrderType::Trail, ou::tf::OrderSide::Sell, args.quantity, Normalize( args.stop ) );
  assert( pOrderStop );
  pOrderStop->SetParentOrderId( pOrderEntry->GetOrderId() );
  m_pPosition->PlaceOrder( pOrderStop );

  // this won't track in the order manager or the position
  //std::dynamic_pointer_cast<ou::tf::ib::TWS>( m_pPosition->GetExecutionProvider() )->PlaceBracketOrder( pOrderEntry, pOrderProfit, pOrderStop );
}

// unused
void TrackBracketOrder::ShowOrder( pOrder_t& pOrder ) {
  //m_pTreeItemOrder = m_pTreeItemSymbol->AppendChild(
  //    "Order "
  //  + boost::lexical_cast<std::string>( m_pOrder->GetOrderId() )
  //  );
}

// unused
void TrackBracketOrder::HandleExitOrderCancelled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderFilled ) );

  switch ( order.GetOrderSide() ) {
    case ou::tf::OrderSide::EOrderSide::Buy: // is dt filled at 'internal' time?
      //m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "LxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade.Set( ETradeState::Cancelled, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );  // or use cancelled for custom processing
      break;
    case ou::tf::OrderSide::EOrderSide::Sell: // is dt filled at 'internal' time?
      //m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetSignalPrice(), "SxC-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      m_stateTrade.Set( ETradeState::Cancelled, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );  // or use cancelled for custom processing
      break;
    default:
      assert( false );
  }
}

// unused
void TrackBracketOrder::HandleExitOrderFilled( const ou::tf::Order& order ) {
  ou::tf::Order& order_( const_cast<ou::tf::Order&>( order ) );
  order_.OnOrderCancelled.Remove( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderCancelled ) );
  order_.OnOrderFilled.Remove( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderFilled ) );

  switch ( order.GetOrderSide() ) {
    case ou::tf::OrderSide::EOrderSide::Buy:
      //m_ceLongExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "LxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      switch( m_stateTrade() ) {
        case ETradeState::ExitSubmitted:
          m_stateTrade.Set( ETradeState::Search, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
          break;
      }
      break;
    case ou::tf::OrderSide::EOrderSide::Sell:
      //m_ceShortExit.AddLabel( order.GetDateTimeOrderFilled(), order.GetAverageFillPrice(), "SxF-" + boost::lexical_cast<std::string>( order.GetOrderId() ) );
      switch( m_stateTrade() ) {
        case ETradeState::ExitSubmitted:
          m_stateTrade.Set( ETradeState::Search, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
          break;
      }
      break;
    default:
      assert( false );
  }
}

// unused
void TrackBracketOrder::ExitPosition( const ou::tf::Quote& quote ) {
  pOrder_t pOrder;
  double dblMidPoint( quote.Midpoint() );

  if ( m_pPosition->IsActive() ) {
    assert( false ); // size of 1 not going to work with currency
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
        pOrder->OnOrderCancelled.Add( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderCancelled ) );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderFilled ) );
        m_ceExitSubmit.AddLabel( quote.DateTime(), dblMidPoint, "LxS2-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
        m_stateTrade.Set( ETradeState::ExitSubmitted, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
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
        pOrder->OnOrderCancelled.Add( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderCancelled ) );
        pOrder->OnOrderFilled.Add( MakeDelegate( this, &TrackBracketOrder::HandleExitOrderFilled ) );
        m_ceExitSubmit.AddLabel( quote.DateTime(), dblMidPoint, "SxS2-" + boost::lexical_cast<std::string>( pOrder->GetOrderId() ) );
        m_stateTrade.Set( ETradeState::ExitSubmitted, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
        m_pPosition->PlaceOrder( pOrder );
        ShowOrder( pOrder );
        break;
      default:
        assert( false ); // maybe check for unknown
    }
  }
  else {
    m_stateTrade.Set( ETradeState::Search, m_pPosition->GetInstrument()->GetInstrumentName(), __FUNCTION__, __LINE__ );
  }
}

} // namespace tradeframe
} // namespace oneunified

