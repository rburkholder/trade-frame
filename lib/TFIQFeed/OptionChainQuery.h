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

#include <OUCommon/ReusableBuffers.h>
#include <OUCommon/Network.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class OptionChainQuery
  : public ou::Network<OptionChainQuery> {
public:
  
  enum Month { jan='F', feb='G', mar='H', apr='J', may='K', jun='M', 
               jul='N', aug='Q', sep='U', oct='V', nov='X', dec='Z' };
  enum Side { put='p', call='c' };

  OptionChainQuery();
  virtual ~OptionChainQuery(void);

  typedef FastDelegate0<void> OnSymbolListReceivedHandler;
  void SetOnSymbolListRecieved( OnSymbolListReceivedHandler function ) {
    OnSymbolListReceived = function;
  }
  
  void QueryFutureChain(
    const std::string& sSymbol, 
    const std::string& sMonthCodes,  // see above
    const std::string& sYears, // last digit
    const std::string& sNearMonths, // 0..4
    const std::string& sRequestId
    );
  void QueryFutureOptionChain(
    const std::string& sSymbol, 
    const std::string& sSide,
    const std::string& sMonthCodes,  // see above
    const std::string& sYears, // last digit
    const std::string& sNearMonths, // 0..4
    const std::string& sRequestId
    );
  void QueryEquityOptionChain(
    const std::string& sSymbol, 
    const std::string& sSide,
    const std::string& sMonthCodes,  // see above
    const std::string& sNearMonths, // 0..4
    const std::string& sFilterType, // 0 no filter, 1 filter on strike range, 2 filter on #contracts in/out money
    const std::string& sFilterOne, // 0 ignored, 1 begin strike, 2 #contracts in the money
    const std::string& sFilterTwo, // 0 ignored, 1 end strike, 2 #contracts out of the money
    const std::string& sRequestId
    );

  std::vector<std::string *> m_vOptionSymbols;

protected:
  virtual void OnNewResponse( const char *szLine );
  void AddOptionSymbol( const char *s, unsigned short cnt );
private:
  OnSymbolListReceivedHandler OnSymbolListReceived;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou
