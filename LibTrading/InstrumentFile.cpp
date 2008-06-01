#include "StdAfx.h"
#include "InstrumentFile.h"
#include "BerkeleyDBDataManager.h"
#include "TradingEnumerations.h"

#include <stdexcept>

CInstrumentFile::CInstrumentFile(void) : 
    pRecord( NULL ), 
    m_pdbSymbols( NULL ), 
    m_pdbIxSymbols_Market( NULL ), m_pdbcIxSymbols_Market( NULL ),
    m_pdbIxSymbols_Underlying( NULL ), m_pdbcIxSymbols_Underlying( NULL ) 
    {
}

CInstrumentFile::~CInstrumentFile(void) {
  //if ( NULL != m_pdbcIxIQFSymbols_Market ) delete m_pdbcIxIQFSymbols_Market;
  //if ( NULL != m_pdbIxIQFSymbols_Market ) delete m_pdbIxIQFSymbols_Market;
  //if ( NULL != m_pdbIQFSymbols ) delete m_pdbIQFSymbols;
}

void CInstrumentFile::OpenIQFSymbols() {
  CBerkeleyDBDataManager dm;
  DbEnv *pDbEnv = dm.GetDbEnv();

  // open/create main symbol table
  m_pdbSymbols = new Db( pDbEnv, 0 );
  m_pdbSymbols->open( NULL, dm.GetBDBFileName(), "IQFSymbols", DB_BTREE, DB_CREATE, 0 );

  // open/create the market index
  m_pdbIxSymbols_Market = new Db( pDbEnv, 0 );
  m_pdbIxSymbols_Market->set_flags( DB_DUPSORT );
  m_pdbIxSymbols_Market->open( NULL, dm.GetBDBFileName(), "IxIQFSymbols_Market", DB_BTREE, DB_CREATE, 0 );

  // associate the index with the main table
  m_pdbSymbols->associate( NULL, m_pdbIxSymbols_Market, &CInstrumentFile::GetMarketName, 0 );

  // open/create the underlying index
  m_pdbIxSymbols_Underlying = new Db( pDbEnv, 0 );
  m_pdbIxSymbols_Underlying->set_flags( DB_DUPSORT );
  m_pdbIxSymbols_Underlying->open( NULL, dm.GetBDBFileName(), "IxIQFSymbols_Underlying", DB_BTREE, DB_CREATE, 0 );

  // associate the index with the main table
  m_pdbSymbols->associate( NULL, m_pdbIxSymbols_Underlying, &CInstrumentFile::GetUnderlyingName, 0 );
}

CInstrument CInstrumentFile::CreateInstrumentFromIQFeed(const std::string &sSymbolName) {
  SetSearchUnderlying( sSymbolName.c_str() );
  bool bSymbolFound = RetrieveSymbolRecord( DB_SET );
  if ( !bSymbolFound ) {
    throw std::out_of_range( "No symbol found" );
  }
  switch ( GetInstrumentType() ) {
    case InstrumentType::Stock: 
      return CInstrument( sSymbolName, InstrumentType::Stock );
      break;
    case InstrumentType::Option: {
      const char *p = GetDescription(); 
      const char *e = strchr( p, ' ' );  
      u_int32_t len = e - p;
      string sUnderlying( GetDescription(), len );
      return CInstrument( sSymbolName, 
        (InstrumentType::enumInstrumentTypes) GetInstrumentType(), 
        GetYear(), GetMonth(),
        sUnderlying, (OptionSide::enumOptionSide) GetOptionSide(), 
        (double) GetStrike() );
      }
      break;
    case InstrumentType::Future: 
      return CInstrument( sSymbolName, (InstrumentType::enumInstrumentTypes) GetInstrumentType(), GetYear(), GetMonth() );
      break;
    default:
      throw std::out_of_range( "Unknown instrument type" ); 
  }
}

void CInstrumentFile::CloseIQFSymbols() {
  m_pdbIxSymbols_Underlying->close(0);
  m_pdbIxSymbols_Market->close(0);
  m_pdbSymbols->close(0);
}

