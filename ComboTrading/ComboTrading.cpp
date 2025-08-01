/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started 2015/11/08

#include <memory>
#include <algorithm>
#include <functional>

#include <boost/lexical_cast.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <wx/app.h>
#include <wx/timer.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>

#include <TFIQFeed/BuildSymbolName.h>
#include <TFIQFeed/BuildInstrument.h>

#include <TFVuTrading/DragDropInstrument.h>

#include "ComboTrading.h"

/*
 * 2019/03/05
 * Books of inspiration:
 *
 *  0470920157 No-Hype Options Trading: Myths, Realities, and Strategies That Really Work  == 2011 by Kerry W. Given
 *    page 108 shows historical volatility chart as backdrop, edge could be used as entry for edges of back spreads
 *
 *  978-1-118-98058-3 Profiting from Weekly Options == 2015 by  Robert J. Seifert
 *     pg 91 credit spread aka vertical spread
 *     pg 98 2 to 1 backspread
 *
 *
 * 20160522 Issues:
 *   don't save without IB contract - should be good now, but validate
 *   need to watch implied volatility
 *   LiveChart:
 *     future with implied volatility curve (pull from HedgedBollinger
 *     futureoptions, options: track implied volatility
 *
 *
 * 20151109 To Do:
 *   based upon Day Trading Options by Jeff Augen
 *   read cboe file and sort symbols - done
 *   run the pivot scanner, filter by volume and volatility (install in library of scanners)
 *     or select based upon the c-c, o-c, c-o profiles and volatility surfaces
 *   sort top and pick 10 symbols (watch daily while building remaining code)
 *   watch with underlying plus 2 or 3 strike call/puts around at the money (option management)
 *   select combo and trade/watch the options watched from open (position/portfolio)
 *   track p/l over the day (portfolio)
 *   exit at day end, or some minimum profit level with trailing parabolic stop
 *   build up the volatility indicators mentioned in the boot
 *   live chart for each symbol and associated combo
 *   save the values at session end
 *
 *  implied volatility surface: pg 84
 *  historical volatility calcs:  pg 91
 *  c-c, c-o, o-c hist. vol. calcs: pg 94
 *  calcs by weekday: pg 99
 *  strangles better than straddles: pg 108
 *
 * then chapter 4 working with price spike charts
 *
 * 20151112 - important steps:
 *  historical volatility
 *  track stocks at 20:00 'this all has a familiar ring to it'
 *  underlying and three strikes at atm.  trade them on opening
 *  track profit over day and exit same day or next day?
 *   es, gc,
 *  hard code for now, build up infrastructure, and automate over time
 *  manually pick the symbols,
 *  run a process for each and watch
 *  save at end of day
 *  wait for flag to exit.
 *
 * 20151113 most important thing:
 *  track atm for a series of instruments
 *  keep a series of trades prepared
 *  show the intraday price spike charts
 *
 */

namespace {
  static const std::string c_sDisplayName_App( "Combo Trading" );
  static const std::string c_sDisplayName_Vendor( "One Unified Net Limited" );
  static const std::string c_sFileName_Base( "ComboTrading" );
  //static const std::string c_sDirectory_Base( "../" );
  static const std::string c_sFileName_SymbolSubset( c_sFileName_Base + ".ser" );
  static const std::string c_sFileName_DataBase( c_sFileName_Base + ".db" );
  static const std::string c_sFileName_State( c_sFileName_Base + ".state" );
  static const std::string c_sFileName_Series( "/app/" + c_sFileName_Base );
}

IMPLEMENT_APP(AppComboTrading)

const std::string sFileNameMarketSymbolSubset( c_sFileName_SymbolSubset );

bool AppComboTrading::OnInit() {

  wxApp::OnInit();
  wxApp::SetAppDisplayName( c_sDisplayName_App );
  wxApp::SetVendorName( c_sDisplayName_Vendor );
  wxApp::SetVendorDisplayName( c_sDisplayName_Vendor );

  //bool bExit = GetExitOnFrameDelete();
  //SetExitOnFrameDelete( true );

  m_pFPP = nullptr;
  m_pFOC = nullptr;

  m_pFPPOE = nullptr;
  m_pMPPOE = nullptr;
  m_pPPPOE = nullptr;
  m_pCPPOE = nullptr;

  m_sizerPM = nullptr;
  m_scrollPM = nullptr;
  m_sizerScrollPM = nullptr;

  m_pLastPPP = nullptr;

  m_pFCharts = nullptr;
  m_pFInteractiveBrokers = nullptr;

  m_pPanelCharts = nullptr;
  //m_pPanelOptionCombo = nullptr;

  m_pOptionEngine = std::make_unique<ou::tf::option::Engine>( m_fedrate );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sDisplayName_App, wxDefaultPosition, wxSize( 800, 1000 ) );
  m_pFrameMain->SetName( "primary" );
  //std::cout << "frame main: primary" << std::endl;
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, [this](const wxMoveEvent& event){
  //  wxPoint point = event.GetPosition();
  //  std::cout << "FrameMain position: " << point.x << "," << point.y << std::endl;
  //}, idFrameMain );
  //m_pFrameMain->Center();
  //m_pFrameMain->Move( 100, 500 );
  //m_pFrameMain->SetSize( 800, 1000 );
  SetTopWindow( m_pFrameMain );
  //wxPoint posFrameMain = m_pFrameMain->GetPosition();
  //std::cout << "Frame Ppsition: " << posFrameMain.x << "," << posFrameMain.y << std::endl;

  wxBoxSizer* psizerMain;
  psizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(psizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  psizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  m_tws->SetClientId( 3 );

  LinkToPanelProviderControl();
/*
  m_pPanelManualOrder = new ou::tf::PanelManualOrder( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelManualOrder, 0, wxEXPAND|wxRIGHT, 5);
  m_pPanelManualOrder->Enable( false );  // portfolio isn't working properly with manual order instrument field
  //m_pPanelManualOrder->Enable( true );  // portfolio isn't working properly with manual order instrument field
  m_pPanelManualOrder->Show( true );

  m_pPanelManualOrder->SetOnNewOrderHandler( MakeDelegate( this, &AppComboTrading::HandlePanelNewOrder ) );
  m_pPanelManualOrder->SetOnSymbolTextUpdated( MakeDelegate( this, &AppComboTrading::HandlePanelSymbolText ) );
  m_pPanelManualOrder->SetOnFocusPropogate( MakeDelegate( this, &AppComboTrading::HandlePanelFocusPropogate ) );
*/
/*
  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );
*/

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  psizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  //m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

//  m_idPortfolio = boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() ) + "StickShift";
  m_idPortfolioMaster = "master";  // keeps name constant over multiple days

  //m_bData1Connected = false;
  //m_bExecConnected = false;

  m_dblMinPL = m_dblMaxPL = 0.0;

  m_pIQFeedSymbolListOps = new ou::tf::IQFeedSymbolListOps( m_listIQFeedSymbols );
  m_pIQFeedSymbolListOps->Status.connect( [this]( const std::string sStatus ){
    CallAfter( [sStatus](){
      std::cout << sStatus << std::endl;
    });
  });

  FrameMain::vpItems_t vItemsSymbols;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItemsSymbols.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListRemote ) ) );
  vItemsSymbols.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListLocal ) ) );
  vItemsSymbols.push_back( new mi( "a3 Load Symbol List", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::LoadIQFeedSymbolList ) ) );
  vItemsSymbols.push_back( new mi( "a4 Save Symbol Subset", MakeDelegate( this, &AppComboTrading::HandleMenuActionSaveSymbolSubset ) ) );
  vItemsSymbols.push_back( new mi( "a5 Load Symbol Subset", MakeDelegate( this, &AppComboTrading::HandleMenuActionLoadSymbolSubset ) ) );
  wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Symbol List", vItemsSymbols );

  FrameMain::vpItems_t vItemsActions;
  vItemsActions.push_back( new mi( "Federal Funds Yield Curve", MakeDelegate( this, &AppComboTrading::HandleMenuActionEmitYieldCurve ) ) );
  //vItems.push_back( new mi( "load weeklies", MakeDelegate( &m_process, &Process::LoadWeeklies ) ) );
  vItemsActions.push_back( new mi( "Save Series", MakeDelegate( this, &AppComboTrading::HandleMenuActionSaveSeries ) ) );
  //vItems.push_back( new mi( "Load DataBase", MakeDelegate( this, &AppComboTrading::HandleLoadDatabase ) ) );
  wxMenu* pMenuActions = m_pFrameMain->AddDynamicMenu( "Actions", vItemsActions );

  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &AppComboTrading::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppComboTrading::OnClose, this );  // start close of windows and controls

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();

  BuildFrameCharts();
  BuildFrameInteractiveBrokers();
  BuildFramePortfolioPosition(); // needed by the database loader
  BuildFrameOptionCombo(); // this one is having problems

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.OnPortfolioLoaded.Add( MakeDelegate( this, &AppComboTrading::HandlePortfolioLoad ) );
  pm.OnPositionLoaded.Add( MakeDelegate( this, &AppComboTrading::HandlePositionLoad ) );

  if ( true ) {
    m_pFPPOE = new FrameMain( m_pFrameMain, wxID_ANY, "Portfolio/Position/Order/Execution" );
    m_pFPPOE->SetName( "PPOE" );
    //std::cout << "frame main: ppoe" << std::endl;

    m_pMPPOE = new MPPOE_t;

    m_pPPPOE = new PPPOE_t( m_pMPPOE, m_pFPPOE );
    m_pPPPOE->Show();

    m_pCPPOE = new CPPOE_t( m_pMPPOE, m_pPPPOE );
    //m_pCPPOE->LoadInitialData();

    m_pFPPOE->Show();
  }

  try {
    if ( boost::filesystem::exists( c_sFileName_DataBase ) ) {
  //    boost::filesystem::remove( sDbName );
    }

    m_db.OnRegisterTables.Add( MakeDelegate( this, &AppComboTrading::HandleRegisterTables ) );
    m_db.OnRegisterRows.Add( MakeDelegate( this, &AppComboTrading::HandleRegisterRows ) );
    m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppComboTrading::HandlePopulateDatabase ) );
    m_db.SetOnLoadDatabaseHandler( MakeDelegate( this, &AppComboTrading::HandleLoadDatabase ) );

    m_db.Open( c_sFileName_DataBase );
  }
  catch(...) {
    std::cout << "database fault on " << c_sFileName_DataBase << std::endl;
  }

  LoadState();

  m_pFrameMain->Show();

  return 1;
}

