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
 * Project: TFVuTrading/MarketDepth/Ladder
 * Created: November 11, 2021 09:08
 */

 #include <string>

#include "PriceRow.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

namespace {
  const std::string sFmtInteger( "%i" );
  const std::string sFmtPrice( "%0.2f" );
  const std::string sFmtString( "%s" );
}

PriceRow::PriceRow( double price ) // TODO: get rid of this?
: m_bFirst( true )
, m_bChanged( false )
  //m_pRowElements( nullptr ),
  //m_price( price ),
  //m_dreAcctPl( sFmtPrice, m_bChanged ),
, m_dreBuyCount( sFmtInteger, m_bChanged )
, m_dreBuyVolume( sFmtInteger, m_bChanged )
, m_dreBidSize( sFmtInteger, m_bChanged )
, m_dreBidOrder( sFmtInteger, m_bChanged )
, m_drePrice( sFmtPrice, m_bChanged )
, m_dreAskOrder( sFmtInteger, m_bChanged )
, m_dreAskSize( sFmtInteger, m_bChanged )
, m_dreSellVolume( sFmtInteger, m_bChanged )
, m_dreSellCount(sFmtInteger, m_bChanged )
, m_dreTicks( sFmtInteger, m_bChanged )
, m_dreVolume( sFmtInteger, m_bChanged )
, m_dreIndicatorStatic( sFmtString, m_bChanged )
, m_dreIndicatorDynamic( sFmtString, m_bChanged )
{}

PriceRow::PriceRow( const PriceRow& rhs ) // don't copy or move anything
: m_bFirst( true )
, m_bChanged( false )
  //m_pRowElements( nullptr ),
  //m_price( rhs.m_price ),
  //m_dreAcctPl( sFmtPrice, m_bChanged ),
, m_dreBuyCount( sFmtInteger, m_bChanged )
, m_dreBuyVolume( sFmtInteger, m_bChanged )
, m_dreBidSize( sFmtInteger, m_bChanged )
, m_dreBidOrder( sFmtInteger, m_bChanged )
, m_drePrice( sFmtPrice, m_bChanged )
, m_dreAskOrder( sFmtInteger, m_bChanged )
, m_dreAskSize( sFmtInteger, m_bChanged )
, m_dreSellVolume( sFmtInteger, m_bChanged )
, m_dreSellCount(sFmtInteger, m_bChanged )
, m_dreTicks( sFmtInteger, m_bChanged )
, m_dreVolume( sFmtInteger, m_bChanged )
, m_dreIndicatorStatic( sFmtString, m_bChanged )
, m_dreIndicatorDynamic( sFmtString, m_bChanged )
{}

PriceRow::~PriceRow() {
  DelRowElements();
}

// TODO: convert to;
//   https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple

void PriceRow::SetRowElements( WinRow& wr ) {
  //if ( nullptr != m_pRowElements ) {
    DelRowElements();
  //}
  //m_pRowElements = re;

  //m_dreAcctPl.SetWinRowElement(    wr[ Field::AcctPL ] );
  // TODO: is this set in another thread?  Do we lock it?
  m_dreBuyCount.SetWinRowElement(         m_bFirst, wr[ (int)EField::BuyCount ] );
  m_dreBuyVolume.SetWinRowElement(        m_bFirst, wr[ (int)EField::BuyVolume ] );
  m_dreBidSize.SetWinRowElement(          m_bFirst, wr[ (int)EField::BidSize ] );
  m_dreBidOrder.SetWinRowElement(         m_bFirst, wr[ (int)EField::BidOrder ] );
  m_drePrice.SetWinRowElement(            m_bFirst, wr[ (int)EField::Price ] );
  m_dreAskOrder.SetWinRowElement(         m_bFirst, wr[ (int)EField::AskOrder ] );
  m_dreAskSize.SetWinRowElement(          m_bFirst, wr[ (int)EField::AskSize ] );
  m_dreSellVolume.SetWinRowElement(       m_bFirst, wr[ (int)EField::SellVolume ] );
  m_dreSellCount.SetWinRowElement(        m_bFirst, wr[ (int)EField::SellCount ] );
  m_dreTicks.SetWinRowElement(            m_bFirst, wr[ (int)EField::Ticks ] );
  m_dreVolume.SetWinRowElement(           m_bFirst, wr[ (int)EField::Volume ] );
  m_dreIndicatorStatic.SetWinRowElement(  m_bFirst, wr[ (int)EField::Static ] );
  m_dreIndicatorDynamic.SetWinRowElement( m_bFirst, wr[ (int)EField::Dynamic ] );

  m_bFirst = false;

}

void PriceRow::Refresh() {
  //m_dreAcctPl.UpdateWinRowElement();
  m_dreBuyCount.UpdateWinRowElement();
  m_dreBuyVolume.UpdateWinRowElement();
  m_dreBidSize.UpdateWinRowElement();
  m_dreBidOrder.UpdateWinRowElement();
  m_drePrice.UpdateWinRowElement();
  m_dreAskOrder.UpdateWinRowElement();
  m_dreAskSize.UpdateWinRowElement();
  m_dreSellVolume.UpdateWinRowElement();
  m_dreSellCount.UpdateWinRowElement();
  m_dreTicks.UpdateWinRowElement();
  m_dreVolume.UpdateWinRowElement();
  m_dreIndicatorStatic.UpdateWinRowElement();
  m_dreIndicatorDynamic.UpdateWinRowElement();
}

void PriceRow::DelRowElements() {
  //m_pRowElements = nullptr;
  //m_dreAcctPl.SetWinRowElement( nullptr );
  m_dreBuyCount.SetWinRowElement( false, nullptr );
  m_dreBuyVolume.SetWinRowElement( false, nullptr );
  m_dreBidSize.SetWinRowElement( false, nullptr );
  m_dreBidOrder.SetWinRowElement( false, nullptr );
  m_drePrice.SetWinRowElement( false, nullptr );
  m_dreAskOrder.SetWinRowElement( false, nullptr );
  m_dreAskSize.SetWinRowElement( false, nullptr );
  m_dreSellVolume.SetWinRowElement( false, nullptr );
  m_dreSellCount.SetWinRowElement( false, nullptr );
  m_dreTicks.SetWinRowElement( false, nullptr );
  m_dreVolume.SetWinRowElement( false, nullptr );
  m_dreIndicatorStatic.SetWinRowElement( false, nullptr );
  m_dreIndicatorDynamic.SetWinRowElement( false, nullptr );

}

void PriceRow::Set( fClick_t&& fClick ) {

  m_fClick = std::move( fClick );

  WinRowElement* pwre;

  pwre = m_dreAskOrder.GetWinRowElement();
  assert( pwre );
  pwre->Set(
    [this](EButton button, bool shift, bool control, bool alt ){
      if ( m_fClick ) m_fClick( m_drePrice.Get(), EField::AskOrder, button, shift, control, alt );
    } );

  pwre = m_dreBidOrder.GetWinRowElement();
  assert( pwre );
  pwre->Set(
    [this](EButton button, bool shift, bool control, bool alt ){
      if ( m_fClick ) m_fClick( m_drePrice.Get(), EField::BidOrder, button, shift, control, alt );
    } );
}

void PriceRow::SetAskOrderSize( unsigned int n ) {
  m_dreAskOrder.Set( n );
}

void PriceRow::SetBidOrderSize( unsigned int n ) {
  m_dreBidOrder.Set( n );
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