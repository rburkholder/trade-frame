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

  enum enumSymbolClassifier: unsigned char { // bits in bitmap of stored data record
    Unknown = 0, Bonds, Calc, Equity, FOption, Forex, Forward, Future, ICSpread, 
      IEOption, Index, MktStats, Money, Mutual, PrecMtl, Spot, Spread, StratSpread, 
      FrontMonth, HasOptions  // these last two are calculated differently than previous enumerations
  };
  typedef std::bitset<32> bitsSymbolClassifier_t;

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
  static const unsigned char nMaxStrings = 4;  // symbol, desc, exchange, listed market
  typedef unsigned char structIndexes_t;
  struct structSymbolRecord {  //members ordered by decreasing size for alignment purposes
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
  } dbRecord, *pRecord;

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