//int AppComboTrading::OnExit() {

  // OnExit is called after destroying all application windows and controls,
  // but before wxWidgets cleanup.

  //return wxApp::OnExit();
//}

AppComboTrading::pWatch_t AppComboTrading::BuildWatch( pInstrument_t pInstrument ) {
  pWatch_t pWatch;
  const std::string& sInstrumentName( pInstrument->GetInstrumentName() );
  mapWatch_t::iterator iterWatch = m_mapWatch.find( sInstrumentName );
  if ( m_mapWatch.end() == iterWatch ) {
    pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pData1Provider );
    m_mapWatch.emplace( sInstrumentName, pWatch );
  }
  else {
    pWatch = iterWatch->second;
  }
  return pWatch;
}

AppComboTrading::pOption_t AppComboTrading::BuildOption( pInstrument_t pInstrument ) {
  pOption_t pOption;
  const std::string& sInstrumentName( pInstrument->GetInstrumentName() );
  mapOption_t::iterator iterOption = m_mapOption.find( sInstrumentName );
  if ( m_mapOption.end() == iterOption ) {
    pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_pData1Provider );
    m_mapOption.emplace( sInstrumentName, pOption );
  }
  else {
    pOption = iterOption->second;
  }
  return pOption;
}

void AppComboTrading::ProvideOptionList( const std::string& sSymbol, ou::tf::PanelCharts::fSymbol_t function ) {
  m_listIQFeedSymbols.SelectOptionsByUnderlying( sSymbol, function );
}

