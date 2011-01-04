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

// should a provider be included?  No, because this allows the same instrument
//  to be used to supply details to any provider.

#pragma once

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

#include <LibCommon/Delegate.h>
#include <LibSqlite/sqlite3.h>

#include "TradingEnumerations.h"

class CInstrument {
public:

  typedef unsigned short enumProviderId_t;  // from CProviderInterfaceBase in ProviderInterface.h
  typedef std::string idInstrument_t;
  typedef const idInstrument_t& idInstrument_cref;
  typedef boost::shared_ptr<CInstrument> pInstrument_t;
  typedef const pInstrument_t& pInstrument_cref;

  CInstrument( 
    idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
    InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  CInstrument( 
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month );
  CInstrument( 
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymm
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month,
    //const idInstrument_t &sUnderlyingName,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  CInstrument( 
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month, unsigned short day,
    //const std::string &sUnderlyingName,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  CInstrument( 
    idInstrument_cref sInstrumentName, 
    //const std::string& sUnderlyingName, // currency
    pInstrument_t pUnderlying,
    InstrumentType::enumInstrumentTypes type, 
    Currency::enumCurrency base, Currency::enumCurrency counter );
  CInstrument( idInstrument_cref sInstrumentId, sqlite3_stmt* pStmt ); // with no underlying
  CInstrument( idInstrument_cref sInstrumentId, sqlite3_stmt* pStmt, pInstrument_t pUnderlying ); // with an underlying
    
  virtual ~CInstrument(void);

  idInstrument_cref GetInstrumentName( void ) const { return m_sInstrumentName; };
  idInstrument_cref GetUnderlyingName( void );

  idInstrument_cref GetInstrumentName( enumProviderId_t id );
  idInstrument_cref GetUnderlyingName( enumProviderId_t id );

  void SetUnderlying( pInstrument_t pUnderlying );

  void SetAlternateName( enumProviderId_t, idInstrument_cref );

  typedef std::pair<idInstrument_cref,idInstrument_cref> pairNames_t;
  ou::Delegate<pairNames_t> OnAlternateNameAdded;  // key, alt
  ou::Delegate<pairNames_t> OnAlternateNameChanged;  // old, new

  InstrumentType::enumInstrumentTypes GetInstrumentType( void ) { return m_InstrumentType; };
  bool IsOption( void ) const { return ( InstrumentType::Option == m_InstrumentType ); };
  bool IsFuture( void ) const { return ( InstrumentType::Future == m_InstrumentType ); };

  const std::string& GetExchangeName( void ) const { return m_sExchange; };
  void SetCurrency( Currency::enumCurrency eCurrency ) { m_Currency = eCurrency; };
  const char *GetCurrencyName( void ) { return Currency::Name[ m_Currency ]; };

  double GetStrike( void ) const { return m_dblStrike; };
  unsigned short GetExpiryYear( void ) const { return m_nYear; };
  unsigned short GetExpiryMonth( void ) const { return m_nMonth; };
  unsigned short GetExpiryDay( void ) const { return m_nDay; };
  OptionSide::enumOptionSide GetOptionSide( void ) { return m_OptionSide; };

  void SetContract( long id ) { m_nContract = id; };
  long GetContract( void ) const { return m_nContract; };

  void SetMultiplier( unsigned long nMultiplier ) { m_nMultiplier = nMultiplier; };
  unsigned long GetMultiplier( void ) const { return m_nMultiplier; };

  static void CreateDbTable( sqlite3* pDb );
  int BindDbKey( sqlite3_stmt* pStmt );
  int BindDbVariables( sqlite3_stmt* pStmt );
  static const std::string& GetSqlSelect( void ) { return m_sSqlSelect; };
  static const std::string& GetSqlInsert( void ) { return m_sSqlInsert; };
  static const std::string& GetSqlUpdate( void ) { return m_sSqlUpdate; };
  static const std::string& GetSqlDelete( void ) { return m_sSqlDelete; };

protected:

  idInstrument_t m_sInstrumentName; // main name
  std::string m_sDescription;
  pInstrument_t m_pUnderlying;
  idInstrument_t m_sUnderlying;  // used only for when loading from db and need to compare assigned underlying
  InstrumentType::enumInstrumentTypes m_InstrumentType;
  Currency::enumCurrency m_Currency;  // base currency - http://en.wikipedia.org/wiki/Currency_pair
  Currency::enumCurrency m_CurrencyCounter; // quote/counter currency -  - depicts how many units of the counter currency are needed to buy one unit of the base currency
  std::string m_sExchange;
  OptionSide::enumOptionSide m_OptionSide;
  unsigned short m_nYear; // future, option
  unsigned short m_nMonth; // future, option
  unsigned short m_nDay; // future, option
  double m_dblStrike;
  long m_nContract;  // used with CIBTWS
  unsigned long m_nMultiplier;  // number of units per contract: stk 1x, option 100x

private:

  typedef std::map<enumProviderId_t, idInstrument_t> mapAlternateNames_t;
  typedef std::pair<enumProviderId_t, idInstrument_t> mapAlternateNames_pair_t;
  mapAlternateNames_t m_mapAlternateNames;

  enum enunUnderlyingStatus {
    EUnderlyingNotSettable, EUnderlyingNotSet, EUnderlyingSet
  } m_eUnderlyingStatus;

  static const std::string m_sSqlCreate;
  static const std::string m_sSqlSelect;
  static const std::string m_sSqlInsert;
  static const std::string m_sSqlUpdate;
  static const std::string m_sSqlDelete;

  CInstrument( const CInstrument& );  // copy ctor
  CInstrument& operator=( const CInstrument& ); // assignement
};
