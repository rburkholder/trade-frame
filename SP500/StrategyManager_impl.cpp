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

// todo:
// * save/load models

#include <sstream>

#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>

#include <OUCommon/TimeSource.h>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include <TFTrading/ComposeInstrument.hpp>

#include "Config.hpp"
#include "Features.hpp"
#include "StrategyManager_impl.hpp"

StrategyManager_impl::StrategyManager_impl(
  const config::Choices& choices
, fQueueTask_t&& fQueueTask
, fSetChartDataView_t&& fSetChartDataView
, fDone_t&& fDone
)
: m_choices( choices )
, m_fQueueTask( std::move( fQueueTask ) )
, m_fSetChartDataView( std::move( fSetChartDataView ) )
, m_fDone( std::move( fDone ) )
, m_model( choices.m_sTorchDevice, choices.m_ixTorchDevice )
{

  switch ( m_choices.eMode ) {
    case config::Choices::EMode::view_training:
      {
        //m_cdv.SetNames( "SPY - training", m_choices.m_sFileTraining );
        m_cdv.SetNames( "SPY", "training" );
        LoadPanelFinancialChart();
        ou::tf::HDF5DataManager hdm(  ou::tf::HDF5DataManager::RO, m_choices.m_vFileTraining.front() );
        IterateHDF5( hdm, [this,&hdm]( const std::string& s1, const std::string& s2 ){ HandleLoadTreeHdf5Object_View( hdm, s1, s2 ); } );
      }
      break;
    case config::Choices::EMode::view_validate:
      {
        m_cdv.SetNames( "SPY - validate", m_choices.m_sFileValidate );
        LoadPanelFinancialChart();
        ou::tf::HDF5DataManager hdm(  ou::tf::HDF5DataManager::RO, m_choices.m_sFileValidate );
        IterateHDF5( hdm, [this,&hdm](const std::string& s1, const std::string& s2 ){ HandleLoadTreeHdf5Object_View( hdm, s1, s2 ); } );
      }
      break;
    case config::Choices::EMode::train_then_validate:
    case config::Choices::EMode::train_save_validate:
    case config::Choices::EMode::train_then_save:
    case config::Choices::EMode::no_model_validate:
      BuildProvider_Sim();
      break;
    case config::Choices::EMode::train_then_run_live:
      BuildProvider_Sim();
      break;
    case config::Choices::EMode::load_then_validate:
      m_model.Load( m_choices.m_sFileModelLoad );
      BuildProvider_Sim();
      break;
    case config::Choices::EMode::load_then_run_live:
      m_model.Load( m_choices.m_sFileModelLoad );
      Phase_predict();
      break;
    case config::Choices::EMode::no_model_run_live:
      BuildProvider_IQF(); // todo:  validate flow
      break;
    case config::Choices::EMode::unknown:
      assert( false );
      break;
  }

}

StrategyManager_impl::~StrategyManager_impl() {
  m_pComposeInstrument.reset();
  m_fSetChartDataView( ou::tf::WinChartView::EView::live_review, nullptr );
  m_pStrategy.reset();
  if ( m_sim ) {
    m_sim->Reset();
  }
  m_mapSymbolInfo.clear();
  m_mapHdf5Instrument.clear();
  m_pComposeInstrument.reset();
  m_sim.reset();
  m_tws.reset();
  m_iqf.reset();
}

void StrategyManager_impl::Phase_predict() {
  m_model.EnablePredictionMode();
  switch ( m_choices.eMode ) {
    case config::Choices::EMode::train_then_save:
      m_model.Save( m_choices.m_sFileModelSave );
      break;
    case config::Choices::EMode::train_save_validate:
      m_model.Save( m_choices.m_sFileModelSave );
      // no break, just fall through to train_then_validate
    case config::Choices::EMode::load_then_validate:
    case config::Choices::EMode::train_then_validate:
      RunStrategy_predict_sim();
      break;
    case config::Choices::EMode::load_then_run_live:
    case config::Choices::EMode::train_then_run_live:
      m_startDateUTC = ou::TimeSource::GlobalInstance().External().date(); // DailyTradeTimeFrame default constructor format
      m_startTimeUTC = boost::posix_time::time_duration( 0, 0, 0 );
      BuildProvider_IQF(); // which will run RunStrategy_predict_live();
      break;
    default:
      assert( false );
      break;
  }
}

