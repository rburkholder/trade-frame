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
 * File:      AppManager.cpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/01 13:31:07
 */

#include "AppTableTrader.hpp"

#include "AppManager.hpp"

static std::unique_ptr<Wt::WApplication> CreateAppTableTrader( const Wt::WEnvironment& env ) {
  //
  // Optionally, check the environment and redirect to an error page.
  //
  bool valid( true );
  std::unique_ptr<AppTableTrader> app;
  if ( !valid ) {
    app = std::make_unique<AppTableTrader>(env);
    app->redirect("error.html");
    app->quit();
  } else {
    // usually you will specialize your application class
    app = std::make_unique<AppTableTrader>(env);
    //
    // Add widgets to app->root() and return the application object.
    //
  }
  return app;
}

AppManager::AppManager( int argc, char** argv )
: m_server( argv[0] )
{
    m_server.setServerConfiguration( argc, argv, WTHTTP_CONFIGURATION );
    m_server.addEntryPoint( Wt::EntryPointType::Application, CreateAppTableTrader );
}

AppManager::~AppManager( ) { }


void AppManager::Start() {

  try {

    //UserAuth::configureAuth();

    if ( m_server.start() ) {
      Wt::WServer::waitForShutdown();
      m_server.stop();
    }
  }
  catch ( Wt::WServer::Exception& e ) {
    std::cerr << e.what() << std::endl;
  }
  catch ( std::exception &e ) {
    std::cerr << "exception: " << e.what() << std::endl;
  }

}