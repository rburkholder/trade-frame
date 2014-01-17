/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "Strategy1.h"

// ideas
/*
* run puts on the high side, and calls on the low side
* bollinger bands need to be 2.5 x spread wide
* then on big moves, can close out pairs as profitable strangles

* count number of band hits
* enumeration of which side was last

* portfolio long
* portfolio short

* close positions when profitable
* track number of open positions

* need slope generator, so can tell if moving average is going up versuus going down
* then trade with the trend, starting on the band

* pick up three on each side then start pruning back when possible
*/

Strategy::Strategy( ou::tf::option::MultiExpiryBundle* meb ) 
  : m_pBundle( meb )
{

  boost::gregorian::date date( ou::TimeSource::Instance().External().date() );
  InitForUS24HourFutures( date );
  SetStartTrading( ptime( date, time_duration( 18, 30, 0 ) ) );

  for ( int ix = 0; ix <= 3; ++ix ) {
    m_vBollingerState.push_back( eBollingerUnknown );
  }

  m_bThreadPopDatumsActive = true;
  m_pThreadPopDatums = new boost::thread( &Strategy::ThreadPopDatums, this );

//  m_pBundle->GetWatchUnderlying()->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuoteUnderlying ) );
//  m_pBundle->GetWatchUnderlying()->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTradeUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnQuote.Add( MakeDelegate( this, &Strategy::HandleInboundQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnTrade.Add( MakeDelegate( this, &Strategy::HandleInboundTradeUnderlying ) );

  m_pBundle->AddOnAtmIv( MakeDelegate( this, &Strategy::HandleCalcIv ) );

}

Strategy::~Strategy(void) {

  m_pBundle->RemoveOnAtmIv( MakeDelegate( this, &Strategy::HandleCalcIv ) );

  m_pBundle->GetWatchUnderlying()->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleInboundQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleInboundTradeUnderlying ) );
//  m_pBundle->GetWatchUnderlying()->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuoteUnderlying ) );
//  m_pBundle->GetWatchUnderlying()->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTradeUnderlying ) );
  // after these are removed, may need to wait for the pushing thread to finish before we destruct Strategy.

  m_bThreadPopDatumsActive = false;
  m_cvCrossThreadDatums.notify_one();
  m_pThreadPopDatums->join();
  delete m_pThreadPopDatums;
  m_pThreadPopDatums = 0;

}

// two threads used:
//  * thread to update vectors and calculate the trade signals
//  * thread to use data from updated vectors and draw charts (a long lived process)
// biggest issue is that vectors can not be re-allocated during charting process.
// put vectors into a visitor process, which can be locked?  
// lock the trade-signals thread and the charting thread

void Strategy::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
  // need to queue this from the originating thread.
  //m_ChartDataUnderlying.HandleTrade( trade );
  ou::ChartDataBase::HandleTrade( trade );
  TimeTick( trade );
}

void Strategy::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
  // need to queue this from the originating thread.
  if ( !quote.IsValid() ) {
    return;
  }
  //m_ChartDataUnderlying.HandleQuote( quote );
  ou::ChartDataBase::HandleQuote( quote );
  TimeTick( quote );
}

void Strategy::ThreadPopDatums( void ) {
  boost::unique_lock<boost::mutex> lock(m_mutexCrossThreadDatums);
  while ( m_bThreadPopDatumsActive ) {
    m_cvCrossThreadDatums.wait( lock );
    EDatumType type;
    while ( m_lfDatumType.pop( type ) ) {
      switch ( type ) {
      case EDatumQuote: {
        ou::tf::Quote quote;
        if ( m_lfQuote.pop( quote ) ) {
          HandleQuoteUnderlying( quote );
        }
                        }
        break;
      case EDatumTrade: {
        ou::tf::Trade trade;
        if ( m_lfTrade.pop( trade ) ) {
          HandleTradeUnderlying( trade );
        }
                        }
        break;
      }
    }
  }
}

void Strategy::HandleInboundQuoteUnderlying( const ou::tf::Quote& quote ) {
  bool bErrored( false );
  while ( !m_lfQuote.push( quote ) ) {
    if ( !bErrored ) {
      bErrored = true;
      std::cout << "m_lfQuote is full" << std::endl;
    }
  }
  while ( !m_lfDatumType.push( EDatumQuote ) );
  m_cvCrossThreadDatums.notify_one();
}

void Strategy::HandleInboundTradeUnderlying( const ou::tf::Trade& trade ) {
  bool bErrored( false );
  while ( !m_lfTrade.push( trade ) ) {
    if ( !bErrored ) {
      bErrored = true;
      std::cout << "m_lfTrade is full" << std::endl;
    }
  }
  while ( !m_lfDatumType.push( EDatumTrade ) );
  m_cvCrossThreadDatums.notify_one();
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  if ( quote.IsValid() ) {
    double mid = quote.Midpoint();

    for ( int ix = 0; ix <=3; ++ix ) {
    }
  }
}

void Strategy::HandleCommon( const ou::tf::Quote& quote ) {
}

void Strategy::HandleCommon( const ou::tf::Trade& trade ) {
}

void Strategy::HandleCalcIv( const ou::tf::PriceIV& iv ) {
  mapAtmIv_t::iterator iter = m_mapAtmIv.find( iv.Expiry() );
  if ( m_mapAtmIv.end() == iter ) {
    BundleAtmIv bai;
    switch ( m_mapAtmIv.size() ) {
    case 0: 
      bai.m_pceCallIV->SetColour( ou::Colour::RosyBrown );
      bai.m_pcePutIV->SetColour( ou::Colour::MediumOrchid );
      break;
    case 1:
      bai.m_pceCallIV->SetColour( ou::Colour::Cyan );
      bai.m_pcePutIV->SetColour( ou::Colour::PaleGreen );
      break;
    }
    std::stringstream ss;
    ss << iv.Expiry();
    bai.m_pceCallIV->SetName( ss.str() + " call" );
    bai.m_pcePutIV->SetName( ss.str() + " put" );
    m_mapAtmIv.insert( mapAtmIv_t::value_type( iv.Expiry(), bai ) );
    //m_ChartDataUnderlying.GetChartDataView().Add( 3, bai.m_pceCallIV.get() );
    //m_ChartDataUnderlying.GetChartDataView().Add( 3, bai.m_pcePutIV.get() );
    ou::ChartDataBase::GetChartDataView().Add( 3, bai.m_pceCallIV.get() );
    ou::ChartDataBase::GetChartDataView().Add( 3, bai.m_pcePutIV.get() );
  }
  else {
    iter->second.m_pceCallIV->Append( iv.DateTime(), iv.IVCall() );
    iter->second.m_pcePutIV->Append( iv.DateTime(), iv.IVPut() );
  }
}

