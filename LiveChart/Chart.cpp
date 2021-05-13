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

#include "stdafx.h"

#include <TFTrading/InstrumentManager.h>

#include "Chart.h"

ChartTest::ChartTest( pProvider_t pProvider )
  : ou::ChartDVBasics()
{
  //static const std::string name( "QGC#" );
  static const std::string name( "GLD" );
  this->GetChartDataView()->SetNames( "LiveChart", name );
  ou::tf::Instrument::pInstrument_t pInstrument
    = ou::tf::InstrumentManager::Instance().ConstructInstrument( name, "SMART", ou::tf::InstrumentType::Stock );
//    = ou::tf::InstrumentManager::Instance().ConstructInstrument( name, "SMART", ou::tf::InstrumentType::Future );
  m_pWatch = new ou::tf::Watch( pInstrument, pProvider );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
  m_pWatch->StartWatch();

}

ChartTest::~ChartTest(void) {
  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
  delete m_pWatch;
}

// C:\Data\Projects\VSC++\TradeFrame\LiveChart\Chart.cpp