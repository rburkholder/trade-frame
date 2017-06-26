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

#include <vector>
#include <string>

#include <OUCommon//FastDelegate.h>
using namespace fastdelegate;

//#include "IQFeedRetrieval.h"

#include <OUCommon/ReusableBuffers.h>
#include <OUCommon/Network.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class OptionChainQuery
  : public ou::Network<OptionChainQuery> {
public:

  //OptionChainQuery(IQFeedProvider *pProvider, );
  OptionChainQuery( const std::string& );
  virtual ~OptionChainQuery(void);

  typedef FastDelegate0<void> OnSymbolListReceivedHandler;
  void SetOnSymbolListRecieved( OnSymbolListReceivedHandler function ) {
    OnSymbolListReceived = function;
  }

  std::vector<std::string *> m_vOptionSymbols;

protected:
  virtual void OnNewResponse( const char *szLine );
  void AddOptionSymbol( const char *s, unsigned short cnt );
private:
  OnSymbolListReceivedHandler OnSymbolListReceived;
  const std::string& m_sUnderlying;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
