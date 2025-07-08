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

#include <TFTrading/Instrument.h>
//#include <TFTrading/InstrumentManager.h>

#include "ChartData.hpp"

ChartData::ChartData( pProvider_t pProvider, const std::string& sSymbolName )
: ou::ChartDVBasics()
{
  this->GetChartDataView()->SetNames( "LiveChart", sSymbolName );
  //ou::tf::Instrument::pInstrument_t pInstrument
  //  = ou::tf::InstrumentManager::GlobalInstance().ConstructInstrument( sSymbolName, "SMART", ou::tf::InstrumentType::Stock );
  // = ou::tf::InstrumentManager::GlobalInstance().ConstructInstrument( sSymbolName, "SMART", ou::tf::InstrumentType::Future );
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sSymbolName ); // suffices for IQFeed instrument
  m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, pProvider );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
  m_pWatch->StartWatch();
}

void ChartData::SaveSeries( const std::string& sPrefix ) {
  if ( m_pWatch ) {
    m_pWatch->SaveSeries( sPrefix );
  }
}

ChartData::~ChartData() {
  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
  m_pWatch.reset();
}