void StrategyManager_impl::BuildProvider_Sim() {

  m_sim = ou::tf::SimulationProvider::Factory();
  //m_data = m_exec = m_sim;
  //m_sim->SetThreadCount( m_choices.nThreads );  // don't do this, will post across unsynchronized threads

  m_sim->OnConnected.Add( MakeDelegate( this, &StrategyManager_impl::HandleSimConnected ) );
  m_sim->Connect();

}

void StrategyManager_impl::BuildProvider_IQF() {

  m_iqf = ou::tf::iqfeed::Provider::Factory();
  //m_data = m_exec = m_sim;
  //m_sim->SetThreadCount( m_choices.nThreads );  // don't do this, will post across unsynchronized threads

  //BOOST_LOG_TRIVIAL(trace) << "BuildProvider_IQF - look at thread id"; // => is thread id of training process
  m_iqf->OnConnected.Add( MakeDelegate( this, &StrategyManager_impl::HandleIQFConnected ) );
  m_iqf->Connect();

}

bool StrategyManager_impl::ValidateSimFile( const std::string& sDataFileName ) {

  bool bOk( true );

  BOOST_LOG_TRIVIAL(trace) << "validate sim file: " << sDataFileName << ',' << m_sSimulatorGroupDirectory;

  try {
    if ( 0 < sDataFileName.size() ) {
      m_sim->SetHdf5FileName( sDataFileName );
    }
    m_sim->SetGroupDirectory( m_sSimulatorGroupDirectory );
  }
  catch( const H5::Exception& e ) {
    // need to look at lib/TFHDF5TimeSeries/HDF5DataManager.cpp line 100 for refinement
    BOOST_LOG_TRIVIAL(error) << "group open failed (1) " << sDataFileName << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (2) " << sDataFileName << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }
  catch( ... ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (3) " << sDataFileName << ',' << m_sSimulatorGroupDirectory;
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

  return bOk;
}



void StrategyManager_impl::HandleSimConnected( int ) {
  switch ( m_choices.eMode ) {
    case config::Choices::EMode::train_then_validate:
    case config::Choices::EMode::train_save_validate:
    case config::Choices::EMode::train_then_save:
    case config::Choices::EMode::train_then_run_live:
      m_model.Train_Init();
      m_iterFileTraining = m_choices.m_vFileTraining.begin(); // iterate through training files
      m_fQueueTask( [this](){ RunStrategy_train(); } );
      break;
    case config::Choices::EMode::load_then_validate:
      m_fQueueTask( [this](){ Phase_predict(); } );
      break;
    case config::Choices::EMode::no_model_validate:
      m_fQueueTask( [this](){ RunStrategy_predict_sim(); } );
      break;
    default:
      assert( false );
      break;
  }
}

void StrategyManager_impl::HandleIQFConnected( int ) {
  //BOOST_LOG_TRIVIAL(trace) << "HandleIQFConnected - look at thread id"; // => is iqf network thread
  //m_fQueueTask( [this](){ RunStrategy_predict_live(); } );
  m_pComposeInstrument = std::make_unique<ou::tf::ComposeInstrument>(
    m_iqf
  , [this](){
    RunStrategy_predict_live();
  } );
}

void StrategyManager_impl::RunStrategy_train() {

  BOOST_LOG_TRIVIAL(info) << "model train: started";

  m_cdv_train.Clear();
  m_mapHdf5Instrument.clear();
  m_sSimulatorGroupDirectory.clear();
  ou::tf::HDF5DataManager hdm( ou::tf::HDF5DataManager::RO, *m_iterFileTraining );
  IterateHDF5( hdm, [this,&hdm](const std::string& s1, const std::string& s2 ){ HandleLoadTreeHdf5Object_Sim( hdm, s1, s2 ); } );

  if ( ValidateSimFile( *m_iterFileTraining ) ) {
    m_sim->SetOnSimulationComplete( MakeDelegate( this, &StrategyManager_impl::HandleSimComplete_train ) );
    m_cdv_train.SetNames( "SPY - train", *m_iterFileTraining );
    m_fSetChartDataView( ou::tf::WinChartView::EView::sim_trail, &m_cdv_train );
    BuildStrategy_sim(
      m_startDateUTC,
      m_cdv_train,
      [this]( const Features_raw& raw, Features_scaled& scaled )->ou::tf::Price { // fForward_t
        m_model.Append( raw, scaled );
        return ou::tf::Price( raw.dt + boost::posix_time::time_duration( 0, 0, m_model.PredictionDistance() ), 0.0 );
      } );
    m_pStrategy->Start();
  }
}

void StrategyManager_impl::RunStrategy_predict_sim() {
  // run stratgy with built model in simulation mode
  BOOST_LOG_TRIVIAL(info) << "model predict: started";

  m_mapHdf5Instrument.clear();
  m_sSimulatorGroupDirectory.clear();
  ou::tf::HDF5DataManager hdm( ou::tf::HDF5DataManager::RO, m_choices.m_sFileValidate );
  IterateHDF5( hdm, [this,&hdm](const std::string& s1, const std::string& s2 ){ HandleLoadTreeHdf5Object_Sim( hdm, s1, s2 ); } );

  if ( ValidateSimFile( m_choices.m_sFileValidate ) ) {
    m_sim->SetOnSimulationComplete( MakeDelegate( this, &StrategyManager_impl::HandleSimComplete_predict ) );
    m_cdv_predict.SetNames( "SPY - predict", m_choices.m_sFileValidate );
    m_fSetChartDataView( ou::tf::WinChartView::EView::sim_trail, &m_cdv_predict );
    BuildStrategy_sim(
      m_startDateUTC,
      m_cdv_predict,
      [this]( const Features_raw& raw, Features_scaled& scaled )->ou::tf::Price { // fForward_t
        m_model.Append( raw, scaled );
        return ou::tf::Price( raw.dt + boost::posix_time::time_duration( 0, 0, m_model.PredictionDistance() ), m_model.Predict() );
      } );
    m_pStrategy->Start();
  }
}

void StrategyManager_impl::RunStrategy_predict_live() {
  // run stratgy with built model in live mode
  BOOST_LOG_TRIVIAL(info) << "model in live environment: started";

  m_mapHdf5Instrument.clear();

  //m_model.SetPredictionResult(
  //  [this,distance]( const Model::rPrediction_t& r ){
  //    m_pStrategy->PredictionVector( distance, r.size(), r.data() );
  //  } );

  m_cdv_predict.SetNames( "SPY", "live" );
  m_fSetChartDataView( ou::tf::WinChartView::EView::live_trail, &m_cdv_predict );
  BuildStrategy_live(
    m_startDateUTC, // this may require adjusting
    m_cdv_predict,
    [this]( const Features_raw& raw, Features_scaled& scaled )->ou::tf::Price { // fForward_t
      m_model.Append( raw, scaled );
      return ou::tf::Price( raw.dt + boost::posix_time::time_duration( 0, 0, m_model.PredictionDistance() ), m_model.Predict() );
    } );
  m_pStrategy->Start();

}

void StrategyManager_impl::BuildStrategy_sim( const boost::gregorian::date date, ou::ChartDataView& cdv, Strategy::fForward_t&& fForward ) {

  using pWatch_t = Strategy::pWatch_t;
  using pPosition_t = Strategy::pPosition_t;

  m_pStrategy = std::make_unique<Strategy>(
    cdv,
    m_choices.m_flags,
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedWatch_t&& f ){ // fConstructWatch_t
      mapHdf5Instrument_t::iterator iter = m_mapHdf5Instrument.find( sIQFeedSymbolName );
      assert( m_mapHdf5Instrument.end() != iter );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( iter->second, m_sim );
      f( pWatch );
    },
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedPosition_t&& f ){ // fConstructPosition_t
      mapHdf5Instrument_t::iterator iter = m_mapHdf5Instrument.find( sIQFeedSymbolName );
      assert( m_mapHdf5Instrument.end() != iter );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( iter->second, m_sim );
      pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_sim );
      f( pPosition );
    },
    [this](){ // fStart_t
      // does this cross into foreground thread?
      m_sim->Run();
    },
    [this](){ // fStop_t
    },
    std::move( fForward ) // fForward_t
  );

  assert( m_pStrategy );
  BOOST_LOG_TRIVIAL(info) << "simulation date: " << date;
  m_pStrategy->InitForUSEquityExchanges( date );
  //BOOST_LOG_TRIVIAL(trace) << "market open: " << m_pStrategy->GetRegularHoursOpen();
  m_pStrategy->InitForNextDay(); // due to also collecting futures which started previous evening
  BOOST_LOG_TRIVIAL(trace) << "market open: " << m_pStrategy->GetRegularHoursOpen();
}

