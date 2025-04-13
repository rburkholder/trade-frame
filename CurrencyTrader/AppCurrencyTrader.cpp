/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    AppCurrencyTrader.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 09, 2024 19:58:27
 */

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/regex.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <fmt/core.h>

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
//#include <TFTrading/BuildInstrument.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/TreeItem.hpp>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>
#include <TFVuTrading/PanelProviderControlv2.hpp>

#include "Strategy.hpp"
#include "AppCurrencyTrader.hpp"

namespace {
  static const std::string c_sAppName( "CurrencyTrader" );
  static const std::string c_sVendorName( "One Unified Net Limited" );

  static const std::string c_sDirectory( "." );
  static const std::string c_sDbName(          c_sDirectory + '/' + c_sAppName + ".db" );
  static const std::string c_sStateFileName(   c_sDirectory + '/' + c_sAppName + ".state" );
  static const std::string c_sChoicesFilename( c_sDirectory + '/' + c_sAppName + ".cfg" );

  static const std::string c_sMenuItemPortfolio( "_USD" );

  static const std::string c_sPortfolioCurrencyName( "USD" ); // pre-created, needs to be uppercase
  static const std::string c_sPortfolioSimulationName( "sim" );
  static const std::string c_sPortfolioRealTimeName( "live" );
  static const std::string c_sAlgorithmName( "forex_swing" );
}

IMPLEMENT_APP(AppCurrencyTrader)