void AppComboTrading::BuildFrameInteractiveBrokers( void ) {

  m_pFInteractiveBrokers = new FrameMain( m_pFrameMain, wxID_ANY, "Interactive Brokers", wxDefaultPosition, wxSize( 900, 500 ),
    wxCAPTION|wxRESIZE_BORDER
    );
  m_pFInteractiveBrokers->SetName( "IB" );
  //std::cout << "frame main: ib" << std::endl;

  FrameMain* itemFrame1 = m_pFInteractiveBrokers;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer2);

    m_splitPanels = new wxSplitterWindow( itemFrame1, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxSP_LIVE_UPDATE );
    m_splitPanels->SetMinimumPaneSize(20);

    m_pPanelIBAccountValues = new ou::tf::GridIBAccountValues( m_splitPanels, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelIBAccountValues->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_pPanelIBPositionDetails = new ou::tf::GridIBPositionDetails( m_splitPanels, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_pPanelIBPositionDetails->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    m_splitPanels->SplitVertically(m_pPanelIBAccountValues, m_pPanelIBPositionDetails, 50);
    itemBoxSizer2->Add(m_splitPanels, 1, wxGROW|wxALL, 2);

  if ( ou::tf::keytypes::EProviderIB == m_pExecutionProvider->ID() ) {
    ou::tf::ib::TWS::pProvider_t pProviderIB = ou::tf::ib::TWS::Cast( m_pExecutionProvider );
    pProviderIB->OnPositionDetailHandler = MakeDelegate( m_pPanelIBPositionDetails, &ou::tf::GridIBPositionDetails::UpdatePositionDetailRow );
    pProviderIB->OnAccountValueHandler = MakeDelegate( m_pPanelIBAccountValues, &ou::tf::GridIBAccountValues::UpdateAccountValueRow );
  }

  m_pFInteractiveBrokers->SetAutoLayout( true );
  m_pFInteractiveBrokers->Layout();
  m_pFInteractiveBrokers->Show();
}

void AppComboTrading::BuildFrameCharts( void ) {

  m_pFCharts = new FrameMain( m_pFrameMain, wxID_ANY, "Instrument Management", wxDefaultPosition, wxSize( 900, 500 ),
//    wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
        wxCAPTION|wxRESIZE_BORDER
    );
  m_pFCharts->SetName( "charts" );
  //std::cout << "frame main: charts" << std::endl;

  wxBoxSizer* psizer;
  psizer = new wxBoxSizer(wxVERTICAL);
  m_pFCharts->SetSizer( psizer );

  m_pPanelCharts = new ou::tf::PanelCharts( m_pFCharts, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
  psizer->Add(m_pPanelCharts, 1, wxGROW|wxALL, 2);

  namespace args = boost::phoenix::placeholders;

  m_pPanelCharts->signalRegisterInstrument.connect( boost::phoenix::bind( &AppComboTrading::RegisterInstrument, this, args::arg1 ) );  // use of this signal needs some cleaning
  m_pPanelCharts->signalLoadInstrument.connect( boost::phoenix::bind( &AppComboTrading::LoadInstrument, this, args::arg1 ) );
  m_pPanelCharts->signalRetrieveOptionList.connect( boost::phoenix::bind( &AppComboTrading::ProvideOptionList, this, args::arg1, args::arg2 ) );

  //m_pPanelCharts->SetProviders( m_pData1Provider, m_pData2Provider, m_pExecutionProvider );

  m_pPanelCharts->m_fSelectInstrument =
    [this](const ou::tf::Allowed::EInstrument selector, const wxString& sUnderlying)->pInstrument_t {
      std::shared_ptr<ou::tf::IQFeedInstrumentBuild> pBuild;
      pBuild.reset( new ou::tf::IQFeedInstrumentBuild( m_pPanelCharts ) );

      namespace ph = std::placeholders;
      pBuild->fLookupIQFeedDescription = std::bind( &AppComboTrading::LookupDescription, this, ph::_1, ph::_2 );

      pBuild->fBuildInstrument = [this,pBuild](ou::tf::IQFeedInstrumentBuild::ValuesForBuildInstrument& values){
        BuildInstrument( values, [pBuild](pInstrument_t pInstrument){
          pBuild->InstrumentUpdated( pInstrument ); // this is the call back into the gui when contract id has been populated
        });
      };
      return pBuild->HandleNewInstrumentRequest( selector, sUnderlying );
    };

  m_pPanelCharts->m_fBuildInstrumentFromIqfeed =
    [this](const std::string& sName)->pInstrument_t {
        ou::tf::iqfeed::InMemoryMktSymbolList::trd_t trd( m_listIQFeedSymbols.GetTrd( sName ) );
        return ou::tf::iqfeed::BuildInstrument( sName, trd );
    };

  // build the callback for when PanelCharts/GridOptionChain_impl needs to build a full instrument
  // for result of drag and drop operations
  m_pPanelCharts->m_fBuildOptionInstrument =
    // first lambda builds an instrument given IQFeed option name and parameters
    // can this use the code from m_fBuildInstrumentFromIqfeed above?
    [this](pInstrument_t pUnderlyingInstrument, const std::string& sIQFeedOptionName, boost::gregorian::date date, double strike, ou::tf::PanelCharts::fBuildOptionInstrumentComplete_t f){
      ou::tf::iqfeed::InMemoryMktSymbolList::trd_t trd( m_listIQFeedSymbols.GetTrd( sIQFeedOptionName ) ); // TODO: check for errors
      std::string sGenericOptionName = ou::tf::Instrument::BuildGenericOptionName( pUnderlyingInstrument->GetInstrumentName(), date.year(), date.month(), date.day(), trd.eOptionSide, strike );

      ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
      pInstrument_t pOptionInstrument;
      if ( im.Exists( sGenericOptionName, pOptionInstrument ) ) {
        if ( nullptr != f ) {
          f( pOptionInstrument );
        }
      }
      else {
        pOptionInstrument = ou::tf::iqfeed::BuildInstrument( sGenericOptionName, trd, date );
        ou::tf::ib::TWS::Contract contract;
        contract.conId = pUnderlyingInstrument->GetContract();
        // this request uses contract id to obtain basic symbol of the underlying
        // TODO: 2018/09/08 this double lookup shouldn't be required, the underlying IB name should be already be stored as an alternate name
        m_tws->RequestContractDetails(
          contract,
          [this, pOptionInstrument, pUnderlyingInstrument, f](const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument){
            // the resulting symbol can then be used to plug into the final option request to obtain
            //   the contract details for the option
            pInstrument_t pOptionInstrument_ = pOptionInstrument; // bypasses a const problem through the capture
            m_tws->RequestContractDetails(
              details.contract.symbol, pOptionInstrument_,
              [this, pUnderlyingInstrument, f](const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument){
                // the contract details fill in the contract in the instrument, which can then be passed back to the caller
                //   as a fully defined, registered instrument
                RegisterInstrument( pInstrument );  // TODO: does the instrument need to be registered?  Is this the best place for this?
                if ( nullptr != f ) {
                  f( pInstrument );
                }
              },
              nullptr  // request complete
              );
          },
          nullptr, // request complete
          pUnderlyingInstrument );
      }
    };

  m_pPanelCharts->m_fCalcOptionGreek_Add = [this]( pOption_t pOption, pWatch_t pWatchUnderlying ){
    m_pOptionEngine->Add( pOption, pWatchUnderlying );
  };

  m_pPanelCharts->m_fCalcOptionGreek_Remove = [this]( pOption_t pOption, pWatch_t pWatchUnderlying ){
    m_pOptionEngine->Remove( pOption, pWatchUnderlying );
  };
  m_pPanelCharts->m_fBuildOption = [this](pInstrument_t pInstrument, pOption_t& pOption){
    return BuildOption( pInstrument );
  };
  m_pPanelCharts->m_fBuildWatch = [this](pInstrument_t pInstrument, pWatch_t& pWatch){
    return BuildWatch( pInstrument );
  };

  int ixItem;
  // prepended in reverse order
  ixItem = m_pFrameMain->AddFileMenuItem( _( "Load Config" ) );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &AppComboTrading::HandleLoad, this, ixItem, -1 );

  ixItem = m_pFrameMain->AddFileMenuItem( _( "Save Config" ) );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &AppComboTrading::HandleSave, this, ixItem, -1 );

  m_pFCharts->SetAutoLayout( true );
  m_pFCharts->Layout();
  m_pFCharts->Show();

}

AppComboTrading::pInstrument_t AppComboTrading::LoadInstrument( const std::string& name ) {
  //std::cout << "AppComboTrading::LoadInstrument: " << name << std::endl;
  pInstrument_t p;
  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
  if ( !im.Exists( name, p ) ) {  // the call will supply instrument if it exists
    throw std::runtime_error( "instrument does not exist" );
  }
  return p;
}

// todo:
//   map of instruments prior to contract
//   map of instruments with contract
//   map of instruments from instrument manager

void AppComboTrading::BuildInstrument( ou::tf::IQFeedInstrumentBuild::ValuesForBuildInstrument& values, fInstrumentFromIB_t callback ) {
  std::cout << "AppComboTrading::BuildInstrument: " << values.sKey << " ";
  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
  if ( im.Exists( values.sKey, values.pInstrument ) ) {  // the call will supply instrument if it exists
    std::cout << "exists ..." << std::endl;
    callback( values.pInstrument );
  }
  else {  // build
    std::cout << "build ..." << std::endl;
    if ( 0 == m_listIQFeedSymbols.Size() ) {
      std::cout << "AppComboTrading::BuildInstrument: m_listIQFeedSymbols not loaded" << std::endl;
    }
    else {
      typedef ou::tf::iqfeed::InMemoryMktSymbolList list_t;
      typedef list_t::trd_t trd_t;
      const trd_t& trd( m_listIQFeedSymbols.GetTrd( values.sIQF ) );
      switch ( trd.sc ) {
        case ou::tf::iqfeed::ESecurityType::Equity:
        case ou::tf::iqfeed::ESecurityType::IEOption:
          values.pInstrument = ou::tf::iqfeed::BuildInstrument( values.sKey, trd );
          break;
        case ou::tf::iqfeed::ESecurityType::Future:
        case ou::tf::iqfeed::ESecurityType::FOption:
          // TODO: will need to change this to match what is in BasketTrading for creation from fundamentals
          values.pInstrument = ou::tf::iqfeed::BuildInstrument( values.sKey, trd, boost::gregorian::date( trd.nYear, trd.nMonth, values.day ) );  // for ib future/fo overrides not supplied by trd
          break;
        default:
          throw std::runtime_error( "ppComboTrading::BuildInstrument: can't process the default" );
      }
      GetContractFor( values.sIB, values.pInstrument, callback );
    }
  }
}

