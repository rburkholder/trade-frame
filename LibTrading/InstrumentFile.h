#pragma once

#include "BerkeleyDb.h"

#include "Instrument.h"

// http://www.tempest-sw.com/cpp/draft/ch13-44-stdexcept.html

class CInstrumentFile {
public:
  CInstrumentFile(void);
  virtual ~CInstrumentFile(void);
  bool GetBitMutual() { return m_bMutual; };
  bool GetBitMoneyMkt()  { return m_bMoneyMkt; };
  bool GetBitIndex()  { return m_bIndex; };
  bool GetBitCboe() { return m_bCboe; };
  bool GetBitIndicator() { return m_bIndicator; };
  bool GetBitHasOptions() { return m_bHasOptions; };
  void OpenIQFSymbols( void );
  void CloseIQFSymbols( void );
  void SetSearchExchange( const char *szExchange );  // must remain set for duration of search
  void SetSearchUnderlying( const char *szUnderlying );
  bool RetrieveSymbolRecordByExchange( u_int32_t flags );
  bool RetrieveSymbolRecordByUnderlying( u_int32_t flags );
  const char *GetSymbol() { return pRecord->line; };
  const char *GetDescription() { return pRecord->line + pRecord->ix[1]; };
  const char *GetExchange() { return pRecord->line + pRecord->ix[2]; };
  unsigned char GetInstrumentType() { return pRecord->eInstrumentType; };
  unsigned char GetOptionSide() { return pRecord->nOptionSide; };
  unsigned short GetYear() { return pRecord->nYear; };
  unsigned short GetMonth() { return pRecord->nMonth; };
  float GetStrike() { return pRecord->fltStrike; };
  void EndSearch( void );
  CInstrument *CreateInstrumentFromIQFeed( const std::string &sIQFeedSymbolName, const std::string &sAlternateSymbolName ) throw( std::out_of_range );

protected:
  Db *m_pdbSymbols;
  Db *m_pdbIxSymbols_Market;
  Dbc *m_pdbcIxSymbols_Market;
  Db *m_pdbIxSymbols_Underlying;
  Dbc *m_pdbcIxSymbols_Underlying;
  Dbt m_dbtKey;
  Dbt m_dbtData;
  const char *m_szSearchKey;
  u_int32_t m_lenSearchKey;

  static int GetMarketName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey );
  static int GetUnderlyingName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey );
    // memset this structure sometime.

  static const size_t nMaxBufferSize = 255;
  typedef unsigned char td_structIndexes;
  struct structSymbolRecord {  //members ordered by decreasing size for alignment purposes
    float fltStrike;  // option strike price
    unsigned short nYear;  // futures or options
    td_structIndexes ix[3]; // looking for three strings: symbol, desc, exchange
    td_structIndexes cnt[3];  // length of each of three strings, excludes terminator
    td_structIndexes bufferedlength; // length of whole structure, can only be <255
    unsigned char ucBits1;  // mutual, moneymkt, index, cboe, indicator, hasoptions
    unsigned char eInstrumentType;  // Trading::enumContractTypes
    unsigned char nMonth;  // 1 - 12, 0 for nothing
    unsigned char nOptionSide;  // OptionSide
    char line[nMaxBufferSize];
  } dbRecord;
  structSymbolRecord *pRecord; // used for retrievals

  static const unsigned char ucMutual = 1;
  static const unsigned char ucMoneyMkt = 2;
  static const unsigned char ucIndex = 4;
  static const unsigned char ucCboe = 8;
  static const unsigned char ucIndicator = 16;
  static const unsigned char ucHasOptions = 32;
  bool m_bMutual, m_bMoneyMkt, m_bIndex, m_bCboe, m_bIndicator, m_bHasOptions;

  void PackBoolean(void);
  void UnPackBoolean( const unsigned char ucBits1 );
private:
};
