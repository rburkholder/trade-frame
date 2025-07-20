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

#include <mutex>
#include <string>
#include <vector>
#include <functional>

#include <OUCommon/Network.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class OptionChainQuery
  : public ou::Network<OptionChainQuery> {
    friend ou::Network<OptionChainQuery>;
public:

  enum Month { jan='F', feb='G', mar='H', apr='J', may='K', jun='M',
               jul='N', aug='Q', sep='U', oct='V', nov='X', dec='Z' };
  enum Side { put='p', call='c' };

  using vSymbol_t = std::vector<std::string>;

  struct FuturesList {
    vSymbol_t vSymbol;
  };

  struct OptionList {
    std::string sUnderlying;
    vSymbol_t vSymbol;
  };

  using fConnected_t = std::function<void()>;
  using fFuturesList_t = std::function<void(const FuturesList&)>;
  using fOptionList_t = std::function<void(const OptionList&)>;

  OptionChainQuery( fConnected_t&& );
  ~OptionChainQuery();

  void Connect();

  // list of futures symbols based upon base#: ie, QGC#, @ES#
  void QueryFuturesChain(
    const std::string& sSymbol,
    const std::string& sMonthCodes, // see above
    const std::string& sYears,      // last digit
    const std::string& sNearMonths, // 0..4
    fFuturesList_t&&
    );

  void QueryFuturesOptionChain(
    const std::string& sSymbol,
    const std::string& sSide,
    const std::string& sMonthCodes, // see above
    const std::string& sYears,      // last digit
    const std::string& sNearMonths, // 0..4
    fOptionList_t&&
    );

  void QueryEquityOptionChain(
    const std::string& sSymbol,
    const std::string& sSide,
    const std::string& sMonthCodes, // see above
    const std::string& sNearMonths, // 0..4
    const std::string& sFilterType, // 0 no filter, 1 filter on strike range, 2 filter on #contracts in/out money
    const std::string& sFilterOne,  // 0 ignored, 1 begin strike, 2 #contracts in the money
    const std::string& sFilterTwo,  // 0 ignored, 1 end strike, 2 #contracts out of the money
    fOptionList_t&&
    );

  void Disconnect();

protected:

  using inherited_t = ou::Network<OptionChainQuery>;
  using linebuffer_t = inherited_t::linebuffer_t;

  // called by Network via CRTP
  void OnNetworkConnected();
  void OnNetworkDisconnected();
  void OnNetworkError( size_t e );
  void OnNetworkSendDone();
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

private:

  // NOTE: this isn't going to work with concurrent requests
  //   may need to store state in the map entry

  enum class EState { quiescent, response, done };
  EState m_state;

  std::mutex m_mutexMapRequest;

  fConnected_t m_fConnected;

  using mapFutures_t = std::map<std::string,fFuturesList_t>;
  mapFutures_t m_mapFutures;

  using mapOptions_t = std::map<std::string,fOptionList_t>;
  mapOptions_t m_mapOptions;

};

} // namespace iqfeed
} // namespace tf
} // namespace ou