void AppComboTrading::BuildFramePortfolioPosition( void ) {

  m_pFPP = new FrameMain( m_pFrameMain, wxID_ANY, "Portfolio Management", wxDefaultPosition, wxSize( 900, 500 ),
//    wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
        wxCAPTION|wxRESIZE_BORDER
    );
  m_pFPP->SetName( "portfolio" );
  //std::cout << "frame main: portfolio" << std::endl;

  m_pFPP->SetAutoLayout( true );

  m_sizerPM = new wxBoxSizer(wxVERTICAL);
  m_pFPP->SetSizer(m_sizerPM);

  //m_scrollPM is used for holding the PanelPortfolioPosition instances
  m_scrollPM = new wxScrolledWindow( m_pFPP, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
  m_sizerPM->Add(m_scrollPM, 1, wxGROW|wxALL, 5);
  //m_scrollPM->SetScrollbars(1, 1, 0, 0);
  m_scrollPM->SetScrollRate(4, 4);

  m_sizerScrollPM = new wxBoxSizer(wxVERTICAL);
  m_scrollPM->SetSizer( m_sizerScrollPM );

  m_pFPP->Layout();
  m_pFPP->Show();


}

void AppComboTrading::BuildFrameOptionCombo() {

  m_pFOC = new FrameMain( m_pFrameMain, wxID_ANY, "Option Combo Sandbox", wxDefaultPosition, wxSize( 900, 500 ),
//    wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
        wxCAPTION|wxRESIZE_BORDER
    );
  m_pFOC->SetName( "sandbox" );
  //std::cout << "frame main: sandbox" << std::endl;

  m_sizerFOC = new wxBoxSizer( wxVERTICAL );
  m_pFOC->SetSizer( m_sizerFOC );

  //m_scrollOC is used for holding the pPanelOptionCombo instances
  m_scrollFOC = new wxScrolledWindow( m_pFOC, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
  m_sizerFOC->Add(m_scrollFOC, 1, wxGROW|wxALL, 4);
  //m_scrollFOC->SetScrollbars(5, 5, 10, 10);
  m_scrollFOC->SetScrollRate(4, 4);

  m_sizerScrollOC = new wxBoxSizer( wxVERTICAL );
  m_scrollFOC->SetSizer( m_sizerScrollOC );

  m_pFOC->SetAutoLayout( true );
  m_pFOC->Layout();
  m_pFOC->Show();

  HandleNewPanelOptionCombo( idPortfolio_t( "sandbox" ), "experimenting with option combinations" );
}

AppComboTrading::pPanelOptionCombo_t AppComboTrading::HandleNewPanelOptionCombo( const idPortfolio_t& idPortfolio, const std::string& sDescription ) {

  typedef ou::tf::PortfolioGreek::pPortfolioGreek_t pPortfolioGreek_t;
  typedef ou::tf::PositionGreek::pPositionGreek_t pPositionGreek_t;

  pPanelOptionCombo_t pPanelOptionCombo;
  pPanelOptionCombo = new ou::tf::PanelOptionCombo( m_scrollFOC );  // start with one empty portfolio

  pPanelOptionCombo->m_fBootStrapNextPanelOptionCombo = [this](const idPortfolio_t& idPortfolio_, const std::string& sDescription_){
    HandleNewPanelOptionCombo( idPortfolio_, sDescription_ );
  };
  pPanelOptionCombo->m_fConstructPortfolioGreek = [this](ou::tf::PanelOptionCombo& poc, const idPortfolio_t& idPortfolio, const std::string& sDescription){
    if ( idPortfolio.empty() ) {
      std::cout << "portfolio id is required" << std::endl;
    }
    else {
      mapPortfoliosSandbox_t::iterator iter = m_mapPortfoliosSandbox.find( idPortfolio );
      if ( m_mapPortfoliosSandbox.end() == iter ) {
        pPortfolioGreek_t pPortfolioGreek( new ou::tf::PortfolioGreek(
          idPortfolio, ou::tf::PortfolioGreek::idAccountOwner_t( "none" ), idPortfolio_t( "self" ),
          ou::tf::Portfolio::EPortfolioType::Standard, "USD", sDescription
        ) );
        poc.SetPortfolioGreek( pPortfolioGreek );

        m_mapPortfoliosSandbox.insert( mapPortfoliosSandbox_t::value_type( pPortfolioGreek->Id(), structPortfolioSandbox( &poc ) ) );
      }
      else {
        std::cout << "portfolio id " << idPortfolio << " already exists" << std::endl;
      }
    }
    m_pFOC->Layout();
    //m_sizerScrollOC->Layout();
  };

  pPanelOptionCombo->m_fConstructPositionGreek
      = [this](pInstrument_t pOptionInstrument, pInstrument_t pUnderlyingInstrument, pPortfolioGreek_t pPortfolioGreek, ou::tf::PanelOptionCombo::fAddPositionGreek_t f) {
        // convert OptionInstrument to option_t, convert UnderlyingInstrument to watch_t
        // register with engine
    ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
    if ( im.Exists( pOptionInstrument ) ) {
      // is typically built at the m_pPanelCharts->m_fBuildOptionInstrument point in time
      //std::cout << "### verify the code at m_fConstructPositionGreek as instrument exists: " << pInstrument->GetInstrumentName() << " ###" << std::endl;
    }
    else {
      std::cout << "AppComboTrading::BuildFrameOptionCombo: registering " << pOptionInstrument->GetInstrumentName() << ", should have been accomplished prior to this?" << std::endl;
      im.Register( pOptionInstrument );
    }

    assert( 0 != pUnderlyingInstrument.use_count() );
    assert( 0 != pUnderlyingInstrument.get() );

    pWatch_t pWatch = BuildWatch( pUnderlyingInstrument );
    assert( 0 != pWatch.use_count() );
    assert( 0 != pWatch.get() );

    assert( 0 != pOptionInstrument.use_count() );
    assert( 0 != pOptionInstrument.get() );

    pOption_t pOption = BuildOption( pOptionInstrument );
    assert( 0 != pOption.use_count() );
    assert( 0 != pOption.get() );

    pPositionGreek_t pPositionGreek( new ou::tf::PositionGreek( pOption, pWatch ) );
    pPortfolioGreek->AddPosition( pOptionInstrument->GetInstrumentName(), pPositionGreek );
    if ( nullptr != f )
      f( pPositionGreek );

    // position needs to start the watch, needs both option and underlying
    // then add to engine here?  who removes from engine?  does the panel manage the list
  };
  pPanelOptionCombo->m_fSelectInstrument = [this]()->pInstrument_t {
      namespace ph = std::placeholders;
      std::shared_ptr<ou::tf::IQFeedInstrumentBuild> pBuild;
      pBuild.reset( new ou::tf::IQFeedInstrumentBuild( m_pLastPPP ) );
      pBuild->fLookupIQFeedDescription = std::bind( &AppComboTrading::LookupDescription, this, ph::_1, ph::_2 );
      pBuild->fBuildInstrument = [this,pBuild](ou::tf::IQFeedInstrumentBuild::ValuesForBuildInstrument& values){
        BuildInstrument( values, [pBuild](pInstrument_t pInstrument){
          pBuild->InstrumentUpdated( pInstrument ); // this is the call back into the gui when contract id has been populated
        });
      };
      return pBuild->HandleNewInstrumentRequest( ou::tf::Allowed::All, "" ); // the dialog pops up in this call
    };
  pPanelOptionCombo->m_fColumnWidthChanged = [this](int nColumn, int width, ou::tf::PanelOptionCombo& poc){
    poc.SaveColumnSizes( m_gcsPanelOptionCombo );
    UpdateColumns_PanelOptionCombo();
  };

  namespace ph = std::placeholders;
  pPanelOptionCombo->m_fRegisterWithEngine = std::bind( &ou::tf::option::Engine::Add, m_pOptionEngine.get(), ph::_1, ph::_2 );
  pPanelOptionCombo->m_fRemoveFromEngine = std::bind( &ou::tf::option::Engine::Remove, m_pOptionEngine.get(), ph::_1, ph::_2 );

  pPanelOptionCombo->m_fLookUpInstrument = [this](const idInstrument_t& idInstrument, pInstrument_t& pInstrument)->pInstrument_t {
    ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
    if ( im.Exists( idInstrument, pInstrument ) ) {
      // all is good, pInstrument is assigned
    }
    else {
      std::string message( "pPOC->m_fLookUpInstrument can't find " );
      message += idInstrument;
      throw std::runtime_error( message );
    }
    return pInstrument;
  };

  pPanelOptionCombo->m_fConstructPortfolioGreek( *pPanelOptionCombo, idPortfolio, sDescription );

  //wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
  //m_sizerScrollOC->Add( pSizer, 1, wxGROW|wxALL, 1 );
  //pSizer->Add( pPanelOptionCombo, 1, wxALL|wxEXPAND, 1);
  //m_sizerScrollOC->Add( pPanelOptionCombo, 0, wxALL|wxGROW, 6 );
  pPanelOptionCombo->AssignToSizer( m_sizerScrollOC );
  m_sizerScrollOC->Layout();

  return pPanelOptionCombo;
}

// 20151124 priority:  get the symbols tracking, draw charts, and watch volatility on futures and equities
// 20151128 use the code in start to obtain the symbol info for populating the portfolio/position info

void AppComboTrading::TestSymbols( void ) {
  // need iqfeed marketsymbols to be loaded before this can start
  if ( 0 == m_listIQFeedSymbols.Size() )
    throw std::runtime_error( "iqfeed market symbols need to be loaded first" );
//  if ( !m_bStarted ) {
  if ( true ) {
    try {
      // new stuff

      // the cycle being:
      //  build the iqfeed instrument
      //  send it off to ib to get the contract
      //  then get it into multi-expiry
      //  then start building the options
      // calculate expiry and flesh out bundle
      // but also may be working with portfolio/position situations

      struct Symbol {
        std::string sName;
        std::string sIq;
        std::string sIb;
        Symbol( const std::string& sName_ ): sName( sName_ ), sIq( sName_ ), sIb( sName_ ) {};
        Symbol( const std::string& sName_, const std::string& sIq_, const std::string& sIb_ ):
        sName( sName_ ), sIq( sIq_ ), sIb( sIb_ ) {};
      };

      typedef std::vector<Symbol> vSymbol_t;
      vSymbol_t vSymbol;

      // list of equities to monitor
      vSymbol.push_back( Symbol( "GLD" ) );

      // create generic symbol: futures and equities, and possibly from weeklies
      struct Future {
        std::string sName;
        std::string sIqBaseName;
        boost::uint16_t nYear;
        boost::uint8_t nMonth;
        std::string sIbBaseName;
        Future(
          const std::string& sName_, const std::string& sIqBaseName_,
          boost::uint16_t nYear_, boost::uint8_t nMonth_,
          const std::string& sIbBaseName_ ):
          sName( sName_ ), sIqBaseName( sIqBaseName_ ),
          nYear( nYear_ ), nMonth( nMonth_ ),
          sIbBaseName( sIbBaseName_ )
        {}
      };

      typedef std::vector<Future> vFuture_t;
      vFuture_t vFuture;

      // list of futures to monitor
      vFuture.push_back( Future( "GC-2015-12", "QGC", 2015, 12, "GC" ) );

      // build futures name and add to vSymbol
      // ie, create the iqfeed name for the future, then pass off the name for instrument building
      for ( vFuture_t::const_iterator iter = vFuture.begin(); vFuture.end() != iter; ++iter ) {
        std::string sName = ou::tf::iqfeed::BuildFuturesName( iter->sIqBaseName, iter->nYear, iter->nMonth );
        vSymbol.push_back( Symbol( iter->sName, sName, iter->sIbBaseName ) );
      }

      // 20151122 build list of options to monitor as well
      // then can mix and match into positions and portfolios
      // which implies ...
      //   need some structures here to assign the code seeded/generated portfolio and position combinations
      // then can generify stuff and get into a separate compilation unit

      struct Option {
        std::string sName;
        std::string sIqBaseName;
        ou::tf::OptionSide::EOptionSide side;
        double dblStrike;
        boost::uint16_t nYear;
        boost::uint8_t nMonth;
        boost::uint8_t nDay;
        std::string sIbBaseName;
        Option( const std::string& sName_, ou::tf::OptionSide::EOptionSide side_,
          const std::string& sIqBaseName_, const std::string& sIbBaseName_,
          boost::uint16_t nYear_, boost::uint8_t nMonth_, boost::uint8_t nDay_, double dblStrike_ ):
            sName( sName_ ), side( side_ ),
            sIqBaseName( sIqBaseName_ ), sIbBaseName( sIbBaseName_ ),
            nYear( nYear_ ), nMonth( nMonth_ ), nDay( nDay_ ), dblStrike( dblStrike ) {}
      };

      typedef std::vector<Option> vOption_t;
      vOption_t vOption;

      vOption.push_back( Option( "GLD-P-2015-12-11-112.0", ou::tf::OptionSide::Call, "GLD", "GLD", 2015, 12, 11, 112.0 ) );

      for ( vOption_t::const_iterator iter = vOption.begin(); vOption.end() != iter; ++iter ) {
        std::string sName =
          ou::tf::iqfeed::BuildOptionName( iter->sIqBaseName, iter->nYear, iter->nMonth, iter->nDay, iter->dblStrike, iter->side );
        vSymbol.push_back( Symbol( iter->sName, sName, iter->sIbBaseName ) );
      }

      // 20151122 at this point, we have our own instrument names, the names to be used for
      //   registering, so at this point, we can perform a retrieval, or build new ones.
      //   some may need some building, some may need retrieval

      // 2015/11/22 take this code and put into module as it will be re-used not only in startup
      //    but in other real time locations

      {
        namespace args = boost::phoenix::placeholders;
        typedef boost::function<void ( pInstrument_t)> cbInstrument_t;

        // function object for building instruments from a list
        struct BuildInstrument {
          typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
          typedef ou::tf::iqfeed::InMemoryMktSymbolList list_t;
          typedef list_t::trd_t trd_t;
          const list_t& list;
          cbInstrument_t f;
          BuildInstrument( const list_t& list_, cbInstrument_t f_ ): list( list_ ), f( f_ ) {};
          void operator()( const Symbol& u ) {
            pInstrument_t pInstrument;
            const trd_t& trd( list.GetTrd( u.sIq ) );
            switch ( trd.sc ) {
              case ou::tf::iqfeed::ESecurityType::Equity:
              case ou::tf::iqfeed::ESecurityType::Future:
                pInstrument = ou::tf::iqfeed::BuildInstrument( u.sName, trd );
                // now hand it off to the IB for contract insertion
                f( pInstrument );
                break;
              case ou::tf::iqfeed::ESecurityType::IEOption:
                // will need to check that Symbol is registered and available in order to build
                throw std::runtime_error( "can't process the BuildInstrument IEOption" );
                break;
              case ou::tf::iqfeed::ESecurityType::FOption:
                throw std::runtime_error( "can't process the BuildInstrument FOption" );
                break;
              default:
                throw std::runtime_error( "can't process the BuildInstrument default" );
            }
          }
        };

            // build instruments.. equities and futures, then pass to the bundle to fill in the options
            // in GetContractFor, need to see if the instrument already exists, or is this done up above somewhere?

        struct GetInstrument {
          typedef ou::tf::iqfeed::InMemoryMktSymbolList list_t;
          BuildInstrument build;
          cbInstrument_t cbBundle;
          GetInstrument( const list_t& list_, cbInstrument_t cbContract, cbInstrument_t cbBundle_ ): build( list_, cbContract ), cbBundle( cbBundle_ ) {}
          void operator()( const Symbol& u ) {
            // see if the instrument already exists,
            ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
            ou::tf::Instrument::pInstrument_t pInstrument;  //empty instrument
            if ( im.Exists( u.sName, pInstrument ) ) {  // the call will supply instrument if it exists
              cbBundle( pInstrument );
            }
            else {
              build( u );
            }
          }
        };

        // 20151227 broke this with IB symbol changes
//        std::for_each( vSymbol.begin(), vSymbol.end(),
//          GetInstrument( m_listIQFeedSymbols,
//            boost::phoenix::bind( &AppComboTrading::GetContractFor, this, args::arg1 ),
//            boost::phoenix::bind( &AppComboTrading::LoadUpBundle, this, args::arg1 )
//            )
//          );

      }

    }
    catch (...) {
      std::cout << "problems with AppStickShift::Start" << std::endl;
    }
  }
}

void AppComboTrading::GetContractFor( const std::string& sBaseName, pInstrument_t pInstrument, fInstrumentFromIB_t callback ) {
  if ( m_bIBConnected ) {
    m_tws->RequestContractDetails(
      sBaseName, pInstrument,
      [this, callback]( const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument ){
        CallAfter([callback, pInstrument](){ // put into GUI thread
          callback( pInstrument );
        });
      },
      []( bool ){}
      );
  }
  else {
    std::cout << "AppComboTrading::GetContractFor: IB Not Connected" << std::endl;
  }
}

// futures expire: 17:15 est
// foption expire: 13:30 est
// option expire:  16:00 est
// holiday expire: 13:00 est 2015/11/27 - weekly option

void AppComboTrading::RegisterInstrument( pInstrument_t pInstrument ) {
  //std::cout << "AppComboTrading::RegisterInstrument: " << pInstrument->GetInstrumentName() << std::endl;
  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
  if ( im.Exists( pInstrument ) ) {
    //std::cout << "Info: Instrument already registered: " << pInstrument->GetInstrumentName() << std::endl;
    // now there are duplicate instruments.  replace inbound with the one registered?
  }
  else {
    assert( 0 != pInstrument->GetContract() );
    im.Register( pInstrument );
    //std::cout << "Info: Instrument/Contract registered: " << pInstrument->GetInstrumentName() << std::endl;
  }
}

// this may need to steal code from ConstructEquityPosition0, and ConstructEquityPosition1
void AppComboTrading::LoadUpBundle( ou::tf::Instrument::pInstrument_t pInstrument ) {

  // 2015/11/22 need to fix this code up

  BundleTracking::BundleDetails details( "GC", "QGC" );
  // need to figure out proper expiry time
  // also need to figure if more options can be watched in order to provide volatility surfaces
  // I think I have a day calculator (which doesn't know holidays though )
  // and will need to convert to utc
  // if there are many symbols, strikes, and expiries, may need to run a different thread for calculating the greeks and IV
  boost::gregorian::date expiry2( boost::gregorian::date( 2015, 12, 28 ) );
  details.vOptionExpiryDay.push_back( expiry2 );
  boost::gregorian::date expiry3( boost::gregorian::date( 2016,  1, 26 ) );
  details.vOptionExpiryDay.push_back( expiry3 );

  m_vBundleTracking.push_back( BundleTracking( "GC" ) );  // can I do some sort of move semantics here?
  m_vBundleTracking.back().SetBundleParameters( details );

}

void AppComboTrading::UpdateColumns_PanelPortfolioPositions() {
  std::for_each( m_mapPortfoliosTrading.begin(), m_mapPortfoliosTrading.end(),
                  [this](mapPortfoliosTrading_t::value_type& vt){
                    vt.second.pT->SetColumnSizes( m_gcsPanelPortfolioPosition );
                  });
}

void AppComboTrading::UpdateColumns_PanelOptionCombo() {
  std::for_each( m_mapPortfoliosSandbox.begin(), m_mapPortfoliosSandbox.end(), [this](mapPortfoliosSandbox_t::value_type& vt){
    vt.second.pT->SetColumnSizes( m_gcsPanelOptionCombo );
  } );
}


void AppComboTrading::HandlePortfolioLoad( pPortfolio_t& pPortfolio ) {

  m_pLastPPP = new ou::tf::PanelPortfolioPosition( m_scrollPM );
  m_pLastPPP->SetColumnSizes( m_gcsPanelPortfolioPosition );
  m_pLastPPP->m_fColumnWidthChanged = [this](int nColumn, int width, ou::tf::PanelPortfolioPosition& ppp ){
    ppp.SaveColumnSizes( m_gcsPanelPortfolioPosition );
    UpdateColumns_PanelPortfolioPositions();
  };
  m_sizerScrollPM->Add( m_pLastPPP, 1, wxALL|wxEXPAND, 0);
  m_sizerScrollPM->Layout();

  std::cout
    << "Adding Portfolio: "
    << "T=" << pPortfolio->GetRow().ePortfolioType
    << ",O=" << pPortfolio->GetRow().idOwner
    << ",ID=" << pPortfolio->GetRow().idPortfolio
    << std::endl;

  namespace ph = std::placeholders;

  m_pLastPPP->SetPortfolio( pPortfolio );
  m_pLastPPP->m_fConstructPortfolio = std::bind( &AppComboTrading::HandleConstructPortfolio, this, ph::_1, ph::_2, ph::_3 );
  m_pLastPPP->m_fConstructPosition = std::bind( &AppComboTrading::ConstructEquityPosition1b, this, ph::_1, ph::_2, ph::_3 );  // something to be fixed here?
  m_pLastPPP->m_fSelectInstrument =
    [this]()->pInstrument_t {
      std::shared_ptr<ou::tf::IQFeedInstrumentBuild> pBuild;
      pBuild.reset( new ou::tf::IQFeedInstrumentBuild( m_pLastPPP ) );

      pBuild->fLookupIQFeedDescription = std::bind( &AppComboTrading::LookupDescription, this, ph::_1, ph::_2 );
      pBuild->fBuildInstrument = [this,pBuild](ou::tf::IQFeedInstrumentBuild::ValuesForBuildInstrument& values){
        BuildInstrument( values, [pBuild](pInstrument_t pInstrument){
          pBuild->InstrumentUpdated( pInstrument ); // this is the call back into the gui when contract id has been populated
        });
      };
      return pBuild->HandleNewInstrumentRequest( ou::tf::Allowed::All, "" ); // the dialog pops up in this call
    };
  m_mapPortfoliosTrading.insert( mapPortfoliosTrading_t::value_type( pPortfolio->Id(), structPortfolioTrading( m_pLastPPP ) ) );
}

void AppComboTrading::HandlePositionLoad( pPosition_t& pPosition ) {
  m_pLastPPP->AddPosition( pPosition );
}

void AppComboTrading::HandleGuiRefresh( wxTimerEvent& event ) {
  std::for_each( m_mapPortfoliosTrading.begin(), m_mapPortfoliosTrading.end(),
    [](mapPortfoliosTrading_t::value_type& vt){
      vt.second.pT->UpdateGui();
    });
  std::for_each( m_mapPortfoliosSandbox.begin(), m_mapPortfoliosSandbox.end(),
    [](mapPortfoliosSandbox_t::value_type& vt){
      vt.second.pT->UpdateGui();
    }
    );
}

void AppComboTrading::LookupDescription( const std::string& sSymbolName, std::string& sDescription ) {
  sDescription = "";
  if ( 0 != m_listIQFeedSymbols.Size() ) {
    try {
      const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd( m_listIQFeedSymbols.GetTrd( sSymbolName ) );
      sDescription = trd.sDescription;
      //std::cout << "AppComboTrading::LookupDescription: " << sSymbolName << "=" << sDescription << std::endl;
    }
    catch ( std::runtime_error& e ) {
      //std::cout << "AppComboTrading::LookupDescription error: " << e.what() <<std::endl;
    }
  }
}

// 20151122 Most of this is now obsolete.  Handled mostly in startup code.  Need to deal with option building though.
//  the line with  ConstructEquityPosition1( pInstrument );  remains to be refactored elsewhere
//   also ConstructEquityPosition1 needs to be fixed to match, as it has some callbacks set from here
// 20170820 this probably can be deprecated after PanelPortfolioPosition fixed up?
void AppComboTrading::ConstructEquityPosition1a( const std::string& sName, pPortfolio_t pPortfolio, fAddPostion_t function ) {

  m_EquityPositionCallbackInfo.pPortfolio = pPortfolio;
  m_EquityPositionCallbackInfo.fAddPosition = function;

  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
  ou::tf::Instrument::pInstrument_t pInstrument;  //empty instrument

  // 20151025
  // can pull equity, future, option out of iqfeed marketsymbols file
  // construct basic instrument based upon iqfeed info
  // then redo the following to obtain the contract info from ib and add to the instrument

  // 20151026
  // maybe get rid of the whole underlying instrument requirement in an option symbol (similar to fact that futures option doesn't have underlying tie in )
  // requires a bunch of rewrite, but probably worth it
  //  20151115: maybe not, it provides a mechanism for getting at underlying's alternate names

  // 20151115
  // need to build state builder to get contracts for a series of underlying symbols, then run through
  //  associated options, and obtain their ib contract numbers
  // and feed the results into the multiexpirybundle
  // then stuff into a library for re-use

  //

  // sName is going to be an IQFeed name from the MarketSymbols file, so needs to be in main as well as alternatesymbolsnames list
  if ( im.Exists( sName, pInstrument ) ) {  // the call will supply instrument if it exists
    ConstructEquityPosition2( pInstrument );
  }
  else {
    // this is going to have to be changed to reflect various symbols types recovered from the IQF Market Symbols file
    // which might be simplified if IB already has the code for interpreting a pInstrument_t

    bool bConstructed( false );
    try {
      const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd( m_listIQFeedSymbols.GetTrd( sName ) );

      switch ( trd.sc ) {
        case ou::tf::iqfeed::ESecurityType::Equity:
        case ou::tf::iqfeed::ESecurityType::Future:
        case ou::tf::iqfeed::ESecurityType::FOption:
      //	  pInstrument = ou::tf::iqfeed::BuildInstrument( trd );
      	  break;
        case ou::tf::iqfeed::ESecurityType::IEOption:
        {
          ou::tf::Instrument::pInstrument_t pInstrumentUnderlying;
          if ( im.Exists( trd.sUnderlying, pInstrumentUnderlying ) ) { // change called name to IfExistsSupplyInstrument
          }
          else {
            // otherwise build instrument
            const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trdUnderlying( m_listIQFeedSymbols.GetTrd( trd.sUnderlying ) );
            switch (trdUnderlying.sc ) {
              case ou::tf::iqfeed::ESecurityType::Equity:
      //		pInstrumentUnderlying = ou::tf::iqfeed::BuildInstrument( trdUnderlying );  // build the underlying in preparation for the option
          im.Register( pInstrumentUnderlying );
          // 20151115 this instrument should also obtain ib contract details, as it will land in multi-expiry bundle and be used in various option delta scenarios
          break;
              default:
          throw std::runtime_error( "ConstructEquityPosition0: no applicable instrument type for underlying" );
            }
          }
      //          pInstrument = ou::tf::iqfeed::BuildInstrument( trd, pInstrumentUnderlying );  // build an option
        }  // end case
          break;
        default:
          throw std::runtime_error( "ConstructEquityPosition0: no applicable instrument type" );
      } // end switch
      bConstructed = true;
    } // end try
    catch ( std::runtime_error& e ) {
      std::cout << "Couldn't find symbol: " + sName << std::endl;
    }
    if ( bConstructed ) {
      std::cout << "Requesting IB: " << sName << std::endl;
      // 20151227 need to fix this as needs different name sent
//      m_tws->RequestContractDetails(
//       	pInstrument,
//	      MakeDelegate( this, &AppComboTrading::HandleIBContractDetails ), MakeDelegate( this, &AppComboTrading::HandleIBContractDetailsDone ) );
    }
  }
}

void AppComboTrading::ConstructEquityPosition1b( pInstrument_t pInstrument, pPortfolio_t pPortfolio, fAddPostion_t function ) {

  m_EquityPositionCallbackInfo.pPortfolio = pPortfolio;
  m_EquityPositionCallbackInfo.fAddPosition = function;

  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );

  // sName is going to be an IQFeed name from the MarketSymbols file, so needs to be in main as well as alternatesymbolsnames list
  if ( im.Exists( pInstrument ) ) {
  }
  else {
    im.Register( pInstrument );
  }

  ConstructEquityPosition2( pInstrument );
}

// 20151025 problem with portfolio is 0 in m_EquityPositionCallbackInfo
void AppComboTrading::ConstructEquityPosition2( pInstrument_t& pInstrument ) {
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  try {
    pPosition_t pPosition( pm.ConstructPosition(
      m_EquityPositionCallbackInfo.pPortfolio->GetRow().idPortfolio,
      pInstrument->GetInstrumentName(),
      "",
      this->m_pExecutionProvider->GetName(),
      this->m_pData1Provider->GetName(),
      //"ib01", "iq01",
      this->m_pExecutionProvider, this->m_pData1Provider,
      pInstrument )
      );
    if ( 0 != m_EquityPositionCallbackInfo.fAddPosition ) {
      m_EquityPositionCallbackInfo.fAddPosition( pPosition );
    }
  }
  catch ( std::runtime_error& e ) {
    std::cout << "Position Construction Error:  " << e.what() << std::endl;
  }
}

void AppComboTrading::HandleConstructPortfolio( ou::tf::PanelPortfolioPosition& ppp, const std::string& sPortfolioId, const std::string& sDescription ) {
  // check if portfolio exists
  if ( sPortfolioId.empty() ) {
    std::cout << "no portfolio id supplied" << std::endl;
  }
  else {
    if ( ou::tf::PortfolioManager::GlobalInstance().PortfolioExists( sPortfolioId ) ) {
      std::cout << "PortfolioId " << sPortfolioId << " already exists." << std::endl;
    }
    else {
      ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
        sPortfolioId, "aoRay", ppp.GetPortfolio()->Id(),ou::tf::Portfolio::Standard, ppp.GetPortfolio()->GetRow().sCurrency, sDescription );
    }
  }
}

