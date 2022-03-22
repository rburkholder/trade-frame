/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    OptionTracker.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: March 21, 2022 23:15
 */

#include "OptionTracker.hpp"

void OptionTracker::Add() {

  if ( !m_bActive ) {
    m_bActive = true;
    m_pOption->OnQuote.Add( MakeDelegate( this, &OptionTracker::HandleQuote ) );
    switch ( m_pOption->GetOptionSide() ) {
      case ou::tf::OptionSide::Call:
        m_pOption->OnTrade.Add( MakeDelegate( this, &OptionTracker::HandleTradeCall ) );
        break;
      case ou::tf::OptionSide::Put:
        m_pOption->OnTrade.Add( MakeDelegate( this, &OptionTracker::HandleTradePut ) );
        break;
      default:
        assert( false );
        break;
    }

    m_ceTrade.SetColour( ou::Colour::Green );
    m_ceTrade.SetName( "Tick" );
    m_ceQuoteAsk.SetColour( ou::Colour::Red );
    m_ceQuoteAsk.SetName( "Ask" );
    m_ceQuoteBid.SetColour( ou::Colour::Blue );
    m_ceQuoteBid.SetName( "Bid" );
    m_ceVolumeUp.SetColour( ou::Colour::Green );
    m_ceVolumeUp.SetName( "Buy" );
    m_ceVolumeDn.SetColour( ou::Colour::Red );
    m_ceVolumeDn.SetName( "Sell" );
    m_ceSpread.SetName( "Spread" );

    m_dvChart.Add( 0, &m_ceQuoteAsk );
    m_dvChart.Add( 0, &m_ceQuoteBid );
    m_dvChart.Add( 0, &m_ceTrade );
    m_dvChart.Add( 1, &m_ceVolumeUp );
    m_dvChart.Add( 1, &m_ceVolumeDn );
    m_dvChart.Add( 2, &m_ceSpread );

    m_pOption->StartWatch();
  }
}

void OptionTracker::Del() {
  if ( m_bActive ) {
    m_pOption->StopWatch();
    m_pOption->OnQuote.Remove( MakeDelegate( this, &OptionTracker::HandleQuote ) );
    switch ( m_pOption->GetOptionSide() ) {
      case ou::tf::OptionSide::Call:
        m_pOption->OnTrade.Remove( MakeDelegate( this, &OptionTracker::HandleTradeCall ) );
        break;
      case ou::tf::OptionSide::Put:
        m_pOption->OnTrade.Remove( MakeDelegate( this, &OptionTracker::HandleTradePut ) );
        break;
      default:
        assert( false );
        break;
    }
    m_bActive = false;
  }
}

OptionTracker::OptionTracker(
  pOption_t pOption_
, ou::ChartEntryShape& ceBullCall, ou::ChartEntryShape& ceBullPut
, ou::ChartEntryShape& ceBearCall, ou::ChartEntryShape& ceBearPut
)
: m_bActive( false ), m_pOption( pOption_ )
, m_volCallBuy {}, m_volCallSell {}, m_volPutBuy {}, m_volPutSell {}
, m_ceBullCall( ceBullCall ), m_ceBullPut( ceBullPut )
, m_ceBearCall( ceBearCall ), m_ceBearPut( ceBearPut )
{
  Add();
  std::cout << "option " << m_pOption->GetInstrumentName() << " added" << std::endl;
}

OptionTracker::OptionTracker( const OptionTracker& rhs )
: m_bActive( false ), m_pOption( rhs.m_pOption )
, m_volCallBuy( rhs.m_volCallBuy ), m_volCallSell( rhs.m_volCallSell)
, m_volPutBuy( rhs.m_volPutBuy ), m_volPutSell( rhs.m_volPutSell)
, m_ceBullCall( rhs.m_ceBullCall ), m_ceBullPut( rhs.m_ceBullPut )
, m_ceBearCall( rhs.m_ceBearCall ), m_ceBearPut( rhs.m_ceBearPut )
{
  Add();
}

