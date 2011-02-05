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

#include "TradingEnumerations.h"
#include "KeyTypes.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CInstrument {
public:

  typedef keytypes::eidProvider_t eidProvider_t;  // from CProviderInterfaceBase in ProviderInterface.h
  typedef keytypes::idExchange_t idExchange_t;
  typedef keytypes::idInstrument_t idInstrument_t;
  typedef const idInstrument_t& idInstrument_cref;
  typedef boost::shared_ptr<CInstrument> pInstrument_t;
  typedef const pInstrument_t& pInstrument_cref;

  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "instrumentid", idInstrument );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "exchangeid", idExchange );
      ou::db::Field( a, "type", eType );
      ou::db::Field( a, "currency", eCurrency );
      ou::db::Field( a, "countercurrency", eCounterCurrency );
      ou::db::Field( a, "optionside", eOptionSide );
      ou::db::Field( a, "underlyingid", idUnderlying );
      ou::db::Field( a, "year", nYear );
      ou::db::Field( a, "month", nMonth );
      ou::db::Field( a, "day", nDay );
      ou::db::Field( a, "strike", dblStrike );
      ou::db::Field( a, "contract", nContract );
      ou::db::Field( a, "multiplier", nMultiplier );

      ou::db::Key( a, "instrumentid" );
      ou::db::Constraint( a, "exchangeid", "exchanges", "exchangeid" );
      ou::db::Constraint( a, "underlyingid", "instruments", "instrumentid" );
    }

    idInstrument_t idInstrument; // main name
    std::string sDescription;
    idExchange_t idExchange;
    InstrumentType::enumInstrumentTypes eType;
    Currency::enumCurrency eCurrency;  // base currency - http://en.wikipedia.org/wiki/Currency_pair
    Currency::enumCurrency eCounterCurrency; // quote/counter currency -  - depicts how many units of the counter currency are needed to buy one unit of the base currency
    OptionSide::enumOptionSide eOptionSide;
    idInstrument_t idUnderlying;  // used only for when loading from db and need to compare assigned underlying
    unsigned short nYear; // future, option
    unsigned short nMonth; // future, option
    unsigned short nDay; // future, option
    double dblStrike;
    long nContract; // used with CIBTWS
    unsigned long nMultiplier;  // number of units per contract: stk 1x, option 100x
  };

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

  const static std::string m_sTableName;

  idInstrument_cref GetInstrumentName( void ) const { return m_sInstrumentName; };
  idInstrument_cref GetUnderlyingName( void );

  idInstrument_cref GetInstrumentName( eidProvider_t id );
  idInstrument_cref GetUnderlyingName( eidProvider_t id );

  void SetUnderlying( pInstrument_t pUnderlying );

  void SetAlternateName( eidProvider_t, idInstrument_cref );

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

protected:

  pInstrument_t m_pUnderlying;

private:

  typedef std::map<eidProvider_t, idInstrument_t> mapAlternateNames_t;
  typedef std::pair<eidProvider_t, idInstrument_t> mapAlternateNames_pair_t;
  mapAlternateNames_t m_mapAlternateNames;

  enum enunUnderlyingStatus {
    EUnderlyingNotSettable, EUnderlyingNotSet, EUnderlyingSet
  } m_eUnderlyingStatus;

  TableRowDef m_row;

  CInstrument( const CInstrument& );  // copy ctor
  CInstrument& operator=( const CInstrument& ); // assignement
};

} // namespace tf
} // namespace ou
