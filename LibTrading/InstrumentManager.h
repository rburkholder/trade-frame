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

#include <string>
#include <map>

#include "Instrument.h"
//#include "InstrumentFile.h"
#include "ManagerBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CInstrumentManager
  : public ManagerBase<CInstrumentManager, CInstrument::idInstrument_t, CInstrument> {
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
    unsigned short year, unsigned short month );
  pInstrument_t ConstructOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymm
    unsigned short year, unsigned short month,
    //const idInstrument_t &sUnderlyingName,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  pInstrument_t ConstructOption(
    idInstrument_cref sInstrumentName, const std::string& sExchangeName,  // option with yymmdd
    unsigned short year, unsigned short month, unsigned short day,
    //const std::string &sUnderlyingName,
    pInstrument_t pUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ); 
  pInstrument_t ConstructCurrency( 
    idInstrument_cref sInstrumentName, 
    //const std::string& sUnderlyingName, // currency
    pInstrument_t pUnderlying,
    const std::string& sExchangeName, 
    Currency::enumCurrency base, Currency::enumCurrency counter );

  bool Exists( idInstrument_cref );
  bool Exists( pInstrument_cref );
  pInstrument_t Get( idInstrument_cref );

  // move these to IQFeed somewhere
//  pInstrument_t GetIQFeedInstrument( const std::string& sName );
//  pInstrument_t GetIQFeedInstrument( const std::string& sName, const std::string& sAlternateName );

protected:

  //CInstrumentFile file;

  void Assign( pInstrument_cref pInstrument );

private:

  typedef std::map<idInstrument_t,pInstrument_t> map_t;
  typedef map_t::iterator iterator;
  typedef std::pair<idInstrument_t,pInstrument_t> pair_t;

  map_t m_map;

  void HandleAlternateNameAdded( CInstrument::pairNames_t );
  void HandleAlternateNameChanged( CInstrument::pairNames_t );
};

} // namespace tf
} // namespace ou
