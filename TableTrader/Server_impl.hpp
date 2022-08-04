/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:      Server_impl.hpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/03 17:14:41
 */

#pragma once

#include <memory>
#include <string>

namespace ou {
namespace tf {
  class db;
namespace iqfeed {
  class IQFeedProvider;
}
namespace ib {
  class TWS;
}
}
}

class Server_impl {
public:
  Server_impl();
  ~Server_impl();

  void Start( const std::string& sUnderlyingFuture );
protected:
private:

  enum EState { quiescent, connecting, connected, disconnecting, disconnected };
  EState m_state;

  std::shared_ptr<ou::tf::ib::TWS> m_pProviderTWS;
  std::shared_ptr<ou::tf::iqfeed::IQFeedProvider> m_pProviderIQFeed;

  std::unique_ptr<ou::tf::db> m_pdb;

  void Connected_IQFeed( int );
  void Connected_TWS( int );
  void Connected( int );

  void Disconnected_IQFeed( int );
  void Disconnected_TWS( int );
  void Disconnected( int );
};