bool AppCurrencyTrader::OnInit() {

  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetAppDisplayName( c_sAppName );
  wxApp::SetVendorDisplayName( "(c)2024 " + c_sVendorName );

  wxApp::OnInit();

  m_bProvidersConfirmed = false;
  m_nTSDataStreamSequence = 0;
  m_dblCommissionTotal = 0.0;

  m_stateSoftwareReset = ESoftwareReset::quiescent;

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
  }
  else {
    return false;
  }

  if ( boost::filesystem::exists( m_choices.m_sHdf5File ) ) {}
  else {
    BOOST_LOG_TRIVIAL(error) << m_choices.m_sHdf5File << " does not exist";
    return false;
  }

  m_pFrameMain = new FrameMain( 0, wxID_ANY,c_sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );

  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerFrame;
  wxBoxSizer* sizerUpper;
  wxBoxSizer* sizerLower;

  sizerFrame = new wxBoxSizer( wxVERTICAL );
  m_pFrameMain->SetSizer( sizerFrame );

  sizerUpper = new wxBoxSizer( wxHORIZONTAL );
  sizerFrame->Add( sizerUpper, 0, wxEXPAND, 2);

  sizerLower = new wxBoxSizer( wxVERTICAL );
  sizerFrame->Add( sizerLower, 1, wxEXPAND, 2 );

  m_pPanelCurrencyStats = new PanelCurrencyStats( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
  sizerUpper->Add( m_pPanelCurrencyStats, 0, wxEXPAND, 2);
  sizerUpper->AddSpacer( 2 );

  {
    std::string sName( m_choices.m_sBaseCurrency );
    std::transform(sName.begin(), sName.end(), sName.begin(), ::toupper);

    m_currencyBase = ou::tf::Currency::ParseName( sName );
    auto result = m_mapCurrency.emplace( m_currencyBase, Currency( m_choices.m_dblBaseCurrencyTopUp ) );
    assert( result.second );
    Currency& currency( result.first->second );

    currency.fUpdateCurrency = std::move( m_pPanelCurrencyStats->AddCurrency( sName ) );
  }

  bool bOk( true );
  if ( 0 == m_choices.m_sHdf5SimSet.size() ) { // live setup
    bOk = BuildProviders_Live( sizerUpper );
  }
  else { // sim setup
    bOk = BuildProviders_Sim();
  }

  if ( !bOk ) {
    BOOST_LOG_TRIVIAL(error) << "failure with building provider";
    return false;
  }

  // for now, overwrite old database on each start
  if ( boost::filesystem::exists( c_sDbName ) ) {
    boost::filesystem::remove( c_sDbName );
  }

  // this needs to be placed after the providers are registered
  m_pdb = std::make_unique<ou::tf::db>( c_sDbName ); // construct database

  // TODO: put logging panel with a resize slider at bottom of FrameMain
  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_pPanelLogging->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  sizerUpper->Add( m_pPanelLogging, 2, wxEXPAND, 2);

  m_splitterData = new wxSplitterWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
  m_splitterData->SetMinimumPaneSize(20);

  m_treeSymbols = new wxTreeCtrl( m_splitterData, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE );

  m_pWinChartView = new ou::tf::WinChartView( m_splitterData, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_pWinChartView->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

  m_splitterData->SplitVertically(m_treeSymbols, m_pWinChartView, 50);
  sizerLower->Add(m_splitterData, 1, wxGROW, 2);

  m_pWorkGuard = std::make_unique<work_guard_t>( asio::make_work_guard( m_io ) );
  m_thread = std::move( std::thread( [this](){ m_io.run(); } ) );

  PopulatePortfolioChart();
  PopulateTreeRoot();
  LoadPortfolioCurrency();
  ConstructStrategyList();

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppCurrencyTrader::OnClose, this );  // start close of windows and controls

  m_pFrameMain->Bind( wxEVT_MOVE, &AppCurrencyTrader::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

  return true;
}

void AppCurrencyTrader::PopulatePortfolioChart() {
  m_ceUnRealized.SetName( "unrealized" );
  m_ceRealized.SetName( "realized" );
  m_ceCommissionsPaid.SetName( "commission" );
  m_ceTotal.SetName( "total" );

  m_ceUnRealized.SetColour( ou::Colour::Blue );
  m_ceRealized.SetColour( ou::Colour::Purple );
  m_ceCommissionsPaid.SetColour( ou::Colour::Red );
  m_ceTotal.SetColour( ou::Colour::Green );

  m_dvPortfolio.Add( 0, &m_ceUnRealized );
  m_dvPortfolio.Add( 0, &m_ceRealized );
  m_dvPortfolio.Add( 0, &m_ceTotal );
  m_dvPortfolio.Add( 2, &m_ceCommissionsPaid );

  //m_pWinChartView->SetChartDataView( &m_dvPortfolio );
  m_pWinChartView->SetChartDataView( nullptr ); // not chart to start with
}

void AppCurrencyTrader::PopulateTreeRoot() {
  TreeItem::Bind( m_pFrameMain, m_treeSymbols );
  m_pTreeItemRoot = new TreeItem( m_treeSymbols, "/" ); // initialize tree
  m_pTreeItemRoot->SetOnClick(
    [this]( TreeItem* pTreeItem ){
      m_pWinChartView->SetChartDataView( nullptr );
    } );
  //wxTreeItemId idPortfolio = m_treeSymbols->AppendItem( idRoot, sMenuItemPortfolio, -1, -1, new CustomItemData( sMenuItemPortfolio ) );
  //m_treeSymbols->Bind( wxEVT_TREE_ITEM_MENU, &AppAutoTrade::HandleTreeEventItemMenu, this, m_treeSymbols->GetId() );
  //m_treeSymbols->Bind( wxEVT_TREE_ITEM_RIGHT_CLICK, &AppAutoTrade::HandleTreeEventItemRightClick, this, m_treeSymbols->GetId() );
  //m_treeSymbols->Bind( wxEVT_TREE_SEL_CHANGED, &AppAutoTrade::HandleTreeEventItemChanged, this, m_treeSymbols->GetId() );
  m_pTreeItemPortfolio = m_pTreeItemRoot->AppendChild(
    c_sMenuItemPortfolio,
    [this]( TreeItem* pTreeItem ){
      if ( 0 != m_choices.m_sHdf5SimSet.size() ) {
        m_pWinChartView->SetSim( true );
      }
      m_pWinChartView->SetChartDataView( &m_dvPortfolio );
    }
  );

  m_treeSymbols->ExpandAll();
}

void AppCurrencyTrader::ConstructStrategyList() {

  assert( 0 == m_mapPair.size() );

  boost::gregorian::date adjusted_date = Strategy::AdjustTimeFrame( m_startDateUTC, m_startTimeUTC );

  bool bLog( true );
  if ( bLog ) {
    BOOST_LOG_TRIVIAL(info)
      << "start UTC=" << m_startDateUTC << ' ' << m_startTimeUTC
      << " => adjusted_date=" << adjusted_date
      ;
  }

  for ( config::CurrencyPair& ps: m_choices.m_vCurrencyPair ) {

    const ou::tf::Instrument::idInstrument_t& idInstrument( ps.m_sName );
    mapPair_t::iterator iterPair = m_mapPair.find( idInstrument );
    if ( m_mapPair.end() == iterPair ) {

      assert( 0 < ps.m_nTradingAmount );
      pStrategy_t pStrategy = std::make_unique<Strategy>( m_choices.m_strategy );
      pStrategy->SetTransaction(
        ps.m_nTradingAmount,
        [this](ou::tf::Currency::ECurrency src, double debit,
               ou::tf::Currency::ECurrency dst, double credit,
               double commission ){

          mapCurrency_t::iterator iterSrc = m_mapCurrency.find( src );
          assert( m_mapCurrency.end() != iterSrc );
          mapCurrency_t::iterator iterDst = m_mapCurrency.find( dst );
          assert( m_mapCurrency.end() != iterDst );

          BOOST_LOG_TRIVIAL(info)
            << "acct,before,de,"
            << ou::tf::Currency::Name[ src ] << ',' << iterSrc->second.amount << ','
            << "cr,"
            << ou::tf::Currency::Name[ dst ] << ',' << iterDst->second.amount << ','
            << m_dblCommissionTotal
            ;

          iterSrc->second.amount -= debit;    // TODO record as a db transaction
          iterDst->second.amount += credit;
          m_dblCommissionTotal += commission;

          BOOST_LOG_TRIVIAL(info)
            << "acct,after,de,"
            << ou::tf::Currency::Name[ src ] << ',' << iterSrc->second.amount << ','
            << "cr,"
            << ou::tf::Currency::Name[ dst ] << ',' << iterDst->second.amount << ','
            << m_dblCommissionTotal
            ;

          }
      );

      auto result = m_mapPair.emplace( idInstrument, std::move( pStrategy ) );
      assert( result.second );

      iterPair = result.first;
      assert( result.second );
      Pair& pair( iterPair->second );
      Strategy& strategy( *pair.pStrategy );

      iterPair->second.fUpdatePair = std::move( m_pPanelCurrencyStats->AddPair( idInstrument ) );

      strategy.InitFor24HourMarkets( adjusted_date );

      boost::posix_time::ptime dt;

      ou::TimeSource& ts( ou::TimeSource::GlobalInstance() );
      auto tz = ts.LoadTimeZone( ps.m_sTimeZone );

      const auto MarketOpen_UTC = strategy.GetRegularHoursOpen();
      const boost::local_time::local_date_time MarketOpen_Local( MarketOpen_UTC, tz );

      const auto MarketClose_UTC = strategy.GetRegularHoursClose();
      const boost::local_time::local_date_time MarketClose_Local( MarketClose_UTC, tz );

      if ( bLog ) {
        BOOST_LOG_TRIVIAL(info)
          << ps.m_sName
          << " mo_raw_utc=" << MarketOpen_UTC
          << ",mo_lcl_utc=" << MarketOpen_Local.utc_time()
          << ",mo_lcl_local=" << MarketOpen_Local.local_time()
          << ",mc_raw_utc=" << MarketClose_UTC
          << ",mc_lcl_utc=" << MarketClose_Local.utc_time()
          << ",mc_lcl_local=" << MarketClose_Local.local_time()
          ;
      }

      // over-ride starting time from instrument's config
      boost::posix_time::ptime dtStart_Local( MarketOpen_Local.local_time().date(), ps.m_tdStartTime );
      if ( dtStart_Local < MarketOpen_Local.local_time() ) {
        dtStart_Local = boost::posix_time::ptime(
          MarketOpen_Local.local_time().date() + boost::gregorian::date_duration( 1 ),
          ps.m_tdStartTime
        );
      }
      assert( MarketOpen_Local.local_time() <= dtStart_Local );

      boost::local_time::local_date_time lt_open( dtStart_Local.date(), dtStart_Local.time_of_day(), tz, boost::local_time::local_date_time::EXCEPTION_ON_ERROR );
      boost::posix_time::ptime lt_open_utc( lt_open.utc_time() );

      assert( MarketOpen_UTC <= lt_open_utc );

      strategy.SetRegularHoursOpen( lt_open_utc );
      strategy.SetStartTrading( lt_open_utc + boost::posix_time::time_duration( 0, 0, 30 ) );

      boost::posix_time::ptime dtClose_Local( dtStart_Local.date(), ps.m_tdStopTime );
      if ( dtStart_Local >= dtClose_Local ) {
        dtClose_Local = boost::posix_time::ptime(
          dtStart_Local.date() + boost::gregorian::date_duration( 1 ),
          ps.m_tdStopTime
        );
      }
      assert( dtStart_Local < dtClose_Local );

      boost::local_time::local_date_time lt_close( dtClose_Local.date(), dtClose_Local.time_of_day(), tz, boost::local_time::local_date_time::EXCEPTION_ON_ERROR );
      boost::posix_time::ptime lt_close_utc( lt_close.utc_time() );

      if ( bLog ) {
        BOOST_LOG_TRIVIAL(info)
          << ps.m_sName
          << " lt_open_utc=" << lt_open_utc
          << " lt_close_utc=" << lt_close_utc
          << " close=" << MarketClose_UTC
          ;
      }

      assert( lt_open_utc < lt_close_utc );
      assert( lt_close_utc <= MarketClose_UTC );

      strategy.SetCancellation( lt_close_utc - boost::posix_time::time_duration( 0, 3, 0 ) );
      strategy.SetGoNeutral( lt_close_utc - boost::posix_time::time_duration( 0, 2, 30 ) );
      strategy.SetWaitForRegularHoursClose( lt_close_utc - boost::posix_time::time_duration( 0, 2, 0 ) );
      strategy.SetRegularHoursClose( lt_close_utc );

      if ( bLog ) {
        BOOST_LOG_TRIVIAL(info)
          << ps.m_sName
          << " local start=" << dtStart_Local
          << " local close=" << dtClose_Local
          ;

        BOOST_LOG_TRIVIAL(info)
          << ps.m_sName << ','
          << "mo=" << strategy.GetMarketOpen() << ','
          << "rh=" << strategy.GetRegularHoursOpen() << ','
          << "st=" << strategy.GetStartTrading() << ','
          << "nn=" << strategy.GetNoon() << ','
          << "cc=" << strategy.GetCancellation() << ','
          << "gn=" << strategy.GetGoNeutral() << ','
          << "rw=" << strategy.GetWaitForRegularHoursClose() << ','
          << "rc=" << strategy.GetRegularHoursClose() << ','
          << "mc=" << strategy.GetMarketClose() << ','
          << "sr=" << strategy.GetSoftwareReset()
          ;
      }

      BOOST_LOG_TRIVIAL(info)
        << ps.m_sName
        << " active market utc "
        << lt_open_utc << " - " << lt_close_utc
        ;

      if ( 0 == m_choices.m_sHdf5SimSet.size() ) {

        auto dtReset = strategy.GetSoftwareReset() + boost::posix_time::time_duration( 0, 1, 0 );
        pair.ptimerSoftwareReset = std::make_unique<boost::asio::deadline_timer>( m_io, dtReset );
        pair.ptimerSoftwareReset->async_wait(
          [name=ps.m_sName,dtReset]( const boost::system::error_code& error ){
            BOOST_LOG_TRIVIAL(info) << name << " sw reset trial timed out " << dtReset << ' ' << error.to_string(); // replace with appropriate action when confirmed accuracy
            // can the pointer be reset here?
          } );

        strategy.SetResetSoftware( [this,&strategy]()->bool {
          bool bReturn( false );
          switch ( m_stateSoftwareReset ) {
            case ESoftwareReset::looking:
              m_stateSoftwareReset = ESoftwareReset::encountered;
              //break; fall through instead
            case ESoftwareReset::encountered:
              // note that this goes re-entrant
              BOOST_LOG_TRIVIAL(info) << "strategy reset, in encountered";
              strategy.InitForNextDay(); // adjusts DailyTradeTimeFrames ahead by 1 day
              bReturn = true;
              break;
            case ESoftwareReset::quiescent:
              BOOST_LOG_TRIVIAL(warning) << "strategy reset, in quiescence";
              break;
          }
          return bReturn;
        } );
      }

      TreeItem* pTreeItem = m_pTreeItemPortfolio->AppendChild(
        idInstrument,
        [this,idInstrument]( TreeItem* pTreeItem ){
          mapPair_t::iterator iter = m_mapPair.find( idInstrument );
          assert( m_mapPair.end() != iter );
          if ( 0 != m_choices.m_sHdf5SimSet.size() ) {
            m_pWinChartView->SetSim( true );
          }
          m_pWinChartView->SetChartDataView( &iter->second.pStrategy->GetChartDataView() );
        }
      );

    }
    else {
      assert( false );
    }
  }
  m_treeSymbols->ExpandAll();
}

void AppCurrencyTrader::SetStreamStartDateTime() {
  auto dt = ou::TimeSource::GlobalInstance().External();
  m_startDateUTC = dt.date();
  m_startTimeUTC = dt.time_of_day();

  std::stringstream ss;
  ss
    << ou::tf::Instrument::BuildDate( m_startDateUTC )
    << " "
    << m_startTimeUTC
    ;
  m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
}

bool AppCurrencyTrader::BuildProviders_Live( wxBoxSizer* sizer ) {

  m_iqf = ou::tf::iqfeed::Provider::Factory();
  m_iqf->SetName( "iq01" );
  //m_iqf->SetThreadCount( m_choices.nThreads );

  m_tws = ou::tf::ib::TWS::Factory();
  m_tws->SetName( "ib01" );
  m_tws->SetClientId( m_choices.m_nIbInstance );

  // TODO: make the panel conditional on simulation flag
  m_pPanelProviderControl = new ou::tf::v2::PanelProviderControl( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  m_pPanelProviderControl->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  m_pPanelProviderControl->Show();

  sizer->Add( m_pPanelProviderControl, 0, wxALIGN_LEFT, 2);

  m_pPanelProviderControl->Add(
    m_iqf,
    true, false, false, false,
    [](){}, // fConnecting
    [this]( bool bD1, bool bD2, bool bX1, bool bX2 ){ // fConnected
      if ( bD1 ) m_data = m_iqf;
      if ( bX1 ) m_exec = m_iqf;
      ConfirmProviders();
    },
    [](){}, // fDisconnecting
    [](){}  // fDisconnected
  );

  // TODO: need to test the updates for disconnecting & disconnected when TWS recycles each night
  m_pPanelProviderControl->Add(
    m_tws,
    false, false, true, false,
    [](){
      BOOST_LOG_TRIVIAL(info) << "panel: tws connecting";
    }, // fConnecting
    [this]( bool bD1, bool bD2, bool bX1, bool bX2 ){ // fConnected
      BOOST_LOG_TRIVIAL(info) << "panel: tws connected";
      if ( bD1 ) m_data = m_tws;
      if ( bX1 ) m_exec = m_tws;
      ConfirmProviders();
    },
    [](){
      BOOST_LOG_TRIVIAL(info) << "panel: tws disconnecting";
    }, // fDisconnecting
    [](){
      BOOST_LOG_TRIVIAL(info) << "panel: tws disconnected";
    }  // fDisconnected
  );

  // TODO: add to a clean up routine
  m_timerOneSecond.SetOwner( this );
  if ( 0 == m_choices.m_sHdf5SimSet.size() ) {
    Bind( wxEVT_TIMER, &AppCurrencyTrader::HandleTimer, this, m_timerOneSecond.GetId() );
    m_timerOneSecond.Start( 500 );
  }

  SetStreamStartDateTime();

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

  vItems.push_back( new mi( "Close, Done", MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionCloseAndDone ) ) );
  vItems.push_back( new mi( "Save Values", MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionSaveValues ) ) );
  vItems.push_back( new mi( "Emit Swing Track", MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionEmitSwingTrack ) ) );
  vItems.push_back( new mi( "Emit Cubic Coef", MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionEmitCubiCoef ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  return true;
}

bool AppCurrencyTrader::BuildProviders_Sim() {

  bool bOk( true );

  // construct m_sim
  //   does not need to be in PanelProviderControl
  m_sim = ou::tf::SimulationProvider::Factory();
  m_data = m_exec = m_sim;
  //m_sim->SetThreadCount( m_choices.nThreads );  // don't do this, will post across unsynchronized threads

  // ensure hdf5 data is available
  try {
    if ( 0 < m_choices.m_sHdf5File.size() ) {
      m_sim->SetHdf5FileName( m_choices.m_sHdf5File );
    }
    m_sim->SetGroupDirectory( m_choices.m_sHdf5SimSet );
  }
  catch( const H5::Exception& e ) {
    // need to look at lib/TFHDF5TimeSeries/HDF5DataManager.cpp line 100 for refinement
    BOOST_LOG_TRIVIAL(error) << "group open failed (1) " << m_choices.m_sHdf5File << ',' << m_choices.m_sHdf5SimSet;
    bOk = false;
  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (2) " << m_choices.m_sHdf5File << ',' << m_choices.m_sHdf5SimSet;
    bOk = false;
  }
  catch( ... ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (3) " << m_choices.m_sHdf5File << ',' << m_choices.m_sHdf5SimSet;
    bOk = false;
  }

  // extract date, time from group directory name
  // TODO: maybe set datetime as an attribute in the hdf5 group
  if ( bOk ) {
    // 20221220-09:20:13.187534
    boost::smatch what;

    static const boost::regex exprDate { "(20[2-3][0-9][0-1][0-9][0-3][0-9])" };
    if ( boost::regex_search( m_choices.m_sHdf5SimSet, what, exprDate ) ) {
      m_startDateUTC = boost::gregorian::from_undelimited_string( what[ 0 ] );
    }
    else bOk = false;

    static const boost::regex exprTime { "([0-9][0-9]:[0-9][0-9]:[0-9][0-9])" };
    if ( boost::regex_search( m_choices.m_sHdf5SimSet, what, exprTime ) ) {
      m_startTimeUTC = boost::posix_time::duration_from_string( what[ 0 ] );
    }
    else {
      bOk = false;
    }
  }

  // construct the simulation date/time
  if ( bOk ) {

    const boost::posix_time::ptime dtSimulation( ptime( m_startDateUTC, m_startTimeUTC ) );
    boost::local_time::local_date_time lt( dtSimulation, ou::TimeSource::TimeZoneNewYork() );
    boost::posix_time::ptime dtStart = lt.local_time();
    BOOST_LOG_TRIVIAL(info) << "times: " << dtSimulation << "(UTC) is " << dtStart << "(eastern)";
    //dateSim = Strategy::Futures::MarketOpenDate( dtUTC ); //
    //std::cout << "simulation date: " << dateSim << std::endl;

    //m_sSimulationDateTime = boost::posix_time::to_iso_string( dtUTC );
  }

  // construct the simulation menu, attach the events, and start simulation
  if ( bOk ) {
    FrameMain::vpItems_t vItems;
    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

    vItems.push_back( new mi( "Start", MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionSimStart ) ) );
    vItems.push_back( new mi( "Stop",  MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionSimStop ) ) );
    vItems.push_back( new mi( "Stats",  MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionSimEmitStats ) ) );
    m_pFrameMain->AddDynamicMenu( "Simulation", vItems );

    m_sim->OnConnected.Add( MakeDelegate( this, &AppCurrencyTrader::HandleSimConnected ) );
    m_sim->SetOnSimulationComplete( MakeDelegate( this, &AppCurrencyTrader::HandleSimComplete ) );

    CallAfter(
      [this](){
        m_sim->Connect();
      }
    );
  }

  return bOk;
}

