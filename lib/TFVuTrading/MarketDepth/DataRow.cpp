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
  const std::string sFmtInteger( "%i" );
  const std::string sFmtPrice( "%0.2f" );
  const std::string sFmtString( "%s" );
}

DataRow::DataRow( double price )
: m_bChanged( false )
  //m_pRowElements( nullptr ),
  //m_price( price ),
  //m_dreAcctPl( sFmtPrice, m_bChanged ),
, m_dreBuyCount( sFmtInteger, m_bChanged )
, m_dreBuyVolume( sFmtInteger, m_bChanged )
, m_dreBidSize( sFmtInteger, m_bChanged )
, m_drePrice( sFmtPrice, m_bChanged )
, m_dreAskSize( sFmtInteger, m_bChanged )
, m_dreSellVolume( sFmtInteger, m_bChanged )
, m_dreSellCount(sFmtInteger, m_bChanged )
, m_dreTicks( sFmtInteger, m_bChanged )
, m_dreVolume( sFmtInteger, m_bChanged )
, m_dreIndicatorStatic( sFmtString, m_bChanged )
, m_dreIndicatorDynamic( sFmtString, m_bChanged )
{}

DataRow::DataRow( const DataRow& rhs ) // don't copy or move anything
: m_bChanged( false )
  //m_pRowElements( nullptr ),
  //m_price( rhs.m_price ),
  //m_dreAcctPl( sFmtPrice, m_bChanged ),
, m_dreBuyCount( sFmtInteger, m_bChanged )
, m_dreBuyVolume( sFmtInteger, m_bChanged )
, m_dreBidSize( sFmtInteger, m_bChanged )
, m_drePrice( sFmtPrice, m_bChanged )
, m_dreAskSize( sFmtInteger, m_bChanged )
, m_dreSellVolume( sFmtInteger, m_bChanged )
, m_dreSellCount(sFmtInteger, m_bChanged )
, m_dreTicks( sFmtInteger, m_bChanged )
, m_dreVolume( sFmtInteger, m_bChanged )
, m_dreIndicatorStatic( sFmtString, m_bChanged )
, m_dreIndicatorDynamic( sFmtString, m_bChanged )
{}

DataRow::~DataRow() {
  DelRowElements();
}

// TODO: convert to;
//   https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple

void DataRow::SetRowElements( WinRow& wr ) {
  //if ( nullptr != m_pRowElements ) {
    DelRowElements();
  //}
  //m_pRowElements = re;

  using EField = WinRow::EField;

  //m_dreAcctPl.SetWinRowElement(    wr[ Field::AcctPL ] );
  m_dreBuyCount.SetWinRowElement(    wr[ EField::BuyCount ] );
  m_dreBuyVolume.SetWinRowElement(   wr[ EField::BuyVolume ] );
  m_dreBidSize.SetWinRowElement(     wr[ EField::BidSize ] );
  m_drePrice.SetWinRowElement(       wr[ EField::Price ] );
  m_dreAskSize.SetWinRowElement(     wr[ EField::AskSize ] );
  m_dreSellVolume.SetWinRowElement(  wr[ EField::SellVolume ] );
  m_dreSellCount.SetWinRowElement(   wr[ EField::SellCount ] );
  m_dreTicks.SetWinRowElement(       wr[ EField::Ticks ] );
  m_dreVolume.SetWinRowElement(      wr[ EField::Volume ] );
  m_dreIndicatorStatic.SetWinRowElement(  wr[ EField::Static ] );
  m_dreIndicatorDynamic.SetWinRowElement( wr[ EField::Dynamic ] );

}

void DataRow::Refresh() {
  //m_dreAcctPl.UpdateWinRowElement();
  m_dreBuyCount.UpdateWinRowElement();
  m_dreBuyVolume.UpdateWinRowElement();
  m_dreBidSize.UpdateWinRowElement();
  m_drePrice.UpdateWinRowElement();
  m_dreAskSize.UpdateWinRowElement();
  m_dreSellVolume.UpdateWinRowElement();
  m_dreSellCount.UpdateWinRowElement();
  m_dreTicks.UpdateWinRowElement();
  m_dreVolume.UpdateWinRowElement();
  m_dreIndicatorStatic.UpdateWinRowElement();
  m_dreIndicatorDynamic.UpdateWinRowElement();
}

void DataRow::DelRowElements() {
  //m_pRowElements = nullptr;
  //m_dreAcctPl.SetWinRowElement( nullptr );
  m_dreBuyCount.SetWinRowElement( nullptr );
  m_dreBuyVolume.SetWinRowElement( nullptr );
  m_dreBidSize.SetWinRowElement( nullptr );
  m_drePrice.SetWinRowElement( nullptr );
  m_dreAskSize.SetWinRowElement( nullptr );
  m_dreSellVolume.SetWinRowElement( nullptr );
  m_dreSellCount.SetWinRowElement( nullptr );
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