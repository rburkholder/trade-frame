#include "StdAfx.h"
#include "KeyValuePair.h"

CKeyValuePair::CKeyValuePair(void) : CCommonDatabaseFunctions<CKeyValuePair>( "KeyValuePairs" ) {
}

CKeyValuePair::~CKeyValuePair(void) {
}

void CKeyValuePair::Save( const structKey &key,  const structValue &value ) {
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeySize );
  Dbt v( value.pAddr, value.nSize );
  int ret = m_pdb->put( 0, &k, &v, 0 ); // overwrite existing value
  if ( 0 != ret ) throw std::runtime_error( "CKeyValuePair::Save put had error" );
}

// redo this, don't need the return of the key, it will only get what we want
void CKeyValuePair::Get( const structKey &key, void **pVoid, size_t *pSize ) {
  Dbt k( (void*) &key, sizeof( structKey ) - nMaxKeySize + key.nKeySize );
  Dbt v;
  int ret = m_pdb->get( 0, &k, &v, 0 );
  if ( DB_NOTFOUND == ret ) throw std::out_of_range( "CKeyValuePair::Get key not found" );
  if ( 0 != ret ) throw std::runtime_error( "CKeyValuePair::Get get had error" );
  structValue *p = (structValue *) v.get_data();
  *pVoid = p->pAddr;
  *pSize = p->nSize;
}

void CKeyValuePair::Save(const std::string &key, char value) {
  structKey k( Char, key.size(), key.c_str() );
  structValue v( &value, sizeof( char ) );
  Save( k, v );
}

void CKeyValuePair::Save(const std::string &key, const std::string &value) {
  structKey k( String, key.size(), key.c_str() );
  structValue v( (void*) value.c_str(), (u_int32_t) value.size() );
  Save( k, v );
}

void CKeyValuePair::Save(const std::string &key, short value) {
  structKey k( Int16, key.size(), key.c_str() );
  structValue v( &value, sizeof( short ) );
  Save( k, v );
}

void CKeyValuePair::Save(const std::string &key, unsigned short value) {
  structKey k( UInt16, key.size(), key.c_str() );
  structValue v( &value, sizeof( unsigned short ) );
  Save( k, v );
}

void CKeyValuePair::Save(const std::string &key, long value) {
  structKey k( Int32, key.size(), key.c_str() );
  structValue v( &value, sizeof( long ) );
  Save( k, v );
}

void CKeyValuePair::Save(const std::string &key, unsigned long value) {
  structKey k( UInt32, key.size(), key.c_str() );
  structValue v( &value, sizeof( unsigned long ) );
  Save( k, v );
}

void CKeyValuePair::Save(const std::string &key, float value) {
  structKey k( Float, key.size(), key.c_str() );
  structValue v( &value, sizeof( float ) );
  Save( k, v );
}

void CKeyValuePair::Save(const std::string &key, double value) {
  structKey k( Double, key.size(), key.c_str() );
  structValue v( &value, sizeof( double ) );
  Save( k, v );
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
  size_t size;
  void *q;
  Get( k, &q, &size );
  assert( size == sizeof( short ) );
  short *p = (short*) q;
  return *p;
}

unsigned short CKeyValuePair::GetUnsignedShort(const std::string &key) {
  structKey k( UInt16, key.size(), key.c_str() );
  size_t size;
  void *q;
  Get( k, &q, &size );
  assert( size == sizeof( unsigned short ) );
  unsigned short *p = (unsigned short *) q;
  return *p;
}

long CKeyValuePair::GetLong(const std::string &key) {
  structKey k( Int32, key.size(), key.c_str() );
  size_t size;
  void *q;
  Get( k, &q, &size );
  assert( size = sizeof( long ) );
  long *p = (long*) q;
  return *p;
}

unsigned long CKeyValuePair::GetUnsignedLong(const std::string &key) {
  structKey k( UInt32, key.size(), key.c_str() );
  size_t size;
  void *q;
  Get( k, &q, &size );
  assert( size == sizeof( unsigned long ) );
  unsigned long *p = (unsigned long *) q;
  return *p;
}

float CKeyValuePair::GetFloat(const std::string &key) {
  structKey k( Float, key.size(), key.c_str() );
  size_t size;
  void *q;
  Get( k, &q, &size );
  assert( size == sizeof( float ) );
  float *p = (float*) q;
  return *p;
}

double CKeyValuePair::GetDouble(const std::string &key) {
  structKey k( Double, key.size(), key.c_str() );
  size_t size;
  void *q;
  Get( k, &q, &size );
  assert( size == sizeof( double ) );
  double *p = (double*) q;
  return *p;
}

CKeyValuePair::structValue CKeyValuePair::GetBlob(const std::string &key) {
  structKey k( Blob, key.size(), key.c_str() );
  u_int32_t size;
  void *p;
  Get( k, &p, &size );
  structValue v( p, size );
  return v;
}