void AppCurrencyTrader::HandleSimConnected( int ) {
  ConfirmProviders();
}

void AppCurrencyTrader::HandleMenuActionSimStart() {
  CallAfter(
    [this](){
      m_sim->Run();
    }
  );
}

void AppCurrencyTrader::HandleMenuActionSimStop() {
  CallAfter(
    [this](){
      m_sim->Stop();
    }
  );
}

void AppCurrencyTrader::HandleMenuActionSimEmitStats() {
  std::stringstream ss;
  m_sim->EmitStats( ss );
  std::cout << "Sim Stats: " << ss.str() << std::endl;
}

void AppCurrencyTrader::HandleSimComplete() {
  m_OnSimulationComplete( 0 );
  BOOST_LOG_TRIVIAL(info) << "simulation complete";
}

void AppCurrencyTrader::HandleMenuActionCloseAndDone() {
  std::cout << "Closing & Done" << std::endl;
  CallAfter(
    [this](){
      for ( mapPair_t::value_type& vt: m_mapPair ) {
        vt.second.pStrategy->CloseAndDone();
      }
    } );
}

void AppCurrencyTrader::HandleMenuActionSaveValues() {
  std::cout << "Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      assert( 0 < m_sTSDataStreamStarted.size() );
      m_nTSDataStreamSequence++;
      const std::string sPath(
        "/app/" + c_sAppName + "/" +
        m_sTSDataStreamStarted + "-" +
        boost::lexical_cast<std::string>( m_nTSDataStreamSequence ) // sequence number on each save
      );
      for ( mapPair_t::value_type& vt: m_mapPair ) {
        vt.second.pStrategy->SaveWatch( sPath );
      }
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppCurrencyTrader::HandleMenuActionEmitSwingTrack() {
  std::cout << "Emit Swing Track" << std::endl;
  CallAfter(
    [this](){
      for ( mapPair_t::value_type& vt: m_mapPair ) {
        vt.second.pStrategy->EmitSwingTrack();
      }
    } );
}

void AppCurrencyTrader::HandleMenuActionEmitCubiCoef() {
  CallAfter(
    [this](){
      for ( mapPair_t::value_type& vt: m_mapPair ) {
        vt.second.pStrategy->EmitCubicCoef();
      }
    } );
}

void AppCurrencyTrader::ConfirmProviders() {
  if ( m_bProvidersConfirmed ) {}
  else {
    if ( m_data && m_exec ) {
      if ( m_data->Connected() && m_exec->Connected() ) {

        m_bProvidersConfirmed = true;

        BOOST_LOG_TRIVIAL(info)
          << "ConfirmProviders: data(" << m_data->GetName() << ") "
          << "& execution(" << m_exec->GetName() << ") "
          << "providers available"
          ;

        ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );

        for ( const config::Choices::vCurrencyPair_t::value_type& ps: m_choices.m_vCurrencyPair ) {
          pInstrument_t pInstrument;
          pInstrument = im.LoadInstrument( ou::tf::keytypes::EProviderUserBase, ps.m_sName );
          if ( pInstrument ) { // skip the build
            PopulateStrategy( pInstrument );
          }
          else { // build the instrument
            pInstrument = ConstructInstrumentBase( ps.m_sName, m_choices.m_sExchange );
            EnhanceInstrument( pInstrument ); // has a PopulateStrategy
          }
        }
      }
    }
  }
}

