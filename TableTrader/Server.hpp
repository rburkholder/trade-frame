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
 * File:      Server.hpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/02 09:58:23
 */

#ifndef SERVER_H
#define SERVER_H

#include <Wt/WServer.h>

#include "Config.hpp"

class Server_impl;

class Server: public Wt::WServer {
public:

  Server(
    int argc, char *argv[],
    const config::Choices&,
    const std::string &wtConfigurationFile=std::string()
    );
  virtual ~Server();

  bool ValidateLogin( const std::string& sUserName, const std::string& sPassWord );

  using fAddCandidateFutures_t = std::function<void(const std::string&)>;
  void AddCandidateFutures( fAddCandidateFutures_t&& );

  using fUpdateUnderlyingInfo_t = std::function<void(const std::string&, const std::string&)>; // name, multiplier
  using fUpdateUnderlyingPrice_t = std::function<void(const std::string&)>; // price
  using fUpdateOptionExpiries_t = std::function<void()>; // will need to convert of std::vector or a callback

  void Start(
    const std::string& sSessionId, const std::string& sUnderlyingFuture,
    fUpdateUnderlyingInfo_t&&,
    fUpdateUnderlyingPrice_t&&,
    fUpdateOptionExpiries_t
    );

protected:
private:
  const config::Choices& m_choices;

  std::unique_ptr<Server_impl> m_implServer;

  fUpdateUnderlyingInfo_t m_fUpdateUnderlyingInfo;
  fUpdateUnderlyingPrice_t m_fUpdateUnderlyingPrice;
  fUpdateOptionExpiries_t m_fUpdateOptionExpiries;
};

#endif /* SERVER_H */

