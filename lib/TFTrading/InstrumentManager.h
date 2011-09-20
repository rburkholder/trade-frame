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

#include "KeyTypes.h"

#include "Instrument.h"
#include "Exchange.h"
#include "ManagerBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CInstrumentManager
  : public ManagerBase<CInstrumentManager> {
public:

  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_cref pInstrument_cref;
  typedef CInstrument::idInstrument_t idInstrument_t;
  typedef CInstrument::idInstrument_cref idInstrument_cref;

  CInstrumentManager(void);
  ~CInstrumentManager(void);

  pInstrument_t ConstructInstrument( 
    idInstrument_cref sInstrumentName, const std::string& sExchangeName, // generic
    InstrumentType::enumInstrumentTypes type = InstrumentType::Unknown );
  pInstrument_t ConstructFuture(     
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // future
    boost::uint16_t year, boost::uint16_t month );
  pInstrument_t ConstructOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymm
    boost::uint16_t year, boost::uint16_t month,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  pInstrument_t ConstructOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
    boost::uint16_t year, boost::uint16_t month, boost::uint16_t day,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  pInstrument_t ConstructCurrency( 
    idInstrument_cref idInstrumentName, 
    idInstrument_cref idCounterInstrument,
    //pInstrument_t pUnderlying,
    const std::string& sExchangeName, 
    Currency::enumCurrency base, Currency::enumCurrency counter );

  void Construct( pInstrument_t& pInstrument );

  bool Exists( idInstrument_cref );
  bool Exists( pInstrument_cref );
  pInstrument_t Get( idInstrument_cref ); // for getting existing associated with id
  void Delete( idInstrument_cref );

  void AttachToSession( ou::db::CSession* pSession );
  void DetachFromSession( ou::db::CSession* pSession );

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

  void HandleRegisterTables( ou::db::CSession& session );
  void HandleRegisterRows( ou::db::CSession& session );
  void HandlePopulateTables( ou::db::CSession& session );

  void HandleAlternateNameAdded( CInstrument::pairNames_t );
  void HandleAlternateNameChanged( CInstrument::pairNames_t );
};


} // namespace tf
} // namespace ou
