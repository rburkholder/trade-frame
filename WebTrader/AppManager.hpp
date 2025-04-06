/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:      AppManager.hpp
 * Author:    raymond@burkholder.net
 * Project:   WebTrader
 * Created:   2025/04/05 21:25:03
 */

#ifndef APPMANAGER_H
#define APPMANAGER_H

#include "Config.hpp"

#include "Server.hpp"

class AppManager {
public:
  AppManager( int argc, char** argv, const config::Choices& );
  virtual ~AppManager( );
  void Start();
private:

  Server m_server;

};

#endif /* APPMANAGER_H */

