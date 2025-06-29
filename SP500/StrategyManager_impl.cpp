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
 * File:    StrategyManager_impl.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: June 28, 2025 21:20:51
 */

#include <boost/regex.hpp>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include "Config.hpp"
#include "StrategyManager_impl.hpp"

StrategyManager_impl::StrategyManager_impl( const config::Choices& choices, ou::ChartDataView& cdv )
: m_choices( choices )
, m_cdv( cdv )
{
  m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO, m_choices.m_sHdf5File );

  if ( m_choices.m_bRunSim ) {
    RunSimulation();
  }
  else {
    LoadPanelFinancialChart();
    IterateObjects();
  }
}

StrategyManager_impl::~StrategyManager_impl() {
  m_mapSymbolInfo.clear();
}

void StrategyManager_impl::RunSimulation() {
 // scan hdf5 file for object groups
 // set the group directory in simulator
 // construct strategy with symbols
 //   provide watch & position constructors

  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group(  group, name ); }, // path
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object_Sim( group, name ); }  // timeseries
  );

  BuildProviders_Sim();
}

bool StrategyManager_impl::BuildProviders_Sim() {

  bool bOk( true );

  // construct m_sim
  //   does not need to be in PanelProviderControl
  m_sim = ou::tf::SimulationProvider::Factory();
  m_data = m_exec = m_sim;
  //m_sim->SetThreadCount( m_choices.nThreads );  // don't do this, will post across unsynchronized threads

  try {
    if ( 0 < m_choices.m_sHdf5File.size() ) {
      m_sim->SetHdf5FileName( m_choices.m_sHdf5File );
    }
    m_sim->SetGroupDirectory( m_sSimulatorGroupDirectory );
  }
  catch( const H5::Exception& e ) {
    // need to look at lib/TFHDF5TimeSeries/HDF5DataManager.cpp line 100 for refinement
    BOOST_LOG_TRIVIAL(error) << "group open failed (1) " << m_choices.m_sHdf5File << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (2) " << m_choices.m_sHdf5File << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }
  catch( ... ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (3) " << m_choices.m_sHdf5File << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }

  // extract date, time from group directory name
  // TODO: maybe set datetime as an attribute in the hdf5 group
  if ( bOk ) {
    // 20221220-09:20:13.187534
    boost::smatch what;

    static const boost::regex exprDate { "(20[2-3][0-9][0-1][0-9][0-3][0-9])" };
    if ( boost::regex_search( m_sSimulatorGroupDirectory, what, exprDate ) ) {
      m_startDateUTC = boost::gregorian::from_undelimited_string( what[ 0 ] );
    }
    else bOk = false;

    static const boost::regex exprTime { "([0-9][0-9]:[0-9][0-9]:[0-9][0-9])" };
    if ( boost::regex_search( m_sSimulatorGroupDirectory, what, exprTime ) ) {
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
    m_sim->OnConnected.Add( MakeDelegate( this, &StrategyManager_impl::HandleSimConnected ) );
    m_sim->SetOnSimulationComplete( MakeDelegate( this, &StrategyManager_impl::HandleSimComplete ) );
    m_sim->Connect();
  }

  return bOk;
}

void StrategyManager_impl::HandleSimConnected( int ) {
  using pWatch_t = Strategy::pWatch_t;
  using pPosition_t = Strategy::pPosition_t;
  BOOST_LOG_TRIVIAL(info) << "building strategy";
  m_pStrategy = std::make_unique<Strategy>(
    m_cdv,
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedWatch_t&& f ){ // fConstructWatch_t
      mapHdf5Instrument_t::iterator iter = m_mapHdf5Instrument.find( sIQFeedSymbolName );
      assert( m_mapHdf5Instrument.end() != iter );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( iter->second, m_data );
      f( pWatch );
    },
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedPosition_t&& f ){ // fConstructPosition_t
      mapHdf5Instrument_t::iterator iter = m_mapHdf5Instrument.find( sIQFeedSymbolName );
      assert( m_mapHdf5Instrument.end() != iter );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( iter->second, m_data );
      pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_exec );
      f( pPosition );
    },
    [this](){
      // does this cross into foreground thread?
      BOOST_LOG_TRIVIAL(info) << "simulation run";
      m_sim->Run();
    },
    [this](){
    }
  );
  assert( m_pStrategy );
  m_pStrategy->InitForUSEquityExchanges( m_startDateUTC );
  m_pStrategy->InitForNextDay();
}

