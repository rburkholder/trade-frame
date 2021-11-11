/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    DataRow.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: November 11, 2021 09:08
 */

 #include <string>

#include "DataRow.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

namespace {
  const std::string sFmtInteger( "%i%" );
  const std::string sFmtPrice( "%0.2f%" );
  const std::string sFmtString( "%s%" );
}

DataRow::DataRow( size_t ix, double price )
: m_bChanged( false ),
  m_pRowElements( nullptr ),
  m_price( price ), m_ix( ix ),
  m_dreAcctPl( sFmtPrice, m_bChanged ),
  m_dreBidVolume( sFmtInteger, m_bChanged ),
  m_dreBid( sFmtPrice, m_bChanged ),
  m_drePrice( sFmtPrice, m_bChanged ),
  m_dreAsk( sFmtPrice, m_bChanged ),
  m_dreAskVolume( sFmtInteger, m_bChanged ),
  m_dreTicks( sFmtInteger, m_bChanged ),
  m_dreVolume( sFmtInteger, m_bChanged ),
  m_dreIndicatorStatic( sFmtString, m_bChanged ),
  m_dreIndicatorDynamic( sFmtString, m_bChanged )
{}

DataRow::~DataRow() {
}

void DataRow::SetRowElements( RowElements* re ) {
  if ( nullptr != m_pRowElements ) {
    DelRowElements();
  }
  m_pRowElements = re;

}

void DataRow::Refresh() {

}

void DataRow::DelRowElements() {
  m_pRowElements = nullptr;
}


} // market depth
} // namespace tf
} // namespace ou
