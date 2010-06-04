/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <algorithm>

#include <boost/foreach.hpp>

#include "Process.h"

CProcess::CProcess(void)
: CIQFeedHistoryBulkQuery<CProcess>()
{
  m_vExchanges.push_back( "NYSE" );
  m_vExchanges.push_back( "NYSE_AMEX" );
  m_vExchanges.push_back( "NYSE,ARCA" );
  m_vExchanges.push_back( "NASDAQ" );
  m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
}

CProcess::~CProcess(void) {
}

void CProcess::Start( void ) {

  SetExchanges( m_vExchanges );
  SetMaxSimultaneousQueries( 100 );
  DailyBars( 200 );

}

void CProcess::OnBars( inherited_t::structResultBar* bars ) {
  std::string s = "bars for " + bars->sSymbol + " done\n";
  OutputDebugString( s.c_str() );
}

void CProcess::OnTicks( inherited_t::structResultTicks* ticks ) {
//  OutputDebugString( "ticks for " + ticks->sSymbol + " done\n" );
}

void CProcess::OnCompletion( void ) {
  OutputDebugString( "all processing complete\n" );
}

