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

// 2011/03/16  add persist-to-db superclass for saving/retrieving instruments

#include <map>
#include <string>

#include <OUCommon/ManagerBase.h>

#include "KeyTypes.h"

#include "Instrument.h"
#include "AlternateInstrumentNames.h"
#include "Exchange.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class InstrumentManager
  : public ou::db::ManagerBase<InstrumentManager> {
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef Instrument::pInstrument_cref pInstrument_cref;
  typedef Instrument::idInstrument_t idInstrument_t;
  typedef Instrument::idInstrument_cref idInstrument_cref;

  InstrumentManager(void);
  virtual ~InstrumentManager(void);

  pInstrument_t ConstructInstrument(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
    InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  pInstrument_t ConstructFuture(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
    boost::uint16_t year, boost::uint16_t month );
  pInstrument_t ConstructOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymm
    boost::uint16_t year, boost::uint16_t month,
//    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side,
    double strike );
  pInstrument_t ConstructOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
//    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side,
    double strike );
  pInstrument_t ConstructCurrency(
    idInstrument_cref idInstrumentName,
//    idInstrument_cref idCounterInstrument,
    //pInstrument_t pUnderlying,
    const std::string& sExchangeName,
    Currency::enumCurrency base, Currency::enumCurrency counter );

  void Register( pInstrument_t& pInstrument );

  bool Exists( idInstrument_cref );
  bool Exists( idInstrument_cref, pInstrument_t& );
  bool Exists( pInstrument_cref );
  pInstrument_t Get( idInstrument_cref ); // for getting existing associated with id
  void Delete( idInstrument_cref );

  template<typename F> void ScanOptions( F f, idInstrument_cref, boost::uint16_t year, boost::uint16_t month, boost::uint16_t day );

  virtual void AttachToSession( ou::db::Session* pSession );
  virtual void DetachFromSession( ou::db::Session* pSession );

  // move these to IQFeed somewhere
//  pInstrument_t GetIQFeedInstrument( const std::string& sName );
//  pInstrument_t GetIQFeedInstrument( const std::string& sName, const std::string& sAlternateName );

protected:

  //CInstrumentFile file;

  void Assign( pInstrument_cref pInstrument );
  bool LoadInstrument( idInstrument_t idInstrument, pInstrument_t& pInstrument );
  void LoadAlternateInstrumentNames( pInstrument_t& pInstrument );

private:

  typedef std::map<idInstrument_t,pInstrument_t> map_t;
  typedef map_t::iterator iterMap;
  typedef std::pair<idInstrument_t,pInstrument_t> pair_t;

  map_t m_map;

  void SaveAlternateInstrumentName( const AlternateInstrumentName::TableRowDef& );
  void SaveAlternateInstrumentName(
    const keytypes::eidProvider_t&, const keytypes::idInstrument_t&, const keytypes::idInstrument_t& );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );

  void HandleAlternateNameAdded( const Instrument::AlternateNameChangeInfo_t& );
  void HandleAlternateNameChanged( const Instrument::AlternateNameChangeInfo_t& );
};

namespace InstrumentManagerQueries {
  struct OptionSelection {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "underlyingid", idInstrument );  // 20151227 idUnderlying has been removed, is this variable still useful?
      ou::db::Field( a, "year", nYear );
      ou::db::Field( a, "month", nMonth );
      ou::db::Field( a, "day", nDay );
    }
    const ou::tf::keytypes::idInstrument_t& idInstrument;
    boost::uint16_t nYear; // future, option
    boost::uint16_t nMonth; // future, option
    boost::uint16_t nDay; // future, option
    OptionSelection( const ou::tf::keytypes::idInstrument_t& idInstrument_, boost::uint16_t nYear_, boost::uint16_t nMonth_, boost::uint16_t nDay_ )
      : idInstrument( idInstrument_ ), nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ) {};
  };

  struct OptionSymbolName {
    template<class A>
    void Fields( A& a ) {
      ou::db::Field( a, "instrumentid", idInstrument );
    }
    ou::tf::keytypes::idInstrument_t idInstrument;
    OptionSymbolName( void ) {};
  };
}

template<typename F>
void InstrumentManager::ScanOptions( F f, idInstrument_cref id, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ) {
  InstrumentManagerQueries::OptionSelection idInstrument( id, nYear, nMonth, nDay );
  ou::db::QueryFields<InstrumentManagerQueries::OptionSelection>::pQueryFields_t pExistsQuery // shouldn't do a * as fields may change order
    = m_pSession->SQL<InstrumentManagerQueries::OptionSelection>(
      "select instrumentid from instruments", idInstrument ).Where( "underlyingid = ? and year = ? and month = ? and day = ?" ).NoExecute();
  m_pSession->Bind<InstrumentManagerQueries::OptionSelection>( pExistsQuery );
  InstrumentManagerQueries::OptionSymbolName name;
  pInstrument_t pInstrument;
  while ( m_pSession->Execute( pExistsQuery ) ) {  // <- need to be able to execute on query pointer, since there is session pointer in every query
    m_pSession->Columns<InstrumentManagerQueries::OptionSelection, InstrumentManagerQueries::OptionSymbolName>( pExistsQuery, name );
    pInstrument = Get( name.idInstrument );
    f( pInstrument );
  }
}

} // namespace tf
} // namespace ou
