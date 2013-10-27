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


Strategy::Strategy( ou::tf::option::MultiExpiryBundle* meb ) 
  : m_pBundle( meb )
{

  m_pBundle->GetWatchUnderlying()->OnQuote.Add( MakeDelegate( this, &Strategy::HandleQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnTrade.Add( MakeDelegate( this, &Strategy::HandleTradeUnderlying ) );

}

Strategy::~Strategy(void) {
  m_pBundle->GetWatchUnderlying()->OnQuote.Remove( MakeDelegate( this, &Strategy::HandleQuoteUnderlying ) );
  m_pBundle->GetWatchUnderlying()->OnTrade.Remove( MakeDelegate( this, &Strategy::HandleTradeUnderlying ) );
}

void Strategy::HandleTradeUnderlying( const ou::tf::Trade& trade ) {
  m_ChartDataUnderlying.HandleTrade( trade );
  TimeTick( trade );
}

void Strategy::HandleQuoteUnderlying( const ou::tf::Quote& quote ) {
  if ( !quote.IsValid() ) {
    return;
  }
  m_ChartDataUnderlying.HandleQuote( quote );
  TimeTick( quote );
}

void Strategy::HandleRHTrading( const ou::tf::Quote& quote ) {
}

void Strategy::HandleCommon( const ou::tf::Quote& quote ) {
}

void Strategy::HandleCommon( const ou::tf::Trade& trade ) {
}

void Strategy::HandleCalcIv( const ou::tf::PriceIV& iv ) {
  mapAtmIv_t::iterator iter = m_mapAtmIv.find( iv.Expiry() );
  if ( m_mapAtmIv.end() == iter ) {
    BundleAtmIv bai;
    bai.m_pceCallIV->SetColour( ou::Colour::RosyBrown );
    bai.m_pcePutIV->SetColour( ou::Colour::MediumOrchid );
    m_mapAtmIv.insert( mapAtmIv_t::value_type( iv.Expiry(), bai ) );
    m_ChartDataUnderlying.GetChartDataView().Add( 3, bai.m_pceCallIV.get() );
    m_ChartDataUnderlying.GetChartDataView().Add( 3, bai.m_pcePutIV.get() );
  }
  else {
    iter->second.m_pceCallIV->Append( iv.DateTime(), iv.IVCall() );
    iter->second.m_pcePutIV->Append( iv.DateTime(), iv.IVPut() );
  }
}

