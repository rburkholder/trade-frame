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

class CInstrumentFile {
public:
  CInstrumentFile(void);
  ~CInstrumentFile(void);

  enum enumSymbolClassifier: unsigned char {
    Mutual = 0, MoneyMarket, Index, CBOE, Indicator, HasOptions, NotAStock
  };
  typedef std::bitset<8> bitsSymbolClassifier_t;

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
  const bitsSymbolClassifier_t& GetSymbolClassifier() { return pRecord->sc; };
  void SetSymbolClassifier( const bitsSymbolClassifier_t& sc ) { pRecord->sc.reset(); pRecord->sc |= sc; };
  float GetStrike() { return pRecord->fltStrike; };
  void EndSearch( void );
  CInstrument::pInstrument_t 
    CreateInstrumentFromIQFeed( const std::string &sIQFeedSymbolName, const std::string &sAlternateSymbolName );

protected:

  static const size_t nMaxBufferSize = 255;
  typedef unsigned char structIndexes_t;
  struct structSymbolRecord {  //members ordered by decreasing size for alignment purposes
    float fltStrike;  // option strike price
    unsigned short nYear;  // futures or options
    structIndexes_t ix[3]; // looking for three strings: symbol, desc, exchange
    structIndexes_t cnt[3];  // length of each of three strings, excludes terminator
    structIndexes_t bufferedlength; // length of whole structure, can only be <255
    //unsigned char ucBits1;  // mutual, moneymkt, index, cboe, indicator, hasoptions
    bitsSymbolClassifier_t sc; // symbol classifications
    unsigned char eInstrumentType;  // Trading::enumContractTypes
    unsigned char nMonth;  // 1 - 12, 0 for nothing
    unsigned char nOptionSide;  // OptionSide
    char line[nMaxBufferSize];
  } dbRecord;
  structSymbolRecord *pRecord; // used for retrievals

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


