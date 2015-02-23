/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "TSReturns.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSReturns::TSReturns(void): m_bFirstAppend( true ) {
}

TSReturns::TSReturns(size_type size) : Prices( size ), m_bFirstAppend( true ) {
}

TSReturns::~TSReturns(void) {
}

void TSReturns::Append( const Bar& bar ) {
  price_t price_ = std::log( bar.Close() );
  if ( m_bFirstAppend ) m_bFirstAppend = false;
  else Prices::Append( Price( bar.DateTime(), price_ - m_priceLast ) );
  m_priceLast = price_;
}

void TSReturns::Append( const Quote& quote ) {
  price_t price_ = std::log( quote.LogarithmicMidPointA() );
  if ( m_bFirstAppend ) m_bFirstAppend = false;
  else Prices::Append( Price( quote.DateTime(), price_ - m_priceLast  ) );
  m_priceLast = price_;
}

void TSReturns::Append( const Trade& trade ) {
  price_t price_ = std::log( trade.Price() );
  if ( m_bFirstAppend ) m_bFirstAppend = false;
  else Prices::Append( Price( trade.DateTime(), price_ - m_priceLast ) );
  m_priceLast = price_;
}

void TSReturns::Append( const Price& price ) {
  price_t price_ = std::log( price.Value() );
  if ( m_bFirstAppend ) m_bFirstAppend = false;
  else Prices::Append( Price( price.DateTime(), price_ - m_priceLast ) );
  m_priceLast = price_;
}


} // namespace tf
} // namespace ou
