/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

// by including TFTrading/Managers.h in the .cpp file
//   all standard tables are constructed and maintained
//   by the respective subject matter manager

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTrading/KeyTypes.h>

#include <TFTrading/InstrumentManager.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class DBOps: public ou::db::Session {
public:

  DBOps();
  virtual ~DBOps();

  typedef Instrument::idInstrument_cref idInstrument_cref;
  //typedef Instrument::idInstrument_t idInstrument_t;

  //void LoadUnderlying( const ou::tf::keytypes::idInstrument_t& id, ou::tf::InstrumentManager::pInstrument_t& pInstrument );
  //bool LoadOptions( ou::tf::InstrumentManager::pInstrument_t& pUnderlying, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ); // uses OnNewInstrument
  bool LoadOptions( idInstrument_cref sInstrumentName, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ); // uses OnNewInstrument

  typedef FastDelegate0<> OnPopulateDatabaseHandler_t;
  void SetOnPopulateDatabaseHandler( OnPopulateDatabaseHandler_t function ) {
    OnPopulateDatabaseHandler = function;
  }

  typedef FastDelegate0<> OnLoadDatabaseHandler_t;
  void SetOnLoadDatabaseHandler( OnLoadDatabaseHandler_t function ) {
    OnLoadDatabaseHandler = function;
  }

  typedef FastDelegate1<ou::tf::InstrumentManager::pInstrument_t> OnNewInstrumentHandler_t;
  void SetOnNewInstrumentHandler( OnNewInstrumentHandler_t function ) {
    OnNewInstrument = function;
  }

protected:
private:

  OnPopulateDatabaseHandler_t OnPopulateDatabaseHandler;
  OnLoadDatabaseHandler_t OnLoadDatabaseHandler;
  OnNewInstrumentHandler_t OnNewInstrument;

  void HandlePopulateTables( ou::db::Session& session );
  void HandleLoadTables( ou::db::Session& session );
};

} // namespace tf
} // namespace ou
