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

#pragma once

#include <bitset>

#include <LibBerkeleyDb/EnvManager.h>

#include "Instrument.h"

// 2010/05/02
// try designing an iterator for the Retrieve* acces stuff

// 2010/05/07 close cursors when finished

struct structSymbolRecord {  // member variables ordered by decreasing size for alignment purposes

  typedef unsigned char structIndexes_t;
  typedef std::bitset<32> bitsSymbolClassifier_t;

  static const size_t nMaxBufferSize = 255;
  static const unsigned char nMaxStrings = 4;  // symbol, desc, exchange, listed market

  enum enumSymbolClassifier: unsigned char { // bits in bitmap of stored data record
    Unknown = 0, Bonds, Calc, Equity, FOption, Forex, Forward, Future, ICSpread, 
      IEOption, Index, MktStats, Money, Mutual, PrecMtl, Spot, Spread, StratSpread, 
      FrontMonth, HasOptions  // these last two are calculated differently than previous enumerations
  };

  float fltStrike;  // option strike price
  unsigned long SIC;
  unsigned long NAICS;
  bitsSymbolClassifier_t sc; // symbol classifications
  unsigned short nYear;  // futures or options
  structIndexes_t ix[nMaxStrings]; // starting position of each expected string
  structIndexes_t cnt[nMaxStrings];  // length of each strings, excludes terminator
  structIndexes_t bufferedlength; // length of whole structure, can only be <255
  unsigned char eInstrumentType;  // Trading::enumContractTypes
  unsigned char nMonth;  // 1 - 12, 0 for nothing
  unsigned char nOptionSide;  // OptionSide
  char line[nMaxBufferSize];

  const char *GetSymbol() { return line; };
  const char *GetDescription() { return line + ix[1]; };
  const char *GetExchange() { return line + ix[2]; };
  unsigned char GetInstrumentType() { return eInstrumentType; };
  unsigned char GetOptionSide() { return nOptionSide; };
  unsigned short GetYear() { return nYear; };
  unsigned short GetMonth() { return nMonth; };
  const bitsSymbolClassifier_t& GetSymbolClassifier() { return sc; };
  void SetSymbolClassifier( const bitsSymbolClassifier_t& sc_ ) { sc.reset(); sc |= sc_; };
  float GetStrike() { return fltStrike; };
};

class CInstrumentFile_Exchange_iterator;

class CInstrumentFile {
  bool m_bOpen;
public:
  CInstrumentFile(void);
  ~CInstrumentFile(void);

  typedef structSymbolRecord::bitsSymbolClassifier_t bitsSymbolClassifier_t;
  typedef structSymbolRecord::enumSymbolClassifier enumSymbolClassifier;
  typedef structSymbolRecord::structIndexes_t structIndexes_t;

  typedef CInstrumentFile_Exchange_iterator iterator;
  
  void OpenIQFSymbols( void );
  bool IsOpen( void ) { return m_bOpen; };
  void CloseIQFSymbols( void );

  void SetSearchExchange( const char *szExchange );  // must remain set for duration of search
  void SetSearchUnderlying( const char *szUnderlying );
  structSymbolRecord* RetrieveSymbolRecordByExchange( u_int32_t flags );
  structSymbolRecord* RetrieveSymbolRecordByUnderlying( u_int32_t flags );
  void EndSearch( void ) {};
  CInstrument::pInstrument_t 
    CreateInstrumentFromIQFeed( const std::string &sIQFeedSymbolName, const std::string &sAlternateSymbolName );

protected:

  structSymbolRecord  dbRecord, *pRecord;

  Db *m_pdbSymbols;
  Db *m_pdbIxSymbols_Market;
  Dbc *m_pdbcIxSymbols_Market;
  Db *m_pdbIxSymbols_Underlying;
  Dbc *m_pdbcIxSymbols_Underlying;
  Dbt m_dbtKey;
  Dbt m_dbtData;
  const char *m_szSearchKey;
  u_int32_t m_lenSearchKey;

private:

  static int GetMarketName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey );
  static int GetUnderlyingName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey );
    // memset this structure sometime.
};

// http://www.cplusplus.com/reference/std/iterator/iterator/

class CInstrumentFile_Exchange_iterator: public std::iterator<std::forward_iterator_tag, structSymbolRecord> {
  structSymbolRecord *m_pSR;
  CInstrumentFile* m_pIF;
public:
  CInstrumentFile_Exchange_iterator( void ): m_pSR( NULL ), m_pIF( NULL ) {
  };
  CInstrumentFile_Exchange_iterator( CInstrumentFile* pIF ): m_pSR( NULL ), m_pIF( NULL ) {
    SetInstrumentFile( pIF );
  };
  ~CInstrumentFile_Exchange_iterator( void ) {};

  void SetInstrumentFile( CInstrumentFile* pIF ) {
    assert( NULL != pIF );
    assert( pIF->IsOpen() );
    m_pIF = pIF;
  };

  structSymbolRecord* begin( const std::string& sExchange ) {
    assert( NULL != m_pIF );
    m_pSR = NULL;
    m_pIF->SetSearchExchange( sExchange.c_str() );
//    m_pIF->SetSearchUnderlying( sExchange.c_str() );
    m_pSR = m_pIF->RetrieveSymbolRecordByExchange( DB_SET );
//    m_pSR = m_pIF->RetrieveSymbolRecordByUnderlying( DB_SET );
    return m_pSR;
  };

  structSymbolRecord* end( void ) {
    return NULL;
  };

  structSymbolRecord* operator++() {
    if ( NULL != m_pSR ) {
      m_pSR = m_pIF->RetrieveSymbolRecordByExchange( DB_NEXT_DUP );
//      m_pSR = m_pIF->RetrieveSymbolRecordByUnderlying( DB_NEXT_DUP );
    }
    return m_pSR;
  };

  bool operator==(const CInstrumentFile_Exchange_iterator& rhs) { return m_pSR == rhs.m_pSR; };
  bool operator!=(const CInstrumentFile_Exchange_iterator& rhs) { return m_pSR != rhs.m_pSR; };

  structSymbolRecord& operator*() { return *m_pSR; };
  structSymbolRecord& operator->() { return *m_pSR; };
protected:
private:
  CInstrumentFile_Exchange_iterator( const CInstrumentFile_Exchange_iterator& iter ) {};  // disable copy constructor
  CInstrumentFile_Exchange_iterator& operator=( const CInstrumentFile_Exchange_iterator& rhs ) {}; // disable assignment 
};

