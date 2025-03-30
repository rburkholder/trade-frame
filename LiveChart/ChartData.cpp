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

#include <TFTrading/InstrumentManager.h>

#include "ChartData.h"

ChartData::ChartData( pProvider_t pProvider )
  : ou::ChartDVBasics()
{
  //static const std::string name( "QGC#" );
  //static const std::string name( "GLD" );
  static const std::string name( "TST$Y" );
  this->GetChartDataView()->SetNames( "LiveChart", name );
  ou::tf::Instrument::pInstrument_t pInstrument
//    = ou::tf::InstrumentManager::GlobalInstance().ConstructInstrument( name, "SMART", ou::tf::InstrumentType::Stock );
    = ou::tf::InstrumentManager::GlobalInstance().ConstructInstrument( name, "SMART", ou::tf::InstrumentType::Future );
  m_pWatch = new ou::tf::Watch( pInstrument, pProvider );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
  m_pWatch->StartWatch();

}

ChartData::~ChartData(void) {
  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
  delete m_pWatch;
}

