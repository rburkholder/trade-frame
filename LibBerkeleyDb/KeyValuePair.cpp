#include "StdAfx.h"
#include "KeyValuePair.h"

CKeyValuePair::CKeyValuePair(void) : CCommonDatabaseFunctions<CKeyValuePair>( "KeyValuePairs" ) {
}

CKeyValuePair::~CKeyValuePair(void) {
}

void CKeyValuePair::Save( const structKey &key, const structValue &value ) {
  assert( 0 < key.nKeySize );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeySize );
  Dbt v( value.pAddr, value.nSize );
  int ret;
  try {
    ret = m_pdb->put( 0, &k, &v, 0 ); // overwrite existing value
  }
  catch ( DbException e ) {
    string err( "CKeyValuePair::Save: DbException error " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( 0 != ret ) throw std::runtime_error( "CKeyValuePair::Save put had error" );
}

void CKeyValuePair::Save( const structKey &key, Dbt *pValue ) {
  assert( 0 < key.nKeySize );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeySize );
  int ret;
  try {
    ret = m_pdb->put( 0, &k, pValue, 0 ); // overwrite existing value
  }
  catch ( DbException e ) {
    string err( "CKeyValuePair::Save: DbException error " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( 0 != ret ) throw std::runtime_error( "CKeyValuePair::Save put had error" );
}

void CKeyValuePair::Get( const structKey &key, void **pVoid, size_t *pSize ) {
  assert( 0 < key.nKeySize );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeySize );
  Dbt v;
  int ret;
  try {
    ret = m_pdb->get( 0, &k, &v, 0 );
  }
  catch ( DbException e ) {
    string err( "CKeyValuePair::Get: DbException error " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( DB_NOTFOUND == ret ) {
    throw std::out_of_range( "CKeyValuePair::Get key not found" );
  }
  if ( 0 != ret ) throw std::runtime_error( "CKeyValuePair::Get get had error" );
  structValue *p = (structValue *) v.get_data();
  *pVoid = p->pAddr;
  *pSize = p->nSize;
}

void CKeyValuePair::Get( const structKey &key, Dbt *pValue ) {
  assert( 0 < key.nKeySize );
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeySize );
  int ret;
  try {
    ret = m_pdb->get( 0, &k, pValue, 0 );
  }
  catch ( DbException e ) {
    string err( "CKeyValuePair::Get: DbException error " );
    err.append( e.what() );
    throw std::domain_error( err );
  }
  if ( DB_NOTFOUND == ret ) {
    throw std::out_of_range( "CKeyValuePair::Get key not found" );
  }
  if ( 0 != ret ) throw std::runtime_error( "CKeyValuePair::Get get had error" );
}

void CKeyValuePair::Save(const std::string &key, char value) {
  structKey k( Char, key.size(), key.c_str() );
  //structValue v( &value, sizeof( char ) );
  Dbt v( &value, sizeof( char ) );
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, const std::string &value) {
  structKey k( String, key.size(), key.c_str() );
  //structValue v( (void*) value.c_str(), (u_int32_t) value.size() );
  Dbt v( (void*) value.c_str(), value.size() );
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, short value) {
  structKey k( Int16, key.size(), key.c_str() );
  //structValue v( &value, sizeof( short ) );
  Dbt v( &value, sizeof( short ) );
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, unsigned short value) {
  structKey k( UInt16, key.size(), key.c_str() );
  //structValue v( &value, sizeof( unsigned short ) );
  Dbt v( &value, sizeof( unsigned short ) );
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, long value) {
  structKey k( Int32, key.size(), key.c_str() );
  //structValue v( &value, sizeof( long ) );
  Dbt v( &value, sizeof( long ) );
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, unsigned long value) {
  structKey k( UInt32, key.size(), key.c_str() );
  //structValue v( &value, sizeof( unsigned long ) );
  Dbt v( &value, sizeof( unsigned long ) );
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, float value) {
  structKey k( Float, key.size(), key.c_str() );
  //structValue v( &value, sizeof( float ) );
  Dbt v( &value, sizeof( float ) ) ;
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, double value) {
  structKey k( Double, key.size(), key.c_str() );
  //structValue v( &value, sizeof( double ) );
  Dbt v( &value, sizeof( double ) );
  Save( k, &v );
}

void CKeyValuePair::Save(const std::string &key, const structValue &value) {
  structKey k( Blob, key.size(), key.c_str() );
  Save( k, value );
}

char CKeyValuePair::GetChar(const std::string &key) {
  structKey k( Char, key.size(), key.c_str() );
  size_t size;
  void *q;
  Get( k, &q, &size );
  assert( size == sizeof( char ) );
  char *p = (char*) q;
  return (char) *p;
}

void CKeyValuePair::GetString(const std::string &key, std::string *pvalue) {
  structKey k( String, key.size(), key.c_str() );
  size_t size;
  void *q;
  Get( k, &q, &size );
  char *p = (char*) q;
  pvalue->assign( p, size );
  return;
}

short CKeyValuePair::GetShort(const std::string &key) {
  structKey k( Int16, key.size(), key.c_str() );
  Dbt v;
  short value;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( short ) );
  v.set_size( sizeof( short ) );
  v.set_data( &value );  
  Get( k, &v );
  return value;
}

unsigned short CKeyValuePair::GetUnsignedShort(const std::string &key) {
  structKey k( UInt16, key.size(), key.c_str() );
  Dbt v;
  unsigned short value;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( unsigned short ) );
  v.set_size( sizeof( unsigned short ) );
  v.set_data( &value );  
  Get( k, &v );
  return value;
}

long CKeyValuePair::GetLong(const std::string &key) {
  structKey k( Int32, key.size(), key.c_str() );
  Dbt v;
  long value;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( long ) );
  v.set_size( sizeof( long ) );
  v.set_data( &value );  
  Get( k, &v );
  return value;
}

unsigned long CKeyValuePair::GetUnsignedLong(const std::string &key) {
  structKey k( UInt32, key.size(), key.c_str() );
  Dbt v;
  unsigned long value;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( unsigned long ) );
  v.set_size( sizeof( unsigned long ) );
  v.set_data( &value );  
  Get( k, &v );
  return value;
}

float CKeyValuePair::GetFloat(const std::string &key) {
  structKey k( Float, key.size(), key.c_str() );
  Dbt v;
  float value;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( float ) );
  v.set_size( sizeof( float ) );
  v.set_data( &value );  
  Get( k, &v );
  return value;
}

double CKeyValuePair::GetDouble(const std::string &key) {
  structKey k( Double, key.size(), key.c_str() );
  Dbt v;
  double value;
  v.set_flags( DB_DBT_USERMEM );
  v.set_ulen( sizeof( double ) );
  v.set_size( sizeof( double ) );
  v.set_data( &value );  
  Get( k, &v );
  return value;
}

CKeyValuePair::structValue CKeyValuePair::GetBlob(const std::string &key) {
  structKey k( Blob, key.size(), key.c_str() );
  u_int32_t size;
  void *p;
  Get( k, &p, &size );
  structValue v( p, size );
  return v;
}

