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
 * File:    ChartData.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l1
 * Created: February 6, 2022 16:15
 * retrofit back to LiveChart
 */

#include <memory>

#include <TFTrading/Instrument.h>

#include "ChartData.h"

ChartData::ChartData( pProvider_t pProvider, const std::string& sIQFeedSymbol )
: ou::ChartDVBasics()
{
  GetChartDataView()->SetNames( "ChartData", sIQFeedSymbol );

  ou::tf::Instrument::pInstrument_t pInstrument;
  pInstrument = std::make_shared<ou::tf::Instrument>( sIQFeedSymbol ); // simple for an iqfeed watch
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sIQFeedSymbol );
  m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, pProvider ); // will need to be iqfeed provider, check?

  m_pWatch->OnQuote.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
  m_pWatch->StartWatch();

}

ChartData::~ChartData(void) {
  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &ou::ChartDVBasics::HandleTrade ) );
}

// C:\Data\Projects\VSC++\TradeFrame\LiveChart\Chart.cpp