// maybe put this into background thread
void AppComboTrading::HandleMenuActionSaveSymbolSubset( void ) {

  m_vExchanges.clear();
  m_vExchanges.insert( "NYSE" );
  m_vExchanges.insert( "NYSE_AMERICAN" );
  m_vExchanges.insert( "NYSE,NYSE_ARCA" );
  m_vExchanges.insert( "NASDAQ,NGSM" );
  m_vExchanges.insert( "NASDAQ,NGM" );
  // m_vExchanges.insert( "OPRA" );  // needed for GLD, but ignore for now
  m_vExchanges.insert( "COMEX" );
  m_vExchanges.insert( "COMEX,COMEX_GBX" );
  //m_vExchanges.insert( "TSE" );
  //m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
  //m_vExchanges.insert( "CANADIAN,TSE" );  // don't do yet, simplifies contract creation for IB

  m_vClassifiers.clear();
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::Equity );
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::IEOption );
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::Future );
  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::FOption );

  std::cout << "Subsetting symbols ... " << std::endl;
  ou::tf::iqfeed::InMemoryMktSymbolList listIQFeedSymbols;
  ou::tf::IQFeedSymbolListOps::SelectSymbols selection( m_vClassifiers, listIQFeedSymbols );
  m_listIQFeedSymbols.SelectSymbolsByExchange( m_vExchanges.begin(), m_vExchanges.end(), selection );
  std::cout << "  " << listIQFeedSymbols.Size() << " symbols in subset." << std::endl;

  //std::string sFileName( sFileNameMarketSymbolSubset );
  std::cout << "Saving subset to " << sFileNameMarketSymbolSubset << " ..." << std::endl;
