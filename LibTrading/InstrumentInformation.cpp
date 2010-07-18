/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "InstrumentInformation.h"

CInstrumentInformation::CInstrumentInformation(void) : CCommonDatabaseFunctions<CInstrumentInformation>( "InstrumentInformation" ) {
}

CInstrumentInformation::~CInstrumentInformation(void) {
}

void CInstrumentInformation::Save(const std::string &sSymbol, const std::string &sCompanyName, 
                                  float fltPriceEarnings, float fltEarningsPerShare, 
                                  float fltDividend, boost::posix_time::ptime dtLastDividend, 
                                  float fltPctInstitutional, float fltSharesOutstanding, 
                                  float fltAssets, float fltLiabilities, 
                                  float fltLongTermDebt, boost::posix_time::ptime dtBalanceSheet, 
                                  char nFormatCode, char nPrecision, int nSIC, 
                                  char nSecurityType, char nListedMarket) {
  structKey key( sSymbol.size(), sSymbol.c_str() );
  structValues values( fltPriceEarnings, fltDividend, dtLastDividend, fltEarningsPerShare, 
    fltPctInstitutional, fltAssets, fltLiabilities, dtBalanceSheet, fltLongTermDebt,
    fltSharesOutstanding, nFormatCode, nPrecision, nSIC, nSecurityType, nListedMarket, sCompanyName );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxSymbolNameSize + key.nKeySize );
  Dbt v( (void*) &values, sizeof( structValues ) - nMaxCompanyNameSize + values.nCompanyNameSize );
  int ret = m_pdb->put( 0, &k, &v, 0 );
  if ( 0 != ret ) throw std::runtime_error( "CInstrumentInformation::Save put had error" );
}

// use Dbt v concept from CInstrumentFile::CreateInstrumentFromIQFeed
void CInstrumentInformation::Retrieve(const std::string &sSymbol) {
  structKey key( sSymbol.size(), sSymbol.c_str() );
  Dbt k(  (void*) &key, sizeof( structKey ) - nMaxSymbolNameSize + key.nKeySize );
  Dbt v;
  int ret = m_pdb->get( 0, &k, &v, 0 );
  if ( DB_NOTFOUND == ret ) throw std::out_of_range( "CInstrumentInformation::Retrieve key not found" );
  if ( 0 != ret ) throw std::runtime_error( "CInstrumentInformation::Retrieve get had error" );
  m_pValues = (structValues *) v.get_data();
}


