/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    TickStreamModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: February 1, 2026 10:42:46
 */

#include "OUCommon/Colour.h"

#include "TickStreamModel.hpp"

TickStreamModel::TickStreamModel( pWatch_t& pWatch ) {

  m_ceAsk.SetColour( ou::Colour::Red );
  m_ceAsk.SetName( "Ask" );

  m_ceTrade.SetColour( ou::Colour::Green );
  m_ceTrade.SetName( "Tick" );

  m_ceBid.SetColour( ou::Colour::Blue );
  m_ceBid.SetName( "Bid" );

  m_dvChart.Add( EChartSlot::Price, &m_ceAsk );
  m_dvChart.Add( EChartSlot::Price, &m_ceTrade );
  m_dvChart.Add( EChartSlot::Price, &m_ceBid );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolume );

  m_pWatch = pWatch;
  m_pWatch->OnTrade.Add( MakeDelegate( this, &TickStreamModel::HandleTrade ) );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &TickStreamModel::HandleQuote ) );
  m_pWatch->StartWatch();
}

TickStreamModel::~TickStreamModel() {
  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &TickStreamModel::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &TickStreamModel::HandleTrade ) );
  m_pWatch.reset();
}

void TickStreamModel::HandleTrade( const ou::tf::Trade& trade ) {
  m_ceTrade.Append( trade.DateTime(), trade.Price() );
  m_ceVolume.Append( trade.DateTime(), trade.Volume() );
}

void TickStreamModel::HandleQuote( const ou::tf::Quote& quote ) {
  m_ceAsk.Append( quote.DateTime(), quote.Ask() );
  m_ceBid.Append( quote.DateTime(), quote.Bid() );
}

