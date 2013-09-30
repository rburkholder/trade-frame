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

#include <TFTrading/InstrumentManager.h>

#include "Chart.h"

ChartTest::ChartTest( pProvider_t pProvider ) 

{
  ou::tf::Instrument::pInstrument_t pInstrument
    = ou::tf::InstrumentManager::Instance().ConstructInstrument( "+GC#", "SMART", ou::tf::InstrumentType::Future );
  m_pWatch = new ou::tf::Watch( pInstrument, pProvider );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &ou::ChartDataBase::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &ou::ChartDataBase::HandleTrade ) );
  m_pWatch->StartWatch();

}
 
ChartTest::~ChartTest(void) {
  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &ou::ChartDataBase::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &ou::ChartDataBase::HandleTrade ) );
}