void StrategyManager_impl::HandleSimComplete() {

  BOOST_LOG_TRIVIAL(info) << "simulation complete, post processing";

  torch::manual_seed( 1 );
  torch::cuda::manual_seed_all( 1 );

  torch::DeviceType device;
  int num_devices = 0;
  if ( torch::cuda::is_available() ) {
    device = torch::kCUDA;
    num_devices = torch::cuda::device_count();
    BOOST_LOG_TRIVIAL(info) << "number of CUDA devices detected: " << num_devices;
    // when > 1, then can use, as example ' .device(torch::kCUDA, 1 )'
  }
  else {
    device = torch::kCPU;
    BOOST_LOG_TRIVIAL(info) << "no CUDA devices detected, set device to CPU";
  }

  m_pStrategy->BuildModel( device );

  // todo:
  // * init simulator, run strategy, build model
  // * init simulator, run strategy, use model for prediction
  // * save/load models
}

void StrategyManager_impl::IterateObjects() {
  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group(  group, name ); }, // path
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object_Static( group, name ); }  // timeseries
    );
}

void StrategyManager_impl::HandleLoadTreeHdf5Group( const std::string& sGroup, const std::string& sName ) {
  //BOOST_LOG_TRIVIAL(info) << "1 Group  " << sGroup << ' ' << sName;
}

void StrategyManager_impl::HandleLoadTreeHdf5Object_Sim( const std::string& sGroup, const std::string& sName ) {
  // for now, confirm all objects in one directory, to be used by simulator

  std::string sPrefix;
  size_t instance {};
  std::string::const_iterator ixLead( sGroup.begin() );
  std::string::const_iterator ixTrail( sGroup.begin() );
  while ( sGroup.end() != ixLead ) {
    if ( '/' == *ixLead ) {
      switch ( instance ) {
        case 0: // leading
          break;
        case 1: // 'app'
          break;
        case 2: // 'collector'
          break;
        case 3: // datetime
          sPrefix = std::string( ixTrail, ixLead );
          break;
        case 4: // collection
          break;
        case 5: // symbol
          break;
      }
      ++instance;
    }
    ++ixLead;
  }

  if ( 0 == m_sSimulatorGroupDirectory.size() ) {
    m_sSimulatorGroupDirectory = sPrefix;
  }
  else {
    assert( sPrefix == m_sSimulatorGroupDirectory );
  }

  ou::tf::HDF5Attributes attrObject( *m_pdm, sGroup );
  BOOST_LOG_TRIVIAL(info)
    << "Sim Object,"
    << sGroup << ',' << sName << ','
    << attrObject.GetSignature() << ','
    << attrObject.GetInstrumentType() << ','
    << attrObject.GetMultiplier() << ','
    << attrObject.GetSignificantDigits()
    ;
  mapHdf5Instrument_t::iterator iterHdf5Instrument = m_mapHdf5Instrument.find( sName );
  if ( m_mapHdf5Instrument.end() == iterHdf5Instrument ) {
    pInstrument_t pInstrument;
    auto type = attrObject.GetInstrumentType();
    switch ( type ) {
      case ou::tf::InstrumentType::EInstrumentType::Stock:
        pInstrument = std::make_shared<ou::tf::Instrument>(
          sName, type, "simulator"
        );
        break;
      case ou::tf::InstrumentType::EInstrumentType::Index:
        pInstrument = std::make_shared<ou::tf::Instrument>(
          sName, type, "simulator"
        );
        break;
      default:
        assert( true ); // ignore other types for now
    }
    if ( pInstrument ) {
      pInstrument->SetMultiplier( attrObject.GetMultiplier() );
      pInstrument->SetSignificantDigits( attrObject.GetSignificantDigits() );
      m_mapHdf5Instrument.emplace( sName, pInstrument );
    }
  }
}