//  listIQFeedSymbols.HandleParsedStructure( m_listIQFeedSymbols.GetTrd( m_sNameUnderlying ) );
//  m_listIQFeedSymbols.SelectOptionsByUnderlying( m_sNameOptionUnderlying, listIQFeedSymbols );
  listIQFeedSymbols.SaveToFile( sFileNameMarketSymbolSubset );  // __.ser
  std::cout << " ... done." << std::endl;

  // next step will be to add in the options for the underlyings selected.
}

// TODO: set flag to only load once?  Otherwise, is the structure cleared first?
void AppComboTrading::HandleMenuActionLoadSymbolSubset( void ) {
  //std::string sFileName( sFileNameMarketSymbolSubset );
  std::cout << "Loading From " << sFileNameMarketSymbolSubset << " ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( sFileNameMarketSymbolSubset );  // __.ser
  std::cout << "  " << m_listIQFeedSymbols.Size() << " symbols loaded." << std::endl;
}

void AppComboTrading::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppComboTrading::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppComboTrading::HandlePopulateDatabase( void ) {

  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor
    = ou::tf::AccountManager::GlobalInstance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::GlobalInstance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::GlobalInstance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::GlobalInstance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountSimulator
    = ou::tf::AccountManager::GlobalInstance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

  std::string sNull;

  m_pPortfolioMaster
    = ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
      m_idPortfolioMaster, "aoRay", sNull, ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "ComboTrading" );

  ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
    ou::tf::Currency::Name[ ou::tf::Currency::USD ], "aoRay", m_idPortfolioMaster, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "Currency Monitor" );
