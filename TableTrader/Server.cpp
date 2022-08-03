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
 * File:      Server.cpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/02 09:58:23
 */

#include "Server.hpp"

Server::Server(
  int argc,
  char *argv[],
  const config::Choices& choices,
  const std::string &wtConfigurationFile
)
: Wt::WServer( argc, argv, wtConfigurationFile )
, m_choices( choices )
{
}

Server::~Server() {
}

bool Server::ValidateLogin( const std::string& sUserName, const std::string& sPassWord ) {
  return ( ( sUserName == m_choices.m_sUIUserName ) && ( sPassWord == m_choices.m_sUIPassWord ) );
}

void Server::AddUnderlyingFutures( fAddUnderlyingFutures_t&& f ) {
  for ( const std::string& s: m_choices.m_vUnderlyingFuture ) {
    f( s );
  }
}