void AppCurrencyTrader::LoadPortfolioCurrency() {

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  if ( pm.PortfolioExists( c_sPortfolioCurrencyName ) ) {
    m_pPortfolioUSD = pm.GetPortfolio( c_sPortfolioCurrencyName );
  }
  else {
    assert( false );  // should already exist
    //m_pPortfolioUSD
    //  = pm.ConstructPortfolio(
    //      sName, "tf01", c_sPortfolioCurrencyName,
    //      ou::tf::Portfolio::EPortfolioType::Standard,
    //      ou::tf::Currency::Name[ ou::tf::Currency::USD ] );
  }
}

void AppCurrencyTrader::PopulateCurrency( ou::tf::Currency::ECurrency id_currency ) {
  mapCurrency_t::iterator iterCurrency;
  iterCurrency = m_mapCurrency.find( id_currency );
  if ( m_mapCurrency.end() == iterCurrency ) {
    auto result = m_mapCurrency.emplace( id_currency, Currency() );
    assert( result.second );
    Currency& currency( result.first->second );
    m_pFrameMain->CallAfter(
      [this, &currency, id_currency](){
        currency.fUpdateCurrency = std::move( m_pPanelCurrencyStats->AddCurrency( ou::tf::Currency::Name[ id_currency ] ) );
        m_pFrameMain->Layout();
      } );
  }
}

