/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    MonitorOrder.cpp
 * Author:  raymond@burkholder.net
 * Project: TFTrading
 * Created on May 25, 2019, 1:44 PM
 */

#include <boost/log/trivial.hpp>

#include <TFInteractiveBrokers/IBTWS.h>

#include "MonitorOrder.h"

namespace ou {
namespace tf {

namespace {
  const size_t c_nAdjustmentPeriods( 4 );
}

MonitorOrder::MonitorOrder()
: m_state( State::NoPosition )
, m_bEnableStatsAdd( false )
, m_CountDownToAdjustment {}
{}

MonitorOrder::MonitorOrder( pPosition_t& pPosition )
: m_bEnableStatsAdd( false )
, m_CountDownToAdjustment {}
{
  // TODO: need to undo anything? or already performed?
  SetPosition( pPosition );
}

// what checks to perform on m_state?  need to be in a good state for things to sync properly
//assert( !m_pPosition ); // let us see where this goes, may raise an issue with the constructor initialized with position
//assert( !m_pOrder ); // this causes issues if duplicated
MonitorOrder::MonitorOrder( MonitorOrder&& rhs )
: m_state( rhs.m_state )
, m_bEnableStatsAdd( rhs.m_bEnableStatsAdd )
, m_CountDownToAdjustment( rhs.m_CountDownToAdjustment )
, m_pPosition( std::move( rhs.m_pPosition ) )
, m_pOrder( std::move( rhs.m_pOrder ) )
{
  rhs.m_bEnableStatsAdd = false;
  rhs.m_state = State::NoPosition;
}

MonitorOrder& MonitorOrder::operator=( MonitorOrder&& rhs ) {
  if ( this != &rhs ) {
    m_state = rhs.m_state;
    m_bEnableStatsAdd = rhs.m_bEnableStatsAdd;
    m_CountDownToAdjustment = rhs.m_CountDownToAdjustment;
    m_pPosition = std::move( rhs.m_pPosition ),
    m_pOrder = std::move( rhs.m_pOrder );
    rhs.m_bEnableStatsAdd = false;
    rhs.m_state = State::NoPosition;
  }
  return *this;
}

void MonitorOrder::EnableStatsRemove() {
  if ( State::NoPosition == m_state ) {
    //BOOST_LOG_TRIVIAL(info) << "MonitorOrder::EnableStatsRemove has inconsistent State::NoPosition";
  }
  else {
    if ( m_bEnableStatsAdd ) {
      if ( m_pPosition ) {
        ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
        pWatch->EnableStatsRemove();
        m_bEnableStatsAdd = false;
      }
      else {
        // also in debug, check the flag, possible problem in caller
        BOOST_LOG_TRIVIAL(info) << "MonitorOrder::EnableStatsRemove inconsistency";
      }
    }
  }
}

void MonitorOrder::SetPosition( pPosition_t pPosition ) {
  assert( !m_pOrder ); // no outstanding orders should exist
  EnableStatsRemove();
  m_pPosition = pPosition;
  ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
  pWatch->EnableStatsAdd();
  m_state = State::Available;
}

double MonitorOrder::NormalizePrice( double price ) const { // step #1
  double interval = PriceInterval( price );
  return m_pPosition->GetInstrument()->NormalizeOrderPrice( price, interval );
}

double MonitorOrder::PriceInterval( double price ) const { // step #2

  double interval( 0.01 );
  ou::tf::Instrument::pInstrument_t pInstrument( m_pPosition->GetInstrument() );

  if ( pInstrument->ExchangeRuleAvailable() ) {
    auto idRule = pInstrument->GetExchangeRule();
    switch ( m_pPosition->GetExecutionProvider()->ID() ) {
      case ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIB:
        interval = ou::tf::ib::TWS::Cast( m_pPosition->GetExecutionProvider() )->GetInterval( price, idRule );
        break;
      default:
        assert( false );  // will need to work on alternate logic
    }
  }
  else {
    interval = pInstrument->GetMinTick();
  }

  return interval;
}

// can only work on one order at a time
bool MonitorOrder::PlaceOrder( boost::uint32_t nOrderQuantity, ou::tf::OrderSide::EOrderSide side ) {
  bool bOk( false );
  switch ( m_state ) {
    case State::Available:
    case State::Cancelled:  // can overwrite?
    case State::Filled:     // can overwrite?
      {
        bool bSpreadOk;
        size_t best_count;
        double best_spread;

        ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
        const double midQuote = pWatch->LastQuote().Midpoint();

        std::tie( bSpreadOk, best_count, best_spread ) = pWatch->SpreadStats();
        if ( !bSpreadOk ) {
          BOOST_LOG_TRIVIAL(info)
            << "MonitorOrder PlaceOrder without best spread: "
            << "count=" << best_count
            << ",spread=" << best_spread
            << ",mid=" << midQuote
            ;
        }

        const double dblNormalizedPrice = NormalizePrice( midQuote );
        m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, side, nOrderQuantity, dblNormalizedPrice );
        if ( m_pOrder ) {
          m_state = State::Active;
          m_pOrder->SetSignalPrice( dblNormalizedPrice );
          m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
          m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
          m_CountDownToAdjustment = c_nAdjustmentPeriods;
          m_pPosition->PlaceOrder( m_pOrder );
          BOOST_LOG_TRIVIAL(info)
            << "MonitorOrder "
            << m_pOrder->GetDateTimeOrderSubmitted().time_of_day() << " "
            << m_pOrder->GetOrderId() << " "
            << ou::tf::OrderSide::Name[ side ] << " "
            << m_pOrder->GetInstrument()->GetInstrumentName() << ": "
            << m_pOrder->GetOrderSideName()
            << " limit submitted at " << dblNormalizedPrice
            ;
          bOk = true;
        }
        else {
          m_state = State::Available;
          BOOST_LOG_TRIVIAL(info)
            << "MonitorOrder::PlaceOrder: "
            << m_pPosition->GetInstrument()->GetInstrumentName() << " failed to construct order"
            ;
        }
      }
      break;
    case State::Active:
      BOOST_LOG_TRIVIAL(info) << "MonitorOrder::PlaceOrder: " << m_pPosition->GetInstrument()->GetInstrumentName() << ": active, cannot place order";
      break;
    case State::NoPosition:
      BOOST_LOG_TRIVIAL(info) << "MonitorOrder::PlaceOrder: " << m_pPosition->GetInstrument()->GetInstrumentName() << ": no position";
      break;
    case State::ManualCancel:
      assert( false ); // will need to correct the logic?  or is this a fall-through?
      break;
  }
  return bOk;
}

void MonitorOrder::ClosePosition() {
  if ( m_pPosition ) {
    const ou::tf::Position::TableRowDef& row( m_pPosition->GetRow() );
    if ( IsOrderActive() ) {
      BOOST_LOG_TRIVIAL(info) << row.sName << ": error, monitor has active order, no close possible";
    }
    else {
      if ( 0 != row.nPositionPending ) {
        BOOST_LOG_TRIVIAL(info) << row.sName << ": warning, has pending size of " << row.nPositionPending << " during close";
      }
      if ( 0 != row.nPositionActive ) {
        BOOST_LOG_TRIVIAL(info) << row.sName << ": monitored closing position, side=" << row.eOrderSideActive << ", q=" << row.nPositionActive;
        switch ( row.eOrderSideActive ) {
          case ou::tf::OrderSide::Buy:
            PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Sell );
            break;
          case ou::tf::OrderSide::Sell:
            PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Buy );
            break;
        }
      }
    }
  }
}

