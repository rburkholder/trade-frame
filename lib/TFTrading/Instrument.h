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
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <OUCommon/Delegate.h>

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
      ou::db::Field( a, "type", eType );
      ou::db::Field( a, "description", sDescription );
      ou::db::Field( a, "exchangeid", idExchange );
      ou::db::Field( a, "underlyingid", idUnderlying );
      ou::db::Field( a, "currency", eCurrency );
      ou::db::Field( a, "countercurrency", eCounterCurrency );
      ou::db::Field( a, "optionside", eOptionSide );
      ou::db::Field( a, "year", nYear );
      ou::db::Field( a, "month", nMonth );
      ou::db::Field( a, "day", nDay );
      ou::db::Field( a, "strike", dblStrike );
      ou::db::Field( a, "ibcontract", nIBContract );
      ou::db::Field( a, "multiplier", nMultiplier );
      ou::db::Field( a, "mintick", dblMinTick );
      ou::db::Field( a, "sigdigits", nSignificantDigits );
    }

    idInstrument_t idInstrument; // main name
    InstrumentType::enumInstrumentTypes eType;
    std::string sDescription;
    idExchange_t idExchange;
    idInstrument_t idUnderlying;  // used only for when loading from db and need to compare assigned underlying
    Currency::enumCurrency eCurrency;  // base currency - http://en.wikipedia.org/wiki/Currency_pair
    Currency::enumCurrency eCounterCurrency; // quote/counter currency -  - depicts how many units of the counter currency are needed to buy one unit of the base currency
    OptionSide::enumOptionSide eOptionSide;
    boost::uint16_t nYear; // future, option
    boost::uint16_t nMonth; // future, option
    boost::uint16_t nDay; // future, option
    double dblStrike;
    boost::int32_t nIBContract; // used with CIBTWS
    boost::uint32_t nMultiplier;  // number of units per contract: stk 1x, option 100x
    double dblMinTick; 
    boost::uint8_t nSignificantDigits;

