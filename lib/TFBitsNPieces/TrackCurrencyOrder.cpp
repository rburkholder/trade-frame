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
 * File:    TrackCurrencyOrder.cpp
 * Author:  raymond@burkholder.net
 * Project: TFBitsNPieces
 * Created: April 28, 2024 10:39:16
 */

#include <boost/log/trivial.hpp>

#include <TFTrading/Instrument.h>

#include "TrackCurrencyOrder.hpp"

namespace ou { // namespace oneunified
namespace tf { // namespace tradeframe

TrackCurrencyOrder::TrackCurrencyOrder()
: TrackOrderBase()
, m_fFillPrice( nullptr )
, m_fTransferFunds( nullptr )
{}

TrackCurrencyOrder::~TrackCurrencyOrder() {}

void TrackCurrencyOrder::Set( pPosition_t pPosition, ou::ChartDataView& cdv, int slot ) {

  TrackOrderBase::Set( pPosition, cdv, slot );

  ou::tf::Instrument::pInstrument_t pInstrument( pPosition->GetInstrument() );
  m_curBase  = pInstrument->GetCurrencyBase();
  m_curQuote = pInstrument->GetCurrencyCounter();

}

void TrackCurrencyOrder::Set( fFillPrice_t&& f ) {
  assert( nullptr == m_fFillPrice );
  m_fFillPrice = std::move( f );
}

void TrackCurrencyOrder::Set( fTransferFunds_t& f ) {
  m_fTransferFunds = f; // make a copy of the function
}

void TrackCurrencyOrder::HandleOrderFilled( const ou::tf::Order& order ) {

  assert( m_fTransferFunds );

  const double exchange_rate = order.GetAverageFillPrice();
  const auto quantity_base = order.GetQuanFilled();
  const auto commission = order.GetIncrementalCommission();
  const double quantity_converted = quantity_base * exchange_rate;

  //double quantity_converted = quantity_base / exchange_rate; // is this correct?

  // Forex quotes show two currencies,
  //   the base currency, which appears first and
  //   the quote currency or variable currency, which appears last.
  // The price of the first currency is always reflected in units of the second currency.
  // An order is for a 'buy' or a 'sell' of the first currency (base currency)

  switch( order.GetOrderSide() ) {
    case ou::tf::OrderSide::Buy:
      // TODO: confirm against gui
      BOOST_LOG_TRIVIAL(info)
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << ",order=" << order.GetOrderId()
        << ",buy,de,"
        << exchange_rate << ','
        << ou::tf::Currency::Name[ m_curQuote ] << ',' << quantity_converted << ','
        << "cr,"
        << ou::tf::Currency::Name[ m_curBase ] << ',' << quantity_base << ','
        << commission
        ;
      m_fTransferFunds( m_curQuote, quantity_converted, m_curBase, quantity_base, commission );
      break;
    case ou::tf::OrderSide::Sell:
      // TODO: confirm against gui
      BOOST_LOG_TRIVIAL(info)
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << ",order=" << order.GetOrderId()
        << ",sell,de,"
        << exchange_rate << ','
        << ou::tf::Currency::Name[ m_curBase ] << ',' << quantity_base << ','
        << "cr,"
        << ou::tf::Currency::Name[ m_curQuote ] << ',' << quantity_converted << ','
        << commission
        ;
      m_fTransferFunds( m_curBase, quantity_base, m_curQuote, quantity_converted, commission );
      break;
    default:
      assert( false );
  }

  if ( m_fFillPrice ) {
    fFillPrice_t fFillPrice( std::move ( m_fFillPrice ) );
    m_fFillPrice = nullptr;
    fFillPrice( exchange_rate, commission );
  }

  TrackOrderBase::HandleOrderFilled( order );

}

} // namespace tradeframe
} // namespace oneunified
