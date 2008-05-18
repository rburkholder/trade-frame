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
  bool RetrieveSymbolRecord( u_int32_t flags );
  const char *GetSymbol() { return pRecord->line; };
  const char *GetDescription() { return pRecord->line + pRecord->ix[1]; };
  const char *GetExchange() { return pRecord->line + pRecord->ix[2]; };
  bool GetBitMutual() { return m_bMutual; };
  bool GetBitMoneyMkt()  { return m_bMoneyMkt; };
  bool GetBitIndex()  { return m_bIndex; };
  bool GetBitCboe() { return m_bCboe; };
  bool GetBitIndicator() { return m_bIndicator; };
  bool GetBitHasOptions() { return m_bHasOptions; };
  void EndSearch( void );
protected:
  static int GetMarketName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey );
    // memset this structure sometime.
  static const size_t nMaxBufferSize = 255;
  typedef unsigned char td_structIndexes;
  struct structIQFSymbolRecord {
    float fltStrike;  // option strike price
    unsigned short nYear;  // futures or options
    td_structIndexes ix[3]; // looking for three strings: symbol, desc, exchange
    td_structIndexes cnt[3];  // length of each of three strings, excludes terminator
    td_structIndexes bufferedlength; // means structure can only be 255 long
    unsigned char ucBits1;  // mutual, moneymkt, index, cboe, indicator, hasoptions
    unsigned char ucBits2;  // stock, future, option, futuresoption, currency, bond, etf
    unsigned char nMonth;  // 1 - 12, 0 for nothing
    char chDirection;  // P put C call for option
    char line[nMaxBufferSize];
  } dbRecord;
  structIQFSymbolRecord *pRecord; // used for retrievals

  bool m_bMutual, m_bMoneyMkt, m_bIndex, m_bCboe, m_bIndicator, m_bHasOptions;
  bool m_bStock, m_bFuture, m_bOption, m_bFuturesOption, m_bCurrency, m_bBond, m_bETF, m_bMetals;
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
  static const unsigned char ucStock = 1;
  static const unsigned char ucFuture = 2;
  static const unsigned char ucOption = 4;
  static const unsigned char ucFuturesOption = 8;
  static const unsigned char ucCurrency = 16;
  static const unsigned char ucBond = 32;
  static const unsigned char ucETF = 64;
  static const unsigned char ucMetals = 128;
  void PackBoolean(void);
  void UnPackBoolean( const unsigned char ucBits1, const unsigned char ucBits2 );
private:
};