void MonitorOrder::CancelOrder() {  // TODO: need to fix this, and take the Order out of UpdateOrder
  switch ( m_state ) {
    case State::Active:
      assert ( m_pOrder );
      m_state = State::ManualCancel;
      m_pPosition->CancelOrder( m_pOrder->GetOrderId() );
      break;
    case State::Available:
    case State::Cancelled:
    case State::Filled:
    case State::NoPosition:
      break;
    case State::ManualCancel:
      if ( m_pOrder ) {
        BOOST_LOG_TRIVIAL(info) << "MonitorOrder::CancelOrder already issued a cancellation" << m_pOrder->GetOrderId();
      }
      else {
        BOOST_LOG_TRIVIAL(info) << "MonitorOrder::CancelOrder: no order to cancel";
      }

      break;
  }
}

void MonitorOrder::Tick( ptime dt ) {
  switch ( m_state ) {
    case State::Active:
      UpdateOrder( dt );
      break;
    case State::Cancelled:
    case State::Filled:
      //m_pOrder.reset();
      EnableStatsRemove();
      m_state = State::Available;
      // TODO: clear position?
      break;
    case State::Available:
    case State::NoPosition:
    case State::ManualCancel:
      break;
  }
}

bool MonitorOrder::IsOrderActive() const { return ( State::Active == m_state ); }

