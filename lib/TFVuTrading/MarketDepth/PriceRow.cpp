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

PriceRow::PriceRow( double price )
: m_bChanged( false )
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
: m_bChanged( false )
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

void PriceRow::Set( fMouseClick_t&& fLeft, fMouseClick_t&& fRight ) {
  m_fMouseClick_Left = std::move( fLeft );
  m_fMouseClick_Right = std::move( fRight );
  m_dreBidOrder.Set(
    [this](){
      if ( m_fMouseClick_Left ) {
        m_fMouseClick_Left( m_drePrice.Get(), EField::BidOrder );
      }
    },
    [this](){
      if ( m_fMouseClick_Right ) {
        m_fMouseClick_Right( m_drePrice.Get(), EField::BidOrder );
      }
    }
  );
  m_dreAskOrder.Set(
    [this](){
      if ( m_fMouseClick_Left ) {
        m_fMouseClick_Left( m_drePrice.Get(), EField::AskOrder );
      }
    },
    [this](){
      if ( m_fMouseClick_Right ) {
        m_fMouseClick_Right( m_drePrice.Get(), EField::AskOrder );
      }
    }
  );
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