void StrategyManager_impl::HandleLoadTreeHdf5Object_Static( const std::string& sGroup, const std::string& sName ) {
  // select only ones in the list
  ESymbol eSymbol = m_pkwmSymbol->FindMatch( sName );
  if ( ESymbol::UKNWN != eSymbol ) {
    ou::tf::HDF5Attributes attrObject( *m_pdm, sGroup );
    BOOST_LOG_TRIVIAL(info)
      << "3 Object,"
      << sGroup << ',' << sName << ','
      << attrObject.GetSignature() << ','
      << attrObject.GetInstrumentType() << ','
      << attrObject.GetMultiplier() << ','
      << attrObject.GetSignificantDigits()
      ;
    mapSymbolInfo_t::iterator iterSymbol = m_mapSymbolInfo.find( eSymbol );
    assert ( m_mapSymbolInfo.end() != iterSymbol );

    if ( ou::tf::Trade::Signature() == attrObject.GetSignature() ) {
      ou::tf::HDF5TimeSeriesContainer<ou::tf::Trade> tsRepository( *m_pdm, sGroup );
      ou::tf::HDF5TimeSeriesContainer<ou::tf::Trade>::iterator begin, end;
      begin = tsRepository.begin();
      end = tsRepository.end();
      hsize_t cnt = end - begin;
      ou::tf::Trades& trades( iterSymbol->second.trades );
      trades.Resize( cnt );
      tsRepository.Read( begin, end, &trades );

      ou::ChartEntryIndicator& indicator( iterSymbol->second.indicatorTrade );
      indicator.SetName( sName );

      boost::posix_time::ptime first;
      bool bFirst( true );

      trades.ForEach(
        [&indicator,&first,&bFirst,iterSymbol]( const ou::tf::Trade& trade ){
          boost::posix_time::ptime dt( trade.DateTime() );
          if ( 1 == iterSymbol->second.ixChart ) {
            indicator.Append( dt - iterSymbol->second.tdDelay, trade.Price() );
          }
          else {
            indicator.Append( dt, trade.Price() );
          }
          if ( bFirst ) {
            first = dt;
            bFirst = false;
          }
        } );

      if ( false && ( 1 == iterSymbol->second.ixChart ) ) {
        ou::tf::Quotes& quotes( iterSymbol->second.quotes );
        ou::ChartEntryIndicator& asks( iterSymbol->second.indicatorAsk );
        ou::ChartEntryIndicator& bids( iterSymbol->second.indicatorBid );
        quotes.ForEach(
          [&bids,&asks,&first]( const ou::tf::Quote& quote ){
            boost::posix_time::ptime dt( quote.DateTime() );
            if ( first <= dt ) {
              asks.Append( dt, quote.Ask() );
              bids.Append( dt, quote.Bid() );
              }
          } );
      }

    }

    if ( 1 == iterSymbol->second.ixChart ) {
      if ( ou::tf::Quote::Signature() == attrObject.GetSignature() ) {
        ou::tf::HDF5TimeSeriesContainer<ou::tf::Quote> tsRepository( *m_pdm, sGroup );
        ou::tf::HDF5TimeSeriesContainer<ou::tf::Quote>::iterator begin, end;
        begin = tsRepository.begin();
        end = tsRepository.end();
        hsize_t cnt = end - begin;
        ou::tf::Quotes& quotes( iterSymbol->second.quotes );
        quotes.Resize( cnt );
        tsRepository.Read( begin, end, &quotes );

        ou::ChartEntryIndicator& asks( iterSymbol->second.indicatorAsk );
        asks.SetName( "ask" );
        ou::ChartEntryIndicator& bids( iterSymbol->second.indicatorBid );
        bids.SetName( "bid" );
      }
    }
  }
}

void StrategyManager_impl::InitStructures( ESymbol eSymbol, const std::string& sName, size_t ixChart, boost::posix_time::time_duration td ) {
  m_pkwmSymbol->AddPattern( sName, eSymbol );
  auto result = m_mapSymbolInfo.emplace( eSymbol, SymbolInfo( sName, ixChart, td ) );
  assert( result.second );
  SymbolInfo& si( result.first->second );
  si.indicatorTrade.SetColour( ou::Colour::Green );
  m_cdv.Add( ixChart, &si.indicatorTrade );
  if ( 1 == ixChart ) {
    si.indicatorAsk.SetColour( ou::Colour::Red );
    m_cdv.Add( ixChart, &si.indicatorAsk );
    si.indicatorBid.SetColour( ou::Colour::Blue );
    m_cdv.Add( ixChart, &si.indicatorBid );
  }
}

void StrategyManager_impl::LoadPanelFinancialChart() {

  m_pkwmSymbol = new ou::KeyWordMatch<ESymbol>( ESymbol::UKNWN, 6 );
  //InitStructures( ESymbol::SPY,  "SPY",    1, boost::posix_time::time_duration( 0, 15, 0 ) );
  InitStructures( ESymbol::SPY,  "SPY",    1 );
  //InitStructures( ESymbol::SPY,  "ES-20250620", 1 );
  InitStructures( ESymbol::II6A, "II6A.Z", 2 );
  InitStructures( ESymbol::II6D, "II6D.Z", 3 );
  InitStructures( ESymbol::JT6T, "JT6T.Z", 4 );
  InitStructures( ESymbol::LI6N, "LI6N.Z", 5 );
  InitStructures( ESymbol::TR6T, "TR6T.Z", 6 );
}

