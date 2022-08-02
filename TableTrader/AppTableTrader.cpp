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
 * File:      AppTableTrader.cpp
 * Author:    raymond@burkholder.net
 * Created:   2022/08/02 13:42:30
 */

//#include <Wt/WApplication.h>
//#include <Wt/WLogger.h>
//#include <Wt/WContainerWidget.h>
#include <Wt/WAnchor.h>
#include <Wt/WText.h>

#include "AppTableTrader.hpp"

namespace model {
  class Account;
  class Task;
} // namespace model

// https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WApplication.html

AppTableTrader::AppTableTrader( const Wt::WEnvironment& env )
: Wt::WApplication( env ),
  m_pServer( nullptr )
{
  m_pServer = dynamic_cast<Server*>( env.server() );

  useStyleSheet("style/tt.css");

  internalPathChanged().connect( this, &AppTableTrader::HandleInternalPathChanged );
  internalPathInvalid().connect( this, &AppTableTrader::HandleInternalPathInvalid );

  namespace ph = std::placeholders;
  RegisterPath( "/", std::bind( &AppTableTrader::HomeRoot, this, ph::_1 ) );

  struct callback {
    void operator()( Wt::WContainerWidget* pcw ) {

    }
  };

  namespace ph = std::placeholders;
  TemplatePage( root(), [this](Wt::WContainerWidget* pcw){
    ShowMainMenu( pcw );
  } );
}

AppTableTrader::~AppTableTrader( ) { }

void AppTableTrader::initialize() {
  Wt::WApplication::log( "info" ) << "AppTableTrader::initialize()";
}

void AppTableTrader::finalize() {
  Wt::WApplication::log( "info" ) << "AppTableTrader::finalize()";
}

void AppTableTrader::HandleInternalPathChanged( const std::string& sPath ) {
  root()->clear();
  std::string sMessage;
  sMessage += "HandleInternalPathChanged: ";
  sMessage += sPath;
  mapInternalPathChanged_t::const_iterator iter = m_mapInternalPathChanged.find( sPath );
  if ( m_mapInternalPathChanged.end() != iter ) {
    sMessage += "iter";
    iter->second( root() );
  }
  else {
    // default home page, or error page, and register a default page
    sMessage += "root";
    Home( root() );
  }
  Wt::WApplication::log( "info" ) << sMessage;
}

void AppTableTrader::HandleInternalPathInvalid( const std::string& s ) {
  Wt::WApplication::log( "warn" ) << "*** HandleInternalPathInvalid: " << s;
}

void AppTableTrader::RegisterPath( const std::string& sPath, const slotInternalPathChanged_t& slot ) {
  mapInternalPathChanged_t::const_iterator iter = m_mapInternalPathChanged.find( sPath );
  if ( m_mapInternalPathChanged.end() != iter )
    std::runtime_error( "path exists" );
  m_mapInternalPathChanged.insert( mapInternalPathChanged_t::value_type( sPath, slot ) );
}

void AppTableTrader::UnRegisterPath( const std::string& sPath ) {
  mapInternalPathChanged_t::const_iterator iter = m_mapInternalPathChanged.find( sPath );
  if ( m_mapInternalPathChanged.end() == iter )
    std::runtime_error( "path not found" );
  m_mapInternalPathChanged.erase( iter );
}

void AppTableTrader::AddLink( Wt::WContainerWidget* pcw, const std::string& sClass, const std::string& sPath, const std::string& sAnchor ) {
  Wt::WContainerWidget* pContainer
    = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );

  pContainer->setStyleClass( sClass );

  // https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WAnchor.html
  Wt::WLink link( Wt::LinkType::InternalPath, sPath );

  Wt::WAnchor* pAnchor
    = pContainer->addWidget( std::make_unique<Wt::WAnchor>( link, sAnchor ) );

  // WApplication::instance()->internalPathChanged().connect(this, &DocsListWidget::onInternalPathChange);
}

void AppTableTrader::ShowMainMenu( Wt::WContainerWidget* pcw ) {

  // still need to show the widget, and there needs to be a click event so when
  //  logged in we can refresh
  // so devote a page to the login, clean out main menu,
  // but still need an event on when logged in is successful
  //   because nothing else happens on the page
/*
  if ( m_pAuth->LoggedIn() ) {

    AddLink( pcw, "member", "/show/addresses", "Address List" );
    AddLink( pcw, "member", "/admin/tables/upload", "Upload" );

    // <a id="ov7qcp1"
    //    href="admin/tables/populate/mysql?wtd=jLpA57e4vgIIoYxI"
    //    class="Wt-rr"><span id="ov7qcp0">Populate Tables: MySQL sourced</span>
    //    </a>
    auto pMenu = new Wt::WContainerWidget( pcw );
    // test against database, and figure out which can be shown, particularily the tables/init one
    pMenu->setList(true); // ==> sub WContainerWidget added as <li> elements
    AddLink( pMenu, "admin", "/admin/tables/init", "Init Tables" );
    AddLink( pMenu, "admin", "/admin/tables/populate/basics", "Populate Tables: Basics" );
    // use the Upload class to do this one:
    AddLink( pMenu, "admin", "/ad min/tables/populate/mysql",  "Populate Tables: MySQL sourced" );
    // use the Upload class to do this one:
    AddLink( pMenu, "admin", "/admin/tables/populate/smcxml", "Populate Tables: SMC XML sourced" );

  }
  else {

  }
  */

  //pcw->addWidget( std::make_unique<page::TimeRecord>( m_session ) );

}

void AppTableTrader::HomeRoot( Wt::WContainerWidget* pcw ) {
  std::cout << "root home" << std::endl;

  namespace ph = std::placeholders;
  TemplatePage( pcw, std::bind( &AppTableTrader::ShowMainMenu, this, ph::_1 ) );
}

void AppTableTrader::Home( Wt::WContainerWidget* pcw ) {

  namespace ph = std::placeholders;
  TemplatePage( pcw, std::bind( &AppTableTrader::ShowMainMenu, this, ph::_1 ) );
}

void AppTableTrader::TemplatePage(Wt::WContainerWidget* pcw, fTemplate_t f) {

  static const std::string sTitle( "Table Trader" );
  setTitle( sTitle );

  auto title = pcw->addWidget( std::make_unique<Wt::WText>( sTitle ) );

  pcw->addWidget( std::make_unique<Wt::WBreak>() );

  //auto pTitle( new Wt::WText( sTitle ) );
  //pTitle->setStyleClass( "MainTitle" );

  //pcw->addWidget( pTitle );

    // should show up to the right of the title
  //if ( this->internalPathMatches( "/auth/signin" ) ) {
    // don't show sign in status
  //}
  //else {
//    if ( m_pAuth->LoggedIn() ) {
//      //pcw->addWidget( m_pAuth->NewAuthWidget() );
//      AddLink( pcw, "member", "/auth/signout", "Sign Out" );
//      AddLink( pcw, "member", "/member/home", "Home" );
//    }
//    else {
//      AddLink( pcw, "admin", "/auth/signin", "Sign In" );
//      AddLink( pcw, "default", "/home", "Home" );
//    }

//  }

  f( pcw );
}