// To think about: use GTD Limit order to automate the count dow?
//  means cancellation, and resubmission, which creates 'dead zones'
//  with currnet order update mechanism, the order is in continuous deployment
void MonitorOrder::UpdateOrder( ptime dt ) {

  if ( 0 == m_pOrder->GetQuanRemaining() ) { // not sure if a cancel adjusts remaining
    // TODO: generate message? error on filled, but may be present on cancel
  }
  else {
    assert( 0 < m_CountDownToAdjustment );
    m_CountDownToAdjustment--;
    bool bUpdateOrder( false );
    if ( 0 == m_CountDownToAdjustment ) {
      // TODO: need logic if order was rejected

      bool bSpreadOk;
      size_t best_count;
      double best_spread;

      ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
      std::tie( bSpreadOk, best_count, best_spread ) = pWatch->SpreadStats();
      const Quote& quote( pWatch->LastQuote() );

      if ( !bSpreadOk ) {
        BOOST_LOG_TRIVIAL(info)
          << "MonitorOrder PlaceOrder without best spread: "
          << "count=" << best_count
          << ",spread=" << best_spread
          << ",bid=" << quote.Bid()
          << ",ask=" << quote.Ask()
          ;
        m_CountDownToAdjustment = 1; // wait for next loop through for a better spread
      }
      else {
        const double priceOrder = m_pOrder->GetPrice1();

        switch ( m_pOrder->GetOrderSide() ) {
          case ou::tf::OrderSide::Buy:
            {
              // TODO: need to use the EnsableStatsAdd to run this code only with decent spread
              // TODO: maximum number of increments? aka don't chase too far?
              // TODO: check that bid is non-zero
              //const double normalizedBid = NormalizePrice( quote.Bid() );
              //if ( normalizedBid > priceOrder ) { // adjust bid with fast moving quote
                // don't chase
                //m_pOrder->SetPrice1( normalizedBid );
                //bUpdateOrder = true;
              //}
              //else { // increment bid on slow moving quote
                if ( quote.Ask() > priceOrder ) {
                  m_pOrder->SetPrice1( NormalizePrice( priceOrder + PriceInterval( quote.Ask() ) ) );
                  bUpdateOrder = true;
                }
                else {  // need to wait for execution
                  // TODO: need to expire this after a while
                }
              //}
            }
            break;
          case ou::tf::OrderSide::Sell:
            {
              // TODO: maximum number of increments? aka don't chase too far?
              // TODO: check that ask is non-zero
              //const double normalizedAsk = NormalizePrice( quote.Ask() );
              //if ( normalizedAsk < priceOrder ) { // adjust bid with fast moving quote
                // don't chase
                //m_pOrder->SetPrice1( normalizedAsk );
                //bUpdateOrder = true;
              //}
              //else { // increment ask on slow moving quote
                if ( quote.Bid() < priceOrder ) {
                  m_pOrder->SetPrice1( NormalizePrice( priceOrder - PriceInterval( quote.Bid() ) ) );
                  bUpdateOrder = true;
                }
                else {  // need to wait for execution
                  // TODO: need to expire this after a while
                }
              //}
            }
            break;
          default:
            assert( false );
            break;
        }
        // TODO: need to cancel both legs if spread is not < something reasonable
        if ( bUpdateOrder ) {

          auto tod = dt.time_of_day();

          BOOST_LOG_TRIVIAL(info)
            << "MonitorOrder "
            << tod << " "
            << m_pOrder->GetOrderId() << " "
            << m_pPosition->GetInstrument()->GetInstrumentName()
            << ": update "
            << ou::tf::OrderSide::Name[ m_pOrder->GetOrderSide() ]
            << " order to " << m_pOrder->GetPrice1()
            //<< " on " << dblNormalizedPrice
            << ", bid=" << quote.Bid()
            << ", ask=" << quote.Ask()
            ;
          m_pPosition->UpdateOrder( m_pOrder );

        }
        m_CountDownToAdjustment = c_nAdjustmentPeriods;
      }
    }
  }
}

void MonitorOrder::OrderCancelled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
  //auto tod = order.GetDateTimeOrderFilled().time_of_day(); [not available in cancelled order]
  // look at TWS::openOrder, ~ line 718 to evaluate order status
  switch ( m_state ) {
    case State::Active:
      // TODO: rework this for order resubmission for GTD, are there any cancellation codes from the exchange?
      //   or not, as we currently simply perform an order update currently
      assert( order.GetOrderId() == m_pOrder->GetOrderId() );
      m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
      m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
      BOOST_LOG_TRIVIAL(info)
//        << tod << " "
        << "MonitorOrder "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": exchange cancelled"
        ;
      m_pOrder.reset();
      m_state = State::Cancelled;
      break;
    case State::ManualCancel:
      assert( order.GetOrderId() == m_pOrder->GetOrderId() );
      m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
      m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
      BOOST_LOG_TRIVIAL(info)
//        << tod << " "
        << "MonitorOrder "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": manual cancellation complete"
        ;
      m_pOrder.reset();
      m_state = State::Cancelled;
      break;
    default:
      BOOST_LOG_TRIVIAL(info)
//        << tod << " "
        << "MonitorOrder "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": cancelled has no matching state (" << (int)m_state << ")"
        ;
  }
}

void MonitorOrder::OrderFilled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
  auto tod = order.GetDateTimeOrderFilled().time_of_day();
  switch ( m_state ) {
    case State::Active:
      assert( order.GetOrderId() == m_pOrder->GetOrderId() );
      m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
      m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
      BOOST_LOG_TRIVIAL(info)
        << "MonitorOrder "
        << tod << " "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": filled at " << m_pOrder->GetAverageFillPrice()
        ;
      m_pOrder.reset();
      m_state = State::Filled;
      break;
    default:
      BOOST_LOG_TRIVIAL(info)
        << "MonitorOrder "
        << tod << " "
        << order.GetOrderId() << " "
        << order.GetInstrument()->GetInstrumentName()
        << ": fillled has no matching state (" << (int)m_state << ")"
        ;
  }
}

} // namespace ou
} // namespace tf