void StrategyManager_impl::BuildStrategy_live( const boost::gregorian::date date, ou::ChartDataView& cdv, Strategy::fForward_t&& fForward ) {

  using pWatch_t = Strategy::pWatch_t;
  using pPosition_t = Strategy::pPosition_t;

  m_pStrategy = std::make_unique<Strategy>(
    cdv,
    m_choices.m_flags,
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedWatch_t&& fWatch ){ // fConstructWatch_t
      m_pComposeInstrument->Compose(
        sIQFeedSymbolName
      , [this,fWatch_=std::move(fWatch)]( pInstrument_t pInstrument, bool bConstructed ){
        assert( pInstrument );
        assert( bConstructed );  // todo: fix and register later when db is attached
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqf );
        fWatch_( pWatch );
      }
      );
    },
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedPosition_t&& fPosition ){ // fConstructPosition_t
      m_pComposeInstrument->Compose(
        sIQFeedSymbolName
      , [this,fPosition_=std::move(fPosition)]( pInstrument_t pInstrument, bool bConstructed ){
        assert( pInstrument );
        assert( bConstructed );  // todo: fix and register later when db is attached
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqf );
        pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_iqf );
        fPosition_( pPosition );
      }
      );
    },
    [this](){ // fStart_t
    },
    [this](){ // fStop_t
    },
    std::move( fForward ) // fForward_t
  );

  assert( m_pStrategy );
  BOOST_LOG_TRIVIAL(info) << "live date: " << date;
  //m_pStrategy->InitForUSEquityExchanges( date );
  //m_pStrategy->InitForNextDay(); // due to also collecting futures which started previous evening
}

