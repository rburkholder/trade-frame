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

#include "Instrument.h"
#include "InstrumentFile.h"
#include "ManagerBase.h"

class CInstrumentManager: public ManagerBase<CInstrumentManager, std::string, CInstrument> {
public:
  CInstrumentManager(void);
  virtual ~CInstrumentManager(void);
  CInstrument::pInstrument_t GetIQFeedInstrument( const std::string& sName );
  CInstrument::pInstrument_t GetIQFeedInstrument( const std::string& sName, const std::string& sAlternateName );
protected:
  CInstrumentFile file;
private:
};