//  ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::GlobalInstance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "Currency Monitor" );

}

void AppComboTrading::HandleLoadDatabase( void ) {
    ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
    pm.LoadActivePortfolios();
}

// copied from StickShift2, maybe put in common place
void AppComboTrading::HandlePanelNewOrder( const ou::tf::PanelManualOrder::Order_t& order ) {
  try {
    ou::tf::InstrumentManager& mgr( ou::tf::InstrumentManager::GlobalInstance() );
    //pInstrument_t pInstrument = m_vManualOrders[ m_curDialogManualOrder ].pInstrument;
    pInstrument_t pInstrument = m_IBInstrumentInfo.pInstrument;
    if ( !mgr.Exists( pInstrument ) ) {
      mgr.Register( pInstrument );
    }
//    if ( 0 == m_pPosition.get() ) {
//      m_pPosition = ou::tf::PortfolioManager::GlobalInstance().ConstructPosition(
//        m_idPortfolioMaster, pInstrument->GetInstrumentName(), "manual", "ib01", "ib01", m_pExecutionProvider, m_pData1Provider, pInstrument );
//    }
    ou::tf::OrderManager& om( ou::tf::OrderManager::GlobalInstance() );
    ou::tf::OrderManager::pOrder_t pOrder;
    switch ( order.eOrderType ) {
    case ou::tf::OrderType::Market:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity );
//      m_pPosition->PlaceOrder( ou::tf::OrderType::Market, order.eOrderSide, order.nQuantity );
      break;
    case ou::tf::OrderType::Limit:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
//      m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    case ou::tf::OrderType::Stop:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
//      m_pPosition->PlaceOrder( ou::tf::OrderType::Stop, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    }
    //ou::tf::OrderManager::pOrder_t pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1, order.dblPrice2 );
    //om.PlaceOrder( m_tws.get(), pOrder );
  }
  catch (...) {
    int i = 1;
  }
}

