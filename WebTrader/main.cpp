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
 * File:    main.cpp
 * Author:  raymond@burkholder.net
 * Project: WebTrader
 * Created: 2025/04/05 21:25:03
 */

#include "Config.hpp"
#include "AppManager.hpp"

// run application in ./x64
// add the following command line parameters:
//   --docroot=web;/favicon.ico,/resources,/style,/image --http-address=0.0.0.0 --http-port=8082 --config=etc/wt_config.xml

// https://www.webtoolkit.eu/wt/doc/reference/html/InstallationUnix.html
// https://www.webtoolkit.eu/wt/doc/reference/html/overview.html

// cp -r ../libs-build/wt/resources web/

int main( int argc, char** argv ) {

  static const std::string sAppName( "WebTrader" );

  config::Choices choices;
  if ( Load( "etc/choices.cfg", choices ) ) {
    AppManager manager( argc, argv, choices );
    manager.Start();
  }
  else {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