void AppCurrencyTrader::PopulateStrategy( pInstrument_t pInstrument ) {

  assert( pInstrument );

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName( ) );

  mapPair_t::iterator iterStrategy = m_mapPair.find( idInstrument );
  assert( m_mapPair.end() != iterStrategy );
  Pair& pair( iterStrategy->second );

  const ou::tf::Currency::ECurrency currency1( pInstrument->GetCurrencyBase() );
  const ou::tf::Currency::ECurrency currency2( pInstrument->GetCurrencyCounter() );

  assert( currency1 != currency2 ); // superfulous as contract id was found

  if ( m_currencyBase == currency1 ) {
    pair.eBase = EBase::First;
    pair.currencyNonBase = currency2;
  }
  else {
    if ( m_currencyBase == currency2 ) {
      pair.eBase = EBase::Second;
      pair.currencyNonBase = currency1;
    }
    else {
      assert( false ); // probably a redundant test to the next assert
    }
  }

  assert( EBase::Unknown != pair.eBase );

  PopulateCurrency( currency1 );
  PopulateCurrency( currency2 );

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pPortfolio_t pPortfolio;

  if ( pm.PortfolioExists( idInstrument ) ) {
    pPortfolio = pm.GetPortfolio( idInstrument );
  }
  else {
    pPortfolio
      = pm.ConstructPortfolio(
          idInstrument, "tf01", c_sPortfolioCurrencyName,
          ou::tf::Portfolio::EPortfolioType::Aggregate,
          pInstrument->GetCurrencyName()
        );
  }

  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_data );

  iterStrategy->second.pStrategy->SetWatch(
    pair.eBase,
    pWatch, pPortfolio,
    [this, pPortfolio, iterStrategy]( pWatch_t pWatch, const std::string& sPositionPrefix )->pPosition_t{
      pPosition_t pPosition = ConstructPosition( pPortfolio, sPositionPrefix, pWatch );
      //iterStrategy->second.bAllowTimer = true;
      return pPosition;
    } );
}

