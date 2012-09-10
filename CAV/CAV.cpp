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

#include <stdio.h>
#include <tchar.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFIQFeed/CurlGetMktSymbols.h>

#include <ioapi.h>
#include <ioapi_mem.h>
#include <unzip.h>

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

  {
    ou::tf::CurlGetMktSymbols cgms;

    char* sZipFile( "**inmem**" );
    char* sSourceName( "mktsymbols_v2.txt" );
    //char* sSourceName( "DIAX.CHM" );

    unzFile uf=NULL;
    int err=UNZ_OK;
    unz_file_info64 file_info;
    char filename_inzip[256];
    ourmemory_t om;
    om.base = 0;
    om.cur_offset = om.limit = om.size = 0;

    om.base = cgms.Buffer();
    om.size = cgms.Size();

    char* UnzippedFileContent;
    UnzippedFileContent = 0;

    zlib_filefunc64_def ffunc;

    fill_memory_filefunc64( &ffunc, &om );

    uf = unzOpen2_64(sZipFile, &ffunc);
    if ( 0 == uf ) 
      throw std::runtime_error( "open" );

    err = unzLocateFile( uf, sSourceName, 0 );
    if ( UNZ_OK != err ) 
      throw  std::runtime_error( "locate" );

    err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
    if ( UNZ_OK != err ) 
      throw  std::runtime_error( "get info" );

    err = unzOpenCurrentFile( uf );
    if ( UNZ_OK != err ) 
      throw  std::runtime_error( "open current" );

    UnzippedFileContent = new char[ file_info.uncompressed_size ];
    if ( 0 == UnzippedFileContent ) {
      throw  std::runtime_error( "UnzippedFileContent" );
    }

    int cnt = unzReadCurrentFile(uf,UnzippedFileContent,file_info.uncompressed_size);
    if ( file_info.uncompressed_size != cnt ) 
      throw  std::runtime_error( "read" );

    err = unzCloseCurrentFile( uf );
    if ( UNZ_OK != err ) 
      throw  std::runtime_error( "close current" );

    err = unzClose( uf );
    if ( UNZ_OK != err ) 
      throw  std::runtime_error( "close" );

    delete[] UnzippedFileContent;
  }

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

