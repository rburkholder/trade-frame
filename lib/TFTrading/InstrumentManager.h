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

// 2011/03/16 add persist-to-db superclass for saving/retrieving instruments

#include <map>
#include <mutex>
#include <string>

#include <OUCommon/ManagerBase.h>

#include "KeyTypes.h"

#include "Exchange.h"
#include "Instrument.h"
#include "AlternateInstrumentNames.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class InstrumentManager
  : public ou::db::ManagerBase<InstrumentManager> {
public:

  using pInstrument_t = Instrument::pInstrument_t;
  using pInstrument_cref = Instrument::pInstrument_cref;
  using idInstrument_t = Instrument::idInstrument_t;
  using idInstrument_cref = Instrument::idInstrument_cref;

  InstrumentManager(void);
  virtual ~InstrumentManager(void);

  pInstrument_t ConstructInstrument(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
    InstrumentType::enumInstrumentType type = InstrumentType::Unknown );
  pInstrument_t ConstructFuture(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day );
  pInstrument_t ConstructFuturesOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
    OptionSide::enumOptionSide side,
    double strike );
  pInstrument_t ConstructOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
    OptionSide::enumOptionSide side,
    double strike );
  pInstrument_t ConstructCurrency(
    idInstrument_cref idInstrumentName,
//    idInstrument_cref idCounterInstrument,
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

protected:

  void Assign( pInstrument_cref pInstrument );
  bool LoadInstrument( idInstrument_t idInstrument, pInstrument_t& pInstrument );
  void LoadAlternateInstrumentNames( pInstrument_t& pInstrument );

private:

  using mapInstruments_t = std::map<idInstrument_t,pInstrument_t>;
  using iterInstruments_t = mapInstruments_t::iterator;
  mapInstruments_t m_mapInstruments;

  std::mutex m_mutexLoadInstrument;

  void SaveAlternateInstrumentName( const AlternateInstrumentName::TableRowDef& );
  void SaveAlternateInstrumentName(
    const keytypes::eidProvider_t&, const keytypes::idInstrument_t&, const keytypes::idInstrument_t& );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );

  void HandleAlternateNameAdded( const Instrument::AlternateNameChangeInfo_t& );
  void HandleAlternateNameChanged( const Instrument::AlternateNameChangeInfo_t& );
};

} // namespace tf
} // namespace ou
