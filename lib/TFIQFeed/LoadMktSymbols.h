/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// Started 2012/10/14

#include "InMemoryMktSymbolList.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace detail {
  // shared between debug and release
  extern const std::string sFileNameMarketSymbolsText;
  extern const std::string sFileNameMarketSymbolsBinary;
}

namespace MktSymbolLoadType {
  enum Enum { Download, LoadTextFromDisk };
}

void LoadMktSymbols( InMemoryMktSymbolList& symbols, MktSymbolLoadType::Enum, bool bSaveTextToDisk, const std::string& sName = detail::sFileNameMarketSymbolsText );

} // namespace iqfeed
} // namespace tf
} // namespace ou