// Sets SubIndex (static function)
int CInstrumentFile::GetMarketName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey ) {
  structSymbolRecord *dbIxRecord = (structSymbolRecord *) data->get_data();
  char *p = dbIxRecord->line + dbIxRecord->ix[2];  // get at the 'exchange' string
  unsigned long l = dbIxRecord->cnt[2];  // set the key and its length
  secKey->set_data( p );
  secKey->set_size( l );
  return 0;
}

// Sets SubIndex (static function)
int CInstrumentFile::GetUnderlyingName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey ) {
  structSymbolRecord *dbIxRecord = (structSymbolRecord *) data->get_data();
  char *p; 
  u_int32_t len;
  bool bUseIndex = true;
  if ( InstrumentType::Option == dbIxRecord->eInstrumentType ) { // OPRA option
    p = dbIxRecord->line + dbIxRecord->ix[1];  // start of description
    char *e = strchr( p, ' ' );  // find the trailing blank
    len = e - p;
    if ( 0 != len ) bUseIndex = false;
  }
  if ( bUseIndex ) {  // by default, use records symbol
    p = dbIxRecord->line;
    len = dbIxRecord->cnt[0];
  }
  secKey->set_data( p );
  secKey->set_size( len );
  return 0;
}

void CInstrumentFile::SetSearchExchange( const char *szExchange ) {
  m_szSearchKey = szExchange;
  m_lenSearchKey = strlen( szExchange );
  m_dbtKey.set_data( (void*) szExchange );
  m_dbtKey.set_size( m_lenSearchKey );
  m_pdbIxSymbols_Market->cursor( NULL, &m_pdbcIxSymbols_Market, 0 );
}

void CInstrumentFile::SetSearchUnderlying( const char *szUnderlying ) {
  m_szSearchKey = szUnderlying;
  m_lenSearchKey = strlen( szUnderlying );
  m_dbtKey.set_data( (void*) szUnderlying );
  m_dbtKey.set_size( m_lenSearchKey );
  m_pdbIxSymbols_Underlying->cursor( NULL, &m_pdbcIxSymbols_Underlying, 0 );
}

bool CInstrumentFile::RetrieveSymbolRecord( u_int32_t flags ) {
  pRecord = NULL;
  int result = m_pdbcIxSymbols_Market->get( &m_dbtKey, &m_dbtData, flags );
  if ( 0 == result ) {
    pRecord = (structSymbolRecord *) m_dbtData.get_data();
    UnPackBoolean( pRecord->ucBits1 );
  }
  return ( 0 == result );
}

void CInstrumentFile::EndSearch( void ) {
}

void CInstrumentFile::PackBoolean( void ) {
  dbRecord.ucBits1 = 0;
  dbRecord.ucBits1 |= ( m_bMutual ? ucMutual : 0 );
  dbRecord.ucBits1 |= ( m_bMoneyMkt ? ucMoneyMkt : 0 );
  dbRecord.ucBits1 |= ( m_bIndex ? ucIndex : 0 );
  dbRecord.ucBits1 |= ( m_bCboe ? ucCboe : 0 );
  dbRecord.ucBits1 |= ( m_bIndicator ? ucIndicator : 0 );
  dbRecord.ucBits1 |= ( m_bHasOptions ? ucHasOptions : 0 );

}

void CInstrumentFile::UnPackBoolean( const unsigned char ucBits1 ) {
  m_bMutual = 0 != ( ucBits1 & ucMutual );
  m_bMoneyMkt = 0 != ( ucBits1 & ucMoneyMkt );
  m_bIndex = 0 != ( ucBits1 & ucIndex );
  m_bCboe = 0 != ( ucBits1 & ucCboe );
  m_bIndicator = 0 != ( ucBits1 & ucIndicator );
  m_bHasOptions = 0 != ( ucBits1 & ucHasOptions );
}

