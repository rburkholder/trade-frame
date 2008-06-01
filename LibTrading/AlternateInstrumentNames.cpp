#include "StdAfx.h"
#include "AlternateInstrumentNames.h"

CAlternateInstrumentNames::CAlternateInstrumentNames(void) : CCommonDatabaseFunctions<CAlternateInstrumentNames>( "AlternateInstrumentNames" ) {
}

CAlternateInstrumentNames::~CAlternateInstrumentNames(void) {
}

void CAlternateInstrumentNames::Save(const std::string &Key, unsigned short id, const std::string &Alternate) {
  structKey key( id, Key );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxSymbolNameSize + key.nNameLength );
  structValue value( Alternate );
  Dbt v( (void*) &value, sizeof( structKey ) - nMaxSymbolNameSize + value.nValueLength );
  int ret = m_pdb->put( 0, &k, &v, 0 );
  if ( 0 != ret ) throw std::runtime_error( "CAlternateInstrumentNames::Save put had error" );
}

void CAlternateInstrumentNames::Get(const std::string &Key, unsigned short id, std::string *pAlternate) {
  structKey key( id, Key );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxSymbolNameSize + key.nNameLength );
  Dbt v;
  int ret = m_pdb->get( 0, &k, &v, 0 );
  if ( DB_NOTFOUND == ret ) throw std::out_of_range( "CAlternateInstrumentNames::Get key not found" );
  if ( 0 != ret ) throw std::runtime_error( "CAlternateInstrumentNames::Get get had error" );
  structValue *p = (structValue *) v.get_data();
  pAlternate->assign( (*p).Value, (*p).nValueLength );
}