void AppComboTrading::HandlePanelSymbolText( const std::string& sName ) {
  // need to fix to handle equity, option, future, etc.  merge with code from above so common code usage
  // 2014/09/30 maybe need to disable this panel, as the order doesn't land in an appropriate portfolio or position.
  if ( m_bIBConnected ) {
    ou::tf::ib::TWS::Contract contract;
    contract.currency = "USD";
    contract.exchange = "SMART";
    contract.secType = "STK";
    contract.symbol = sName;
    // IB responds only when symbol is found, bad symbols will not illicit a response
    //  m_pPanelManualOrder->SetInstrumentDescription( "" );
    //m_tws->RequestContractDetails(
    //  contract,
    //  MakeDelegate( this, &AppComboTrading::HandleIBContractDetails ), MakeDelegate( this, &AppComboTrading::HandleIBContractDetailsDone ) );
    //m_tws->RequestContractDetails(
    //  contract,
    //  [this](const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument){HandleIBContractDetails(details, pInstrument);},
    //  [this](void){HandleIBContractDetailsDone();}
    //  );


    // 2018/08/03 needs to be redone if it is to be used.
    //namespace args = std::placeholders;
    //m_tws->RequestContractDetails(
    //  contract,
    //  std::bind( &AppComboTrading::HandleIBContractDetails, this, args::_1, args::_2 ),
    //  std::bind( &AppComboTrading::HandleIBContractDetailsDone, this )
    //  );
  }
  else {
    std::cout << "AppComboTrading::HandlePanelSymbolText: IB Not Connected" << std::endl;
  }
}

void AppComboTrading::HandlePanelFocusPropogate( unsigned int ix ) {
}

void AppComboTrading::HandleMenuActionSaveSeries( void ) {
  m_worker.Run( MakeDelegate( this, &AppComboTrading::HandleSaveValues ) );
}

void AppComboTrading::HandleSaveValues( void ) {
  std::cout << "Saving collected values ... " << std::endl;
  try {
    //std::string sPrefixSession( "/app/ComboTrading/" + m_sTSDataStreamStarted + "/" + m_pBundle->Name() );
    //std::string sPrefix86400sec( "/bar/86400/AtmIV/" + iter->second.sName.substr( 0, 1 ) + "/" + iter->second.sName );
    //std::string sPrefix86400sec( "/app/ComboTrading/AtmIV/" + m_pBundle->Name() );
    //m_pBundle->SaveData( sPrefixSession, sPrefix86400sec );
    const std::string sPrefixSession( c_sFileName_Series + "/" + m_sTSDataStreamStarted );
    m_pPanelCharts->SaveSeries( sPrefixSession, c_sFileName_Series );
    m_fedrate.SaveSeries( sPrefixSession );
  }
  catch(...) {
    // TODO: will occur when attempting over-write, may need to try a delete first in the code above
    std::cout << " ... issues with saving ... " << std::endl;
  }
  std::cout << "  ... Done " << std::endl;
}

void AppComboTrading::HandleSave( wxCommandEvent& event ) {
  CallAfter( [this](){ // do after main gui action, allows debugging
    SaveState();
  });
}

void AppComboTrading::HandleLoad( wxCommandEvent& event ) {
  CallAfter( [this](){ // do after main gui action, allows debugging
    LoadState();
  });
}

void AppComboTrading::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sFileName_State );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppComboTrading::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( c_sFileName_State );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppComboTrading::OnClose( wxCloseEvent& event ) {

  m_timerGuiRefresh.Stop();

  SaveState();

  DelinkFromPanelProviderControl();

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.OnPortfolioLoaded.Remove( MakeDelegate( this, &AppComboTrading::HandlePortfolioLoad ) );
  pm.OnPositionLoaded.Remove( MakeDelegate( this, &AppComboTrading::HandlePositionLoad ) );

  m_db.OnRegisterTables.Remove( MakeDelegate( this, &AppComboTrading::HandleRegisterTables ) );
  m_db.OnRegisterRows.Remove( MakeDelegate( this, &AppComboTrading::HandleRegisterRows ) );

  assert( m_db.IsOpen() );
  m_db.Close();

  event.Skip();  // auto followed by Destroy();
}

// this probably isn't in the gui thread
void AppComboTrading::OnData1Connecting( int status ) {
  //m_pPanelCharts->SetProviders( m_pData1Provider, m_pData2Provider, m_pExecutionProvider );
}

// this probably isn't in the gui thread
void AppComboTrading::OnData1Connected( int status ) {
//  if ( m_bData1Connected & m_bExecConnected ) {
//    Start();
//  }
  //m_pPanelCharts->StartWatch();
}

// this probably isn't in the gui thread
void AppComboTrading::OnData1Disconnecting( int status ) {
  //m_pPanelCharts->StopWatch();
}

// this probably isn't in the gui thread
void AppComboTrading::OnData1Disconnected( int status ) {
//  if ( !m_bData1Connected & !m_bExecConnected ) {
//    Stop();
//  }
}

// this probably isn't in the gui thread
void AppComboTrading::OnExecConnecting( int status ) {
  //m_pPanelCharts->SetProviders( m_pData1Provider, m_pData2Provider, m_pExecutionProvider );
}

// this probably isn't in the gui thread
void AppComboTrading::OnExecConnected( int status ) {
  if ( m_bData1Connected & m_bExecConnected ) {
//    Start();
  }
}

// this probably isn't in the gui thread
void AppComboTrading::OnExecDisconnecting( int status ) {
}

// this probably isn't in the gui thread
void AppComboTrading::OnExecDisconnected( int status ) {
  if ( !m_bData1Connected & !m_bExecConnected ) {
//    Stop();
  }
}

void AppComboTrading::OnIQFeedConnected( int status ) {
  //m_libor.SetWatchOn( m_iqfeed );
  m_fedrate.SetWatchOn( m_iqfeed );
}

void AppComboTrading::OnIQFeedDisconnecting( int status ) {
  //m_libor.SetWatchOff();
  m_fedrate.SetWatchOff();
}

void AppComboTrading::HandleMenuActionEmitYieldCurve( void ) {
  //ou::tf::libor::EmitYieldCurve();
  //m_libor.EmitYieldCurve();
  //std::cout << "Libor: " << std::endl << m_libor;
  std::cout << "FedRate: " << std::endl << m_fedrate;
}

void AppComboTrading::OnAssertFailure(
  const wxChar* file, int line, const wxChar* function, const wxChar* condition, const wxChar* message ) {
  std::cout
    << "wxAssert: "
    << " file " << *file
    << " line " << line
    << " function " << *function
    << " condition " << *condition
    << " message " << *message
    << std::endl;
}

//void AppComboTrading::HandleEvent( wxEvtHandler*, wxEventFunction, wxEvent& ) const {
//   //wxASSERT( 0 );
//}
