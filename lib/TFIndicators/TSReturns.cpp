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

#pragma once

#include "StdAfx.h"

#include "TSReturns.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

TSReturns::TSReturns(void) {
}

TSReturns::TSReturns(size_type size) : CPrices( size ) {
}

TSReturns::~TSReturns(void) {
}

void TSReturns::Append( const CBar& bar ) {
  price_t price_ = std::log( bar.Close() );
  if ( 0 != CPrices::Size() ) CPrices::Append( CPrice( bar.DateTime(), price_ - m_priceLast ) );
  m_priceLast = price_;
}

void TSReturns::Append( const CQuote& quote ) {
  price_t price_ = std::log( quote.LogarithmicMidPointA() );
  if ( 0 != CPrices::Size() ) CPrices::Append( CPrice( quote.DateTime(), price_ - m_priceLast  ) );
  m_priceLast = price_;
}

void TSReturns::Append( const CTrade& trade ) {
  price_t price_ = std::log( trade.Trade() );
  if ( 0 != CPrices::Size() ) CPrices::Append( CPrice( trade.DateTime(), price_ - m_priceLast ) );
  m_priceLast = price_;
}

void TSReturns::Append( const CPrice& price ) {
  price_t price_ = std::log( price.Price() );
  if ( 0 != CPrices::Size() ) CPrices::Append( CPrice( price.DateTime(), price_ - m_priceLast ) );
  m_priceLast = price_;
}


} // namespace tf
} // namespace ou
