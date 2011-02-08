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

#include <TFTrading/TradingEnumerations.h>

#include "IQFeedInstrumentFile.h"


// http://www.dtniq.com/product/mktsymbols.zip  <-- deprecated
// http://www.dtniq.com/product/mktsymbols_v2.zip

// Is designed for running in debug mode at the moment.

namespace ou { // One Unified
namespace tf { // TradeFrame

class CIQFeedSymbolFile: public CInstrumentFile {
public:
  CIQFeedSymbolFile(void);
  ~CIQFeedSymbolFile(void);

  bool Load( const std::string& sTxtFileName, const std::string& sDbFileName );

protected:

private:
};

} // namespace tf
} // namespace ou
