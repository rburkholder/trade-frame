#pragma once

#include "BerkeleyDb.h"
#include <string>

//using namespace std;

class CIQFeedSymbolFile {
public:
  CIQFeedSymbolFile(void);
  virtual ~CIQFeedSymbolFile(void);
  void Open( void );
  bool Load( const string &filename );
  void Close( void );
  void SetSearchExchange( const char *szExchange );  // must remain set for duration of search
  const char *GetSymbol( u_int32_t flags );
  void EndSearch( void );
  //const char *Get
protected:
  static int GetMarketName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey );
    // memset this structure sometime.
  static const size_t nMaxBufferSize = 255;
  typedef unsigned char td_structIndexes;
  struct structIQFSymbolRecord {
    td_structIndexes bufferedlength; // means structure can only be 255 long
    td_structIndexes ix[3]; // looking for three strings: symbol, desc, exchange
    td_structIndexes cnt[3];  // length of each of three strings, excludes terminator
    unsigned char ucBits;  // mutual, moneymkt, index, cboe, indicator
    char line[nMaxBufferSize];
  } dbRecord;

  bool m_bMutual, m_bMoneyMkt, m_bIndex, m_bCboe, m_bIndicator, m_bHasOptions;
  Db *m_pdbIQFSymbols;
  Db *m_pdbIxIQFSymbols_Market;
  Dbc *m_pdbcIxIQFSymbols_Market;
  Dbt m_dbtKey;
  Dbt m_dbtData;
  const char *m_szSearchKey;
  u_int32_t m_lenSearchKey;

  static const unsigned char ucMutual = 1;
  static const unsigned char ucMoneyMkt = 2;
  static const unsigned char ucIndex = 4;
  static const unsigned char ucCboe = 8;
  static const unsigned char ucIndicator = 16;
  static const unsigned char ucHasOptions = 32;
  void PackBoolean(void);
  void UnPackBoolean(void);
private:
};
