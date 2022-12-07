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
  const std::string sFmtPrice(   "%0.2f" );
  const std::string sFmtString(  "%s" );
}

PriceRow::PriceRow( double price, const vElement_t& v ) // called from PriceRows::operator[]( int ix )
: m_bChanged( false )
, m_dreAcctPl(           m_bChanged, sFmtPrice,   v[ (int)rung::EField::PL ].colours )
, m_dreBuyCount(         m_bChanged, sFmtInteger, v[ (int)rung::EField::BuyCount ].colours )
, m_dreBuyVolume(        m_bChanged, sFmtInteger, v[ (int)rung::EField::BuyVolume ].colours )
, m_dreBidSize(          m_bChanged, sFmtInteger, v[ (int)rung::EField::BidSize ].colours )
, m_dreBidOrder(         m_bChanged, sFmtInteger, v[ (int)rung::EField::BidOrder ].colours )
, m_drePrice(            m_bChanged, sFmtPrice,   v[ (int)rung::EField::Price ].colours )
, m_dreAskOrder(         m_bChanged, sFmtInteger, v[ (int)rung::EField::AskOrder ].colours )
, m_dreAskSize(          m_bChanged, sFmtInteger, v[ (int)rung::EField::AskSize ].colours )
, m_dreSellVolume(       m_bChanged, sFmtInteger, v[ (int)rung::EField::SellVolume ].colours )
, m_dreSellCount(        m_bChanged, sFmtInteger, v[ (int)rung::EField::SellCount ].colours )
, m_dreTicks(            m_bChanged, sFmtInteger, v[ (int)rung::EField::Ticks ].colours )
, m_dreVolume(           m_bChanged, sFmtInteger, v[ (int)rung::EField::Volume ].colours )
, m_dreIndicatorStatic(  m_bChanged, sFmtString,  v[ (int)rung::EField::Static ].colours )
, m_dreIndicatorDynamic( m_bChanged, sFmtString,  v[ (int)rung::EField::Dynamic ].colours )
{}

PriceRow::PriceRow( const PriceRow& rhs ) // called from PriceRows::operator[]( int ix )
: m_bChanged( false )
, m_dreAcctPl(           m_bChanged, rhs.m_dreAcctPl )
, m_dreBuyCount(         m_bChanged, rhs.m_dreBuyCount )
, m_dreBuyVolume(        m_bChanged, rhs.m_dreBuyVolume )
, m_dreBidSize(          m_bChanged, rhs.m_dreBidSize )
, m_dreBidOrder(         m_bChanged, rhs.m_dreBidOrder )
, m_drePrice(            m_bChanged, rhs.m_drePrice )
, m_dreAskOrder(         m_bChanged, rhs.m_dreAskOrder )
, m_dreAskSize(          m_bChanged, rhs.m_dreAskSize )
, m_dreSellVolume(       m_bChanged, rhs.m_dreSellVolume )
, m_dreSellCount(        m_bChanged, rhs.m_dreSellCount )
, m_dreTicks(            m_bChanged, rhs.m_dreTicks )
, m_dreVolume(           m_bChanged, rhs.m_dreVolume )
, m_dreIndicatorStatic(  m_bChanged, rhs.m_dreIndicatorStatic )
, m_dreIndicatorDynamic( m_bChanged, rhs.m_dreIndicatorDynamic )
{}

PriceRow::~PriceRow() {
  DelRowElements();
}

// TODO: convert to;
//   https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple

void PriceRow::SetRowElements( WinRow& wr ) {

  DelRowElements();

  // TODO: is this set in another thread?  Do we lock it?
  m_dreAcctPl.SetWinRowElement(           wr[ (int)EField::PL ] );
  m_dreBuyCount.SetWinRowElement(         wr[ (int)EField::BuyCount ] );
  m_dreBuyVolume.SetWinRowElement(        wr[ (int)EField::BuyVolume ] );
  m_dreBidSize.SetWinRowElement(          wr[ (int)EField::BidSize ] );
  m_dreBidOrder.SetWinRowElement(         wr[ (int)EField::BidOrder ] );
  m_drePrice.SetWinRowElement(            wr[ (int)EField::Price ] );
  m_dreAskOrder.SetWinRowElement(         wr[ (int)EField::AskOrder ] );
  m_dreAskSize.SetWinRowElement(          wr[ (int)EField::AskSize ] );
  m_dreSellVolume.SetWinRowElement(       wr[ (int)EField::SellVolume ] );
  m_dreSellCount.SetWinRowElement(        wr[ (int)EField::SellCount ] );
  m_dreTicks.SetWinRowElement(            wr[ (int)EField::Ticks ] );
  m_dreVolume.SetWinRowElement(           wr[ (int)EField::Volume ] );
  m_dreIndicatorStatic.SetWinRowElement(  wr[ (int)EField::Static ] );
  m_dreIndicatorDynamic.SetWinRowElement( wr[ (int)EField::Dynamic ] );

  m_bChanged = true;

}

void PriceRow::Refresh() {
  if ( m_bChanged ) {
    m_dreAcctPl.UpdateWinRowElement();
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
    m_bChanged = false;
  }
}

void PriceRow::DelRowElements() {
  m_dreAcctPl.SetWinRowElement( nullptr );
  m_dreBuyCount.SetWinRowElement( nullptr );
  m_dreBuyVolume.SetWinRowElement( nullptr );
  m_dreBidSize.SetWinRowElement( nullptr );
  m_dreBidOrder.SetWinRowElement( nullptr );
  m_drePrice.SetWinRowElement( nullptr );
  m_dreAskOrder.SetWinRowElement( nullptr );
  m_dreAskSize.SetWinRowElement( nullptr );
  m_dreSellVolume.SetWinRowElement( nullptr );
  m_dreSellCount.SetWinRowElement( nullptr );
  m_dreTicks.SetWinRowElement( nullptr );
  m_dreVolume.SetWinRowElement( nullptr );
  m_dreIndicatorStatic.SetWinRowElement( nullptr );
  m_dreIndicatorDynamic.SetWinRowElement( nullptr );
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

void PriceRow::SetProfitLoss( double pl ) {
  m_dreAcctPl.Set( pl );
}

} // market depth
} // namespace tf
} // namespace ou

/* from gscalp, for some additional todos
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