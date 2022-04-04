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

#include "DataRow.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

namespace {
  const std::string sFmtInteger( "%i%" );
  const std::string sFmtPrice( "%0.2f%" );
  const std::string sFmtString( "%s%" );
}

DataRow::DataRow( int ix, double price )
: m_bChanged( false ),
  //m_pRowElements( nullptr ),
  m_price( price ), m_ix( ix ),
  m_dreAcctPl( sFmtPrice, m_bChanged ),
  m_dreBidVolume( sFmtInteger, m_bChanged ),
  m_drePrice( sFmtPrice, m_bChanged ),
  m_dreAskVolume( sFmtInteger, m_bChanged ),
  m_dreTicks( sFmtInteger, m_bChanged ),
  m_dreVolume( sFmtInteger, m_bChanged ),
  m_dreIndicatorStatic( sFmtString, m_bChanged ),
  m_dreIndicatorDynamic( sFmtString, m_bChanged )
{}

DataRow::~DataRow() {
  DelRowElements();
}

// TODO: convert to;
//   https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple

void DataRow::SetRowElements( RowElements& re ) {
  //if ( nullptr != m_pRowElements ) {
    DelRowElements();
  //}
  //m_pRowElements = re;

  using Field = RowElements::Field;

  m_dreAcctPl.SetWinRowElement( re[ Field::AcctPL ] );
  m_dreBidVolume.SetWinRowElement( re[ Field::BidVol ] );
  m_drePrice.SetWinRowElement( re[ Field::Price ] );
  m_dreAskVolume.SetWinRowElement( re[ Field::AskVol ] );
  m_dreTicks.SetWinRowElement( re[ Field::Ticks ] );
  m_dreVolume.SetWinRowElement( re[ Field::Volume ] );
  m_dreIndicatorStatic.SetWinRowElement( re[ Field::Static ] );
  m_dreIndicatorDynamic.SetWinRowElement( re[ Field::Dynamic ] );

}

void DataRow::Refresh() {
  m_dreAcctPl.UpdateWinRowElement();
  m_dreBidVolume.UpdateWinRowElement();
  m_drePrice.UpdateWinRowElement();
  m_dreAskVolume.UpdateWinRowElement();
  m_dreTicks.UpdateWinRowElement();
  m_dreVolume.UpdateWinRowElement();
  m_dreIndicatorStatic.UpdateWinRowElement();
  m_dreIndicatorDynamic.UpdateWinRowElement();
}

void DataRow::DelRowElements() {
  //m_pRowElements = nullptr;
  m_dreAcctPl.SetWinRowElement( nullptr );
  m_dreBidVolume.SetWinRowElement( nullptr );
  m_drePrice.SetWinRowElement( nullptr );
  m_dreAskVolume.SetWinRowElement( nullptr );
  m_dreTicks.SetWinRowElement( nullptr );
  m_dreVolume.SetWinRowElement( nullptr );
  m_dreIndicatorStatic.SetWinRowElement( nullptr );
  m_dreIndicatorDynamic.SetWinRowElement( nullptr );
}


} // market depth
} // namespace tf
} // namespace ou

/*
  "Acct1 P/L",
  "Acct2 P/L",
  "Acct P/L",
  "Bid Pend 1",
  "Bid Pend 2",
  "Bid Pend",
  "Bid",
  "Price",
  "Ask",
  "Ask Pend 1",
  "Ask Pend 2",
  "Ask Pend",
  "Ticks",
  "Volume",
  "Histogram",
  "Static Ind",
  "Dynamic Ind"
*/