//  m_eUnderlyingStatus = EUnderlyingNotSettable;
//  if ( InstrumentType::Option == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingNotSet;
//  if ( InstrumentType::Currency == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingNotSet;
//  if ( InstrumentType::FuturesOption == m_InstrumentType ) m_eUnderlyingStatus = EUnderlyingNotSet;
    // see more in cpp file

    TableRowDef( void ) // default constructor
      : eType( InstrumentType::Unknown ), eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( OptionSide::Unknown ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), dblStrike( 0.0 ), 
      nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {};
    TableRowDef( // equity / generic creation
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentTypes eType_, idExchange_t idExchange_ )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ), 
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( OptionSide::Unknown ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), dblStrike( 0.0 ), 
      nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( eType < InstrumentType::_Count );
        assert( eType > InstrumentType::Unknown );
        assert( 0 < idInstrument.size() );  
    };
    TableRowDef( // future
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentTypes eType_, idExchange_t idExchange_,
      boost::uint16_t nYear_, boost::uint16_t nMonth_ )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ), 
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( OptionSide::Unknown ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( 0 ), dblStrike( 0.0 ), 
      nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( eType == InstrumentType::Future  );
        assert( 0 < idInstrument.size() );   
    };
    TableRowDef( // option with yymm
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentTypes eType_, idExchange_t idExchange_,
      idInstrument_t idUnderlying_, 
      boost::uint16_t nYear_, boost::uint16_t nMonth_, 
      OptionSide::enumOptionSide eOptionSide_, double dblStrike_  )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ), idUnderlying( idUnderlying_ ),
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( eOptionSide_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( 0 ), dblStrike( dblStrike_ ), 
      nIBContract( 0 ), nMultiplier( 100 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( ( OptionSide::Call == eOptionSide_ ) || ( OptionSide::Put == eOptionSide_ ) );
        assert( ( eType_ == InstrumentType::Option )
             || ( eType_ == InstrumentType::FuturesOption ) );
        assert( 0 < idInstrument.size() );   
        assert( 0 < idUnderlying.size() );
    };
    TableRowDef( // option with yymmdd
      idInstrument_t idInstrument_, InstrumentType::enumInstrumentTypes eType_, idExchange_t idExchange_,
      idInstrument_t idUnderlying_, 
      boost::uint16_t nYear_, boost::uint16_t nMonth_, boost::uint16_t nDay_,
      OptionSide::enumOptionSide eOptionSide_, double dblStrike_  )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ), idUnderlying( idUnderlying_ ),
      eCurrency( Currency::USD ), eCounterCurrency( Currency::USD ),
      eOptionSide( eOptionSide_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ), dblStrike( dblStrike_ ), 
      nIBContract( 0 ), nMultiplier( 100 ), dblMinTick( 0.01 ), nSignificantDigits( 2 ) {
        assert( ( OptionSide::Call == eOptionSide_ ) || ( OptionSide::Put == eOptionSide_ ) );
        assert( ( eType_ == InstrumentType::Option )
             || ( eType_ == InstrumentType::FuturesOption ) );
        assert( 0 < idInstrument.size() ); 
        assert( 0 < idUnderlying.size() );
    };
    TableRowDef( // currency
      const idInstrument_t& idInstrument_, const idInstrument_t& idCounterInstrument_,
      InstrumentType::enumInstrumentTypes eType_, idExchange_t idExchange_,
      Currency::enumCurrency eCurrency_, Currency::enumCurrency eCounterCurrency_ )
      : idInstrument( idInstrument_ ), eType( eType_ ), idExchange( idExchange_ ), 
        idUnderlying( idCounterInstrument_ ), eCurrency( eCurrency_ ), eCounterCurrency( eCounterCurrency_ ), 
        eOptionSide( OptionSide::Unknown ), nYear( 0 ), nMonth( 0 ), nDay( 0 ), dblStrike( 0.0 ), 
        nIBContract( 0 ), nMultiplier( 1 ), dblMinTick( 0.00005 ), nSignificantDigits( 5 ) {
          assert( eType_ == InstrumentType::Currency );
          assert( 0 < idInstrument.size() );
          assert( 0 < idUnderlying.size() );
    };
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "instrumentid" );
      ou::db::Constraint( a, "exchangeid", tablenames::sExchange, "exchangeid" );
      ou::db::Constraint( a, "underlyingid", tablenames::sInstrument, "instrumentid" );  // what happens with empty string?
    }
  };

  CInstrument( const TableRowDef& row );  // regular instruments
  CInstrument( const TableRowDef& row, pInstrument_t& pUnderlying ); // options, futuresoptions
  CInstrument( // equity / generic creation
    idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes type,
    const idExchange_t& sExchangeName 
     );
  CInstrument(   // future
    idInstrument_cref idInstrument, InstrumentType::enumInstrumentTypes type, 
    const idExchange_t& sExchangeName,
    boost::uint16_t year, boost::uint16_t month );
  CInstrument(   // option with yymm
    idInstrument_cref sInstrumentName, InstrumentType::enumInstrumentTypes type, 
    const idExchange_t& sExchangeName,
    boost::uint16_t year, boost::uint16_t month,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  CInstrument(   // option with yymmdd
    idInstrument_cref sInstrumentName, InstrumentType::enumInstrumentTypes type, 
    const idExchange_t& sExchangeName,
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  CInstrument(  // currency
    const idInstrument_t& idInstrument, const idInstrument_t& idCounterInstrument,
    InstrumentType::enumInstrumentTypes eType, const idExchange_t& idExchange,
    Currency::enumCurrency base, Currency::enumCurrency counter );
    
  virtual ~CInstrument(void);

  idInstrument_cref GetInstrumentName( void ) const { return m_row.idInstrument; };
  idInstrument_cref GetUnderlyingName( void );

  idInstrument_cref GetInstrumentName( eidProvider_t id );
  idInstrument_cref GetUnderlyingName( eidProvider_t id );

//  void SetUnderlying( pInstrument_t pUnderlying );

  void SetAlternateName( eidProvider_t, idInstrument_cref );

  typedef std::pair<idInstrument_cref,idInstrument_cref> pairNames_t;
  ou::Delegate<pairNames_t> OnAlternateNameAdded;  // key, alt
  ou::Delegate<pairNames_t> OnAlternateNameChanged;  // old, new

  InstrumentType::enumInstrumentTypes GetInstrumentType( void ) const { return m_row.eType; };
  bool IsOption( void ) const { return ( InstrumentType::Option == m_row.eType ); };
  bool IsFuture( void ) const { return ( InstrumentType::Future == m_row.eType ); };

  const std::string& GetExchangeName( void ) const { return m_row.idExchange; };
  void SetCurrency( Currency::enumCurrency eCurrency ) { m_row.eCurrency = eCurrency; };
  const char *GetCurrencyName( void ) const { return Currency::Name[ m_row.eCurrency ]; };

  double GetStrike( void ) const { return m_row.dblStrike; };
  boost::uint16_t GetExpiryYear( void ) const { return m_row.nYear; };
  boost::uint16_t GetExpiryMonth( void ) const { return m_row.nMonth; };
  boost::uint16_t GetExpiryDay( void ) const { return m_row.nDay; };
  OptionSide::enumOptionSide GetOptionSide( void ) { return m_row.eOptionSide; };

  void SetContract( boost::int32_t id ) { m_row.nIBContract = id; };
  boost::int32_t GetContract( void ) const { return m_row.nIBContract; };

  void SetMultiplier( boost::uint32_t nMultiplier ) { m_row.nMultiplier = nMultiplier; };
  boost::uint32_t GetMultiplier( void ) const { return m_row.nMultiplier; };

  void SetMinTick( double dblMinTick ) { m_row.dblMinTick = dblMinTick; };
  double GetMinTick( void ) const { return m_row.dblMinTick; };

  void SetSignificantDigits( boost::uint8_t nSignificantDigits ) { m_row.nSignificantDigits = nSignificantDigits; };
  boost::uint8_t GetSignificantDigits( void ) const { return m_row.nSignificantDigits; };

  typedef boost::posix_time::time_period dtrMarketOpenClose_t;
  void SetTimeLiquid( const ptime& dtOpen, const ptime& dtClose ) { m_dtrTimeLiquid = dtrMarketOpenClose_t( dtOpen, dtClose ); };
  const dtrMarketOpenClose_t& GetTimeLiquid( void ) const { return m_dtrTimeLiquid; };

  void SetTimeTrading( const ptime& dtOpen, const ptime& dtClose ) { m_dtrTimeTrading = dtrMarketOpenClose_t( dtOpen, dtClose ); };
  const dtrMarketOpenClose_t& GetTimeTrading( void ) const { return m_dtrTimeTrading; };

  bool operator==( const CInstrument& rhs ) const;

  const TableRowDef& GetRow( void ) const { return m_row; };

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

  dtrMarketOpenClose_t m_dtrTimeLiquid;
  dtrMarketOpenClose_t m_dtrTimeTrading;

  CInstrument( const CInstrument& );  // copy ctor
  CInstrument& operator=( const CInstrument& ); // assignement
};

//
// CAlternateInstrumentName
//

class CAlternateInstrumentName {
public:
  struct TableRowDef {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "providerid", idProvider ); 
      ou::db::Field( a, "alternateid", idAlternate );
      ou::db::Field( a, "instrumentid", idInstrument );
    }

    keytypes::eidProvider_t idProvider;
    keytypes::idInstrument_t idInstrument;
    keytypes::idInstrument_t idAlternate;
  };

  struct TableCreateDef: TableRowDef {
    template<class A>
    void Fields( A& a ) {
      TableRowDef::Fields( a );
      ou::db::Key( a, "providerid" );
      ou::db::Key( a, "alternateid" );
      ou::db::Constraint( a, "instrumentid", tablenames::sInstrument, "instrumentid" );
      //ou::db::Constraint( a, "alternateid", tablenames::sInstrument, "instrumentid" );  // don't think this one makes sense
      // set instrumentid as secondary index
    }
  };

protected:
private:

  TableRowDef m_row;

};

} // namespace tf
} // namespace ou
