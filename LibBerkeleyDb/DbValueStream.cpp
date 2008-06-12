#include "StdAfx.h"
#include "DbValueStream.h"

CDbValueStream::CDbValueStream(void) 
: std::streambuf(), CCommonDatabaseFunctions<CDbValueStream>( "BasicBasket" )
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