AppCurrencyTrader::pPosition_t AppCurrencyTrader::ConstructPosition( pPortfolio_t pPortfolio, const std::string& sPositionPrefix, pWatch_t pWatch ) {

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  const ou::tf::Instrument::idInstrument_t& idInstrument( pWatch->GetInstrumentName() );

  std::string sPositionName( sPositionPrefix );

  assert( pWatch->GetProvider()->ID() == m_data->ID() );

  switch ( m_data->ID() ) {
    case ou::tf::keytypes::EProviderIB:
      sPositionName += "_ib";
      break;
    case ou::tf::keytypes::EProviderIQF:
      sPositionName += "_iqf";
      break;
    case ou::tf::keytypes::EProviderSimulator:
     sPositionName += "_sim";
     break;
    default:
      assert( false );
  }

  switch ( m_exec->ID() ) {
    case ou::tf::keytypes::EProviderIB:
      sPositionName += "_ib";
      break;
    case ou::tf::keytypes::EProviderIQF:
      sPositionName += "_iqf";
      break;
    case ou::tf::keytypes::EProviderSimulator:
     sPositionName += "_sim";
     break;
    default:
      assert( false );
  }

  const std::string& idPortfolio( pPortfolio->GetRow().idPortfolio );
  pPosition_t pPosition;

  //BOOST_LOG_TRIVIAL(debug) << "ConstructPosition position name: " << sPositionName;

  if ( pm.PositionExists( idPortfolio, sPositionName ) ) {
    pPosition = pm.GetPosition( idPortfolio, sPositionName );
    BOOST_LOG_TRIVIAL(info)
      << "position loaded "
      << pPosition->GetRow().idPortfolio << ','
      << pPosition->GetRow().sName << ','
      << pPosition->GetInstrument()->GetInstrumentName()
      ;
  }
  else {
    pPosition = pm.ConstructPosition(
      idPortfolio, sPositionName,
      c_sAlgorithmName,
      m_exec, pWatch
    );
    if ( ou::tf::keytypes::EProviderSimulator == m_exec->ID() ) {
      m_sim->SetCommission( idInstrument, 0.0 );
    }

    BOOST_LOG_TRIVIAL(info)
      << "position constructed: "
      << pPosition->GetRow().idPortfolio << ','
      << pPosition->GetRow().sName << ','
      << pPosition->GetInstrument()->GetInstrumentName()
      ;
  }

  assert( pPosition );
  return pPosition;
}

