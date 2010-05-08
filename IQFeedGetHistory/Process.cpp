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

#include <boost/foreach.hpp>

#include "Process.h"

CProcess::CProcess(void) {
  m_vExchanges.push_back( "NYSE" );
  m_vExchanges.push_back( "NYSE_AMEX" );
  //m_vExchanges.push_back( "NYSE,ARCA" );
  //m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
}

CProcess::~CProcess(void) {
}

void CProcess::Start( void ) {
  m_IF.OpenIQFSymbols();

  structSymbolRecord *pRec;

  BOOST_FOREACH( std::string s, m_vExchanges ) {
    m_iterSymbols.SetInstrumentFile( &m_IF );
    pRec = m_iterSymbols.begin( s );
    while ( m_iterSymbols.end() != pRec ) {
      m_vSymbols.push_back( pRec->GetSymbol() );
      pRec = ++m_iterSymbols;
    }
  }

  m_IF.CloseIQFSymbols();
}