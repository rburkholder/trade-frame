/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// CAV.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

//#include <stdio.h>
//#include <tchar.h>

#include <fstream>
#include <vector>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFIQFeed/ParseMktSymbols.h>

#include "CAV.h"

IMPLEMENT_APP(AppCollectAndView)

bool AppCollectAndView::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Collect And View" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 500, 600 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();
/*
  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND|wxALIGN_LEFT, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );
*/
  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  // load mktsymbols somewhere

    std::ifstream file;
    char* name = "mktsymbols_v2.txt";
    std::cout << "Opening Input Instrument File ";
    std::cout << name;
    std::cout << " ... ";
    file.open( name );
    std::cout << std::endl;

    std::cout << "Loading Symbols ..." << std::endl;

    char line[ 500 ];
    unsigned int cntLines( 0 );

    typedef ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc_t;
    ou::tf::iqfeed::MktSymbolsParser<const char*> parser;
    std::vector<size_t> vSymbolTypeStats( sc_t::_Count );  // number of symbols of this SymbolType

    file.getline( line, 500 );  // remove header line
    file.getline( line, 500 );
    while ( !file.fail() ) {

      ++cntLines;  // number data lines processed

      ou::tf::iqfeed::MarketSymbol::TableRowDef trd;
      const char* pLine1( line );
      const char* pLine2( line + 500 );

      // try http://stackoverflow.com/questions/2291802/is-there-a-c-iterator-that-can-iterate-over-a-file-line-by-line

      try {
        bool b = qi::parse( pLine1, pLine2, parser, trd );
        if ( b ) {
          vSymbolTypeStats[ trd.sc ]++;
          if ( sc_t::Unknown == trd.sc ) {
            // set marker not to save record
            std::cout << "Unknown symbol type for:  " << trd.sSymbol << std::endl;
          }

        }
        else {
          std::cout << "problems parsing" << std::endl;
        }
        if ( 0 == trd.sDescription.length() ) {
          std::cout << trd.sSymbol << ": missing description" << std::endl;
        }
      }
      catch (...) {
        std::cout << "broken" << std::endl;
      }

//      std::cout << trd.sSymbol << std::endl;

      file.getline( line, 500 );
    }
    file.close();

  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppCollectAndView::HandlePopulateDatabase ) );
  m_db.Open( "cav.db" );

  return 1;

}

int AppCollectAndView::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppCollectAndView::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountSimulator
    = ou::tf::CAccountManager::Instance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

//  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
//    //= ou::tf::CPortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "SemiAuto" );
//    = ou::tf::CPortfolioManager::Instance().ConstructPortfolio( "pflioOptions", "aoRay", "options" );

}