AppCurrencyTrader::pInstrument_t AppCurrencyTrader::ConstructInstrumentBase( const std::string& sName, const std::string& sExchange  ) {

  pInstrument_t pInstrument;

  std::string sSymbolName( sName );
  std::transform(sSymbolName.begin(), sSymbolName.end(), sSymbolName.begin(), ::toupper);
  const ou::tf::Currency::pair_t pairCurrency( ou::tf::Currency::Split( sSymbolName ) );

  // TODO: use ConstructInstrument like for PortfolioManager for position/portfolio?
  //   will need an updated registration with alternate names?
  //   will need an updated registration with IB contract assginment?

  pInstrument
    = std::make_shared<ou::tf::Instrument>(
        sName,
        ou::tf::InstrumentType::Currency, sExchange,
        pairCurrency.first, pairCurrency.second
        );

  return pInstrument;
}

void AppCurrencyTrader::EnhanceInstrument( pInstrument_t pInstrument ) {

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );

  const ou::tf::Currency::ECurrency eCurrencyOne( pInstrument->GetCurrencyBase() );
  const ou::tf::Currency::ECurrency eCurrencyTwo( pInstrument->GetCurrencyCounter() );

  const std::string sCurrency1( ou::tf::Currency::Name[ eCurrencyOne ] );
  const std::string sCurrency2( ou::tf::Currency::Name[ eCurrencyTwo ] );

  const std::string sSymbolName_IB( sCurrency1 + '.' + sCurrency2 );
  const std::string sSymbolName_IQFeed( sCurrency1 + sCurrency2 + ".FXCM" );

  BOOST_LOG_TRIVIAL(info)
    << "Compose "
    << idInstrument << ','
    << sSymbolName_IQFeed << ','
    << sSymbolName_IB
    ;

  const auto eIB = ou::tf::keytypes::EProviderIB;
  const auto eIqf = ou::tf::keytypes::EProviderIQF;

  pInstrument->SetAlternateName( eIB,  sSymbolName_IB );
  pInstrument->SetAlternateName( eIqf, sSymbolName_IQFeed );

  if ( ( eIB == m_data->ID() ) || ( eIB == m_exec->ID() ) ) {
    m_tws->RequestContractDetails(
      idInstrument,
      pInstrument,
      [this]( const ContractDetails& details, ou::tf::Instrument::pInstrument_t& pInstrument ){
        BOOST_LOG_TRIVIAL(info) << "IB contract found: "
                 << details.contract.localSymbol
          << ',' << details.contract.conId
          << ',' << details.contract.secType
          << ',' << details.mdSizeMultiplier
          << ',' << fmt::format( "{:f}", details.minTick )
          << ',' << details.contract.exchange
          << ',' << details.validExchanges
          << ',' << details.timeZoneId
          //<< ',' << details.liquidHours
          //<< ',' << details.tradingHours
          << ',' << "market rule id " << details.marketRuleIds
          ;
        m_tws->Sync( pInstrument ); // obtain market rule list

        ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
        im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
        PopulateStrategy( pInstrument );
      },
      [this,pInstrument]( bool bDone ){
        if ( 0 == pInstrument->GetContract() ) {
          BOOST_LOG_TRIVIAL(error)  << "IB contract incomplete " << pInstrument->GetInstrumentName();
        }
      }
    );
  }
  else {
    ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
    im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
    PopulateStrategy( pInstrument );
  }
}

void AppCurrencyTrader::UpdatePanelCurrencyStats() {

  double dblUnRealized;
  double dblRealized;
  double dblCommissionsPaid;
  double dblTotal;

  if ( m_pPortfolioUSD ) {
    m_pPortfolioUSD->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );

    ptime dt = ou::TimeSource::GlobalInstance().Internal();

    m_ceUnRealized.Append( dt, dblUnRealized );
    m_ceRealized.Append( dt, dblRealized );
    m_ceCommissionsPaid.Append( dt, dblCommissionsPaid );
    m_ceTotal.Append( dt, dblTotal );
  }

  double dblBaseTotal {};

  for ( const mapPair_t::value_type& vt: m_mapPair ) {

    const Pair& pair( vt.second );

    auto latest = pair.pStrategy->Latest();
    pair.fUpdatePair( latest.bid, latest.ask, latest.count, latest.commission );

    mapCurrency_t::const_iterator iter = m_mapCurrency.find( pair.currencyNonBase );
    if ( m_mapCurrency.end() != iter ) {
      const Currency& currency( iter->second );

      const double mid( 0.5 * ( latest.bid + latest.ask ) );
      double dblConverted {};
      bool bSuccess( false );

      switch ( pair.eBase ) {
        case EBase::First:
          dblConverted = currency.amount / mid;
          bSuccess = true;
          break;
        case EBase::Second:
          dblConverted = currency.amount * mid;
          bSuccess = true;
          break;
        default:
          BOOST_LOG_TRIVIAL(warning) << vt.first << " has no conversion lookup yet";
      }

      if ( bSuccess ) {
        dblBaseTotal += dblConverted;
        currency.fUpdateCurrency( currency.amount, dblConverted );
      }
    }
    else {
      // log the problem or just wait for things to start?
    }
  }

  mapCurrency_t::const_iterator iter = m_mapCurrency.find( m_currencyBase );
  assert( m_mapCurrency.end() != iter );
  const Currency& base( iter->second );

  dblBaseTotal += base.amount;
  base.fUpdateCurrency( base.amount, dblBaseTotal );
}