void StrategyManager_impl::HandleSimComplete_train() {

  //m_fSetChartDataView( ou::tf::WinChartView::EState::sim_review, &m_cdv_build );
  m_fSetChartDataView( ou::tf::WinChartView::EView::sim_review, nullptr );

  std::stringstream ss;
  m_sim->EmitStats( ss );
  BOOST_LOG_TRIVIAL(info) << "simulation (train) results " << ss.str();

  m_model.Train_BuildSamples();

  m_fQueueTask(
  [this](){
    m_pStrategy.reset(); // needs to be prior to sim reset
    m_sim->Reset();
  });

  ++m_iterFileTraining;
  if ( m_choices.m_vFileTraining.end() != m_iterFileTraining ) {
    m_fQueueTask(
      [this](){
        RunStrategy_train();
      });
  }
  else {
    m_fQueueTask(
      [this](){
        try {
          m_model.Train_Perform( m_choices.m_hp );
          Phase_predict();
        }
        catch( ... ) {
          BOOST_LOG_TRIVIAL(error) << "training error";
        }
      } );
  }

}

void StrategyManager_impl::HandleSimComplete_predict() {

  m_fSetChartDataView( ou::tf::WinChartView::EView::sim_review, &m_cdv_predict );

  std::stringstream ss;
  m_sim->EmitStats( ss );
  BOOST_LOG_TRIVIAL(info) << "simulation (predict) results " << ss.str();

  m_fQueueTask( [this](){ CleanUp_predict(); } );

}

void StrategyManager_impl::CleanUp_predict() {
  //m_pStrategy.reset(); // needs to be prior to sim reset
  //m_sim->Reset(); // can't do this without resetting strategy first
  m_fDone();
}

void StrategyManager_impl::IterateHDF5( ou::tf::HDF5DataManager& hdm, fHandleLoadTreeHdf5Object_t&& f ) {
  ou::tf::hdf5::IterateGroups ig(
    hdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group(  group, name ); }, // path
    std::move( f )  // timeseries
    );
}

