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

#include "DbValueStream.h"

CDbValueStream::CDbValueStream( const std::string& sDbFileName ) 
: std::streambuf(), CCommonDatabaseFunctions<CDbValueStream>( sDbFileName, "BasicBasket" )
{
  setp( buf, buf + BufSize );
}

CDbValueStream::~CDbValueStream(void) {
}

int CDbValueStream::overflow(int_type meta) {
  throw std::runtime_error( "CDbValueStream overflow" );
}

void CDbValueStream::Save(void *pKey, size_t nKeyLength) {
  Dbt key( pKey, nKeyLength );
  Dbt value( pbase(), pptr() - pbase() );
  CCommonDatabaseFunctions<CDbValueStream>::Save( &key, &value );
}

int CDbValueStream::sync() {
  setp( pbase(), epptr() );
  return 0;
}