void AppCurrencyTrader::HandleTimer( wxTimerEvent& event ) {

  if ( m_bProvidersConfirmed ) {
    UpdatePanelCurrencyStats();
  }

  m_stateSoftwareReset = ESoftwareReset::looking;

  const auto dt = ou::TimeSource::GlobalInstance().External();
  const ou::tf::DatedDatum datum( dt );
  for ( const mapPair_t::value_type& vt: m_mapPair ) {
    //if ( vt.second.bAllowTimer ) {
    //  vt.second.pStrategy->TimeTick<ou::tf::DatedDatum>( datum );
    //}
  }

  // what happens if we have laggards? might be ok, with timeseries cleared in Strategy
  if ( ESoftwareReset::encountered == m_stateSoftwareReset ) {
    HandleMenuActionSaveValues();
    SetStreamStartDateTime(); // affects m_startDateUTC
    if ( m_tws ) {
      if ( m_tws->Connected() ) {
        BOOST_LOG_TRIVIAL(info) << "strategy timer, tws connected";
      }
      else {
        BOOST_LOG_TRIVIAL(info) << "strategy timer, tws reconnecting";
        m_tws->Disconnect();
        m_tws->Connect();
      }
    }
  }

  m_stateSoftwareReset = ESoftwareReset::quiescent;

}

void AppCurrencyTrader::OnFrameMainAutoMove( wxMoveEvent& event ) {
// load state works properly _after_ first move (library initiated)

  CallAfter(
    [this](){
      LoadState();
      m_pFrameMain->Layout();
    }
  );

  m_pFrameMain->Unbind( wxEVT_MOVE, &AppCurrencyTrader::OnFrameMainAutoMove, this );

  event.Skip(); // set to false if we want to ignore auto move

}

void AppCurrencyTrader::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppCurrencyTrader::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( c_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppCurrencyTrader::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  if ( m_timerOneSecond.IsRunning() ) {
    m_timerOneSecond.Stop();
    Unbind( wxEVT_TIMER, &AppCurrencyTrader::HandleTimer, this, m_timerOneSecond.GetId() );
  }

  m_pWinChartView->SetChartDataView( nullptr );

  SaveState();

  m_mapPair.clear();

  m_pWorkGuard.reset();
  if ( m_thread.joinable() ) {
    m_thread.join();
  }

  //m_pBuildInstrument.reset();

  if ( m_pdb ) m_pdb.reset();

  // NOTE: when running the simuliation, perform a deletion instead
  //   use the boost file system utilities?
  //   or the object Delete() operator may work
//  if ( m_choices.bStartSimulator ) {
//    if ( m_pFile ) { // performed at exit to ensure no duplication in file
      //m_pFile->Delete(); // double free or corruption here
//    }
//  }
//  else {
//    if ( m_pFile ) { // performed at exit to ensure no duplication in file
//      m_pFile->Write();
//    }
//  }

  event.Skip();  // auto followed by Destroy();
}

int AppCurrencyTrader::OnExit() {
  // Exit Steps: #4
  return wxAppConsole::OnExit();
}

/*
https://www.forex.com/en-us/forex-trading/what-is-forex-trading/

Every currency in forex trading is signified by three letters. These are known as the ISO 4217 Currency Codes.

The first two letters denote the country. The third represents the currency name.

    AUD = Australia dollar
    USD = United States dollar

Forex currency pair nicknames

    GBP/USD – Cable
    EUR/CHF – Swissy
    EUR/USD – Fiber
    EUR/GBP – Chunnel 
    NZD/USD – Kiwi

Major currency pairs
Over a quarter of all forex trades are in EUR/USD.   the major pairs to have the tightest spreads

    EUR/USD – the euro vs the US dollar 
    USD/JPY – the US dollar versus the Japanese yen
    GBP/USD – British pound sterling versus the US dollar
    AUD/USD – the Australian dollar versus the US dollar 
    USD/CHF – the US dollar versus the Swiss franc
    USD/CAD – the US dollar versus the Canadian dollar

Minor currency pairs

Minor pairs are currency pairs that don’t include the US dollar.
They are also known as cross pairs.

Examples include:

    EUR/GBP – the euro versus British pound sterling
    EUR/CHF – the euro versus the Swiss franc
    GBP/AUD – British pound sterling versus the Australian dollar
    GBP/JPY – British pound sterling versus the Japanese yen
    CAD/JPY – the Canadian dollar versus Japanese yen
    CHF/JPY – the Swiss franc versus the Japanese yen
    EUR/NZD – the euro versus the New Zealand dollar

*/
