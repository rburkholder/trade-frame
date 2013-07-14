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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include "Database.h"

#include <TFTrading/KeyTypes.h>

#include <TFTrading/InstrumentManager.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class DBOps: public ou::db::Session {
public:

  DBOps(void);
  ~DBOps(void);

  void LoadUnderlying( const ou::tf::keytypes::idInstrument_t& id, ou::tf::InstrumentManager::pInstrument_t& pInstrument );
  bool LoadOptions( ou::tf::InstrumentManager::pInstrument_t& pUnderlying, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ); // uses OnNewInstrument

  typedef FastDelegate0<> OnPopulateDatabaseHandler_t;
  void SetOnPopulateDatabaseHandler( OnPopulateDatabaseHandler_t function ) {
    OnPopulateDatabaseHandler = function;
  }

  typedef FastDelegate1<ou::tf::InstrumentManager::pInstrument_t> OnNewInstrumentHandler_t;
  void SetOnNewInstrumentHandler( OnNewInstrumentHandler_t function ) {
    OnNewInstrument = function;
  }

protected:
private:

  OnPopulateDatabaseHandler_t OnPopulateDatabaseHandler;
  OnNewInstrumentHandler_t OnNewInstrument;

  void HandlePopulateTables( ou::db::Session& session );
};

} // namespace tf
} // namespace ou
