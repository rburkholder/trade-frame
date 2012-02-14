/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// StrategyRunner.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <wx/bitmap.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include "StrategyRunner.h"

IMPLEMENT_APP(AppStrategyRunner)

bool AppStrategyRunner::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Strategy Simulator" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
//  m_pFrameMain->SetSize( 4400, 1500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxHORIZONTAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxVERTICAL );
  m_sizerMain->Add( m_sizerControls, 0, wxALL, 5 );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxBOTTOM, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxBOTTOM, 5);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 02, 10 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 05, 18 ) );

  m_pFrameMain->Show( true );

  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppStrategyRunner::HandlePopulateDatabase ) );

  return 1;

}

int AppStrategyRunner::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppStrategyRunner::HandleBtnStart( void ) {

  m_pPanelOptionsParameters->SetBtnStartEnable( false);

  std::string sDbName;

  if ( m_bExecConnected && m_bData1Connected && m_bData2Connected ) {
    if ( ou::tf::keytypes::EProviderSimulator == m_pExecutionProvider->ID() ) {
      sDbName = ":memory:";
    }
    else {
      sDbName = "StrategyTradeOptions.db";
    }

    assert( 0 != sDbName.length() );
    m_db.Open( sDbName );

    m_pStrategyTradeOptions = new StrategyTradeOptions( m_pExecutionProvider, m_pData1Provider, m_pData2Provider );
    m_pStrategyTradeOptions->Start( 
      m_pPanelOptionsParameters->GetUnderlying(),
      m_pPanelOptionsParameters->GetOptionNearDate(), 
      m_pPanelOptionsParameters->GetOptionFarDate()
      );
    m_pPanelOptionsParameters->SetBtnStopEnable( true );
  }
  else {
    m_pPanelOptionsParameters->SetBtnStartEnable( true);
  }
}

void AppStrategyRunner::HandleBtnStop( void ) {
  m_pStrategyTradeOptions->Stop();
  m_pPanelOptionsParameters->SetBtnStartEnable( true);
}

void AppStrategyRunner::HandleBtnSave( void ) {
}

void AppStrategyRunner::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
    //= ou::tf::CPortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "SemiAuto" );
    = ou::tf::CPortfolioManager::Instance().ConstructPortfolio( "pflioOptions", "aoRay", "options" );

}