void StrategyManager_impl::HandleLoadTreeHdf5Group( const std::string& sGroup, const std::string& sName ) {
  //BOOST_LOG_TRIVIAL(info) << "1 Group  " << sGroup << ' ' << sName;
}

void StrategyManager_impl::HandleLoadTreeHdf5Object_Sim( ou::tf::HDF5DataManager& hdm, const std::string& sGroup, const std::string& sName ) {
  // for now, confirm all objects in one directory, to be used by simulator

  std::string sPrefix;
  size_t instance {};
  std::string::const_iterator ixLead( sGroup.begin() );
  std::string::const_iterator ixTrail( sGroup.begin() );
  while ( sGroup.end() != ixLead ) {
    if ( '/' == *ixLead ) {
      switch ( instance ) {
        case 0: // leading /
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

  // sPrefix has something similar to: '"/app/collector/20250629-21:40:01.765214"'

  if ( 0 == m_sSimulatorGroupDirectory.size() ) {
    m_sSimulatorGroupDirectory = sPrefix;
  }
  else {
    assert( sPrefix == m_sSimulatorGroupDirectory );
  }

  bool bAdded( true );
  ou::tf::HDF5Attributes attrObject( hdm, sGroup );
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
        bAdded = false;
        assert( true ); // ignore other types for now
    }
    if ( pInstrument ) {
      pInstrument->SetMultiplier( attrObject.GetMultiplier() );
      pInstrument->SetSignificantDigits( attrObject.GetSignificantDigits() );
      m_mapHdf5Instrument.emplace( sName, pInstrument );
    }
  }

  BOOST_LOG_TRIVIAL(info)
  << "Sim Object,"
  << sGroup << ',' << sName << ','
  << attrObject.GetSignature() << ','
  << attrObject.GetInstrumentType() << ','
  << attrObject.GetMultiplier() << ','
  << attrObject.GetSignificantDigits() << ','
  << ( bAdded ? "added" : "ignored" )
  ;

}

// non-sim mode
void StrategyManager_impl::HandleLoadTreeHdf5Object_View( ou::tf::HDF5DataManager& hdm, const std::string& sGroup, const std::string& sName ) {
  // select only ones in the list
  ESymbol eSymbol = m_pkwmSymbol->FindMatch( sName );
  if ( ESymbol::UKNWN != eSymbol ) {
    ou::tf::HDF5Attributes attrObject( hdm, sGroup );
    BOOST_LOG_TRIVIAL(info)
      << "Object,"
      << sGroup << ',' << sName << ','
      << attrObject.GetSignature() << ','
      << attrObject.GetInstrumentType() << ','
      << attrObject.GetMultiplier() << ','
      << attrObject.GetSignificantDigits()
      ;

    mapSymbolInfo_t::iterator iterSymbol = m_mapSymbolInfo.find( eSymbol );
    assert ( m_mapSymbolInfo.end() != iterSymbol );

    if ( ou::tf::Trade::Signature() == attrObject.GetSignature() ) {
      ou::tf::HDF5TimeSeriesContainer<ou::tf::Trade> tsRepository( hdm, sGroup );
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
        ou::tf::HDF5TimeSeriesContainer<ou::tf::Quote> tsRepository( hdm, sGroup );
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

// non-sim mode
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

  m_fSetChartDataView( ou::tf::WinChartView::EView::live_review, &m_cdv );
}

// non-sim mode
void StrategyManager_impl::LoadPanelFinancialChart() {

  m_pkwmSymbol = new ou::KeyWordMatch<ESymbol>( ESymbol::UKNWN, 6 );
  InitStructures( ESymbol::SPY,  "SPY",    1 );
  InitStructures( ESymbol::II6A, "II6A.Z", 2 );
  InitStructures( ESymbol::II6D, "II6D.Z", 3 );
  InitStructures( ESymbol::JT6T, "JT6T.Z", 4 );
  InitStructures( ESymbol::LI6N, "LI6N.Z", 5 );
  InitStructures( ESymbol::TR6T, "TR6T.Z", 6 );
}