OptionTracker::OptionTracker( OptionTracker&& rhs )
: m_bActive( false )
, m_volCallBuy( rhs.m_volCallBuy ), m_volCallSell( rhs.m_volCallSell)
, m_volPutBuy( rhs.m_volPutBuy ), m_volPutSell( rhs.m_volPutSell)
, m_ceBullCall( rhs.m_ceBullCall ), m_ceBullPut( rhs.m_ceBullPut )
, m_ceBearCall( rhs.m_ceBearCall ), m_ceBearPut( rhs.m_ceBearPut )
{
  rhs.Del();
  m_pOption = std::move( rhs.m_pOption );
  Add();
};

OptionTracker::~OptionTracker() {
  Del();
  m_pOption.reset();
}

void OptionTracker::HandleQuote( const ou::tf::Quote& quote ) {
  m_ceQuoteAsk.Append( quote.DateTime(), quote.Ask() );
  m_ceQuoteBid.Append( quote.DateTime(), quote.Bid() );
  m_ceSpread.Append( quote.DateTime(), quote.Ask() - quote.Bid() );
}

void OptionTracker::HandleTradeCall( const ou::tf::Trade& trade ) {
  const double price = trade.Price();
  const ou::tf::Quote& quote( m_pOption->LastQuote() );
  const double mid = quote.Midpoint();
  m_ceTrade.Append( trade.DateTime(), price );
  if ( ( price == mid ) || ( quote.Bid() == quote.Ask() ) ) {
    // can't really say, will need to check if bid came to ask or ask came to bid
  }
  else {
    ou::tf::Trade::volume_t volume = trade.Volume();
    if ( price > mid ) {
      m_volCallBuy += volume;
      m_ceBullCall.AddLabel( trade.DateTime(), m_pOption->GetStrike(), "C" );
      m_ceVolumeUp.Append( trade.DateTime(), volume );
      m_ceVolumeDn.Append( trade.DateTime(), 0 );
    }
    else {
      m_volCallSell += volume;
      m_ceBearCall.AddLabel( trade.DateTime(), m_pOption->GetStrike(), "C" );
      m_ceVolumeUp.Append( trade.DateTime(), 0 );
      m_ceVolumeDn.Append( trade.DateTime(), volume );
    }
  }
}

void OptionTracker::HandleTradePut( const ou::tf::Trade& trade ) {
  const double price = trade.Price();
  const ou::tf::Quote& quote( m_pOption->LastQuote() );
  const double mid = quote.Midpoint();
  m_ceTrade.Append( trade.DateTime(), price );
  if ( ( price == mid ) || ( quote.Bid() == quote.Ask() ) ) {
    // can't really say, will need to check if bid came to ask or ask came to bid
  }
  else {
    ou::tf::Trade::volume_t volume = trade.Volume();
    if ( price > mid ) {
      m_volPutBuy += volume;
      m_ceBearPut.AddLabel( trade.DateTime(), m_pOption->GetStrike(), "P" );
      m_ceVolumeUp.Append( trade.DateTime(), volume );
      m_ceVolumeDn.Append( trade.DateTime(), 0 );
    }
    else {
      m_volPutSell += volume;
      m_ceBullPut.AddLabel( trade.DateTime(), m_pOption->GetStrike(), "P" );
      m_ceVolumeUp.Append( trade.DateTime(), 0 );
      m_ceVolumeDn.Append( trade.DateTime(), volume );
    }
  }
}

void OptionTracker::SaveWatch( const std::string& sPrefix ) {
  m_pOption->SaveSeries( sPrefix );
}

void OptionTracker::Emit(
  ou::tf::Trade::volume_t& volCallBuy, ou::tf::Trade::volume_t& volCallSell
, ou::tf::Trade::volume_t& volPutBuy,  ou::tf::Trade::volume_t& volPutSell
) {
  ou::tf::Quote quote( m_pOption->LastQuote() );
  volCallBuy += m_volCallBuy;
  volCallSell += m_volCallSell;
  volPutBuy += m_volPutBuy;
  volPutSell += m_volPutSell;
  std::cout <<
        m_pOption->GetInstrumentName()
    << ": b=" << quote.Bid()
    << ",a=" << quote.Ask()
    //<< ",oi=" << m_pOption->GetFundamentals().nOpenInterest // not available
    << ",cbv=" << m_volCallBuy
    << ",csv=" << m_volCallSell
    << ",pbv=" << m_volPutBuy
    << ",psv=" << m_volPutSell
    << std::endl;
}

