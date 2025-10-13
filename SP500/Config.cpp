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
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: March 30, 2025 17:59:23
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <OUCommon/KeyWordMatch.h>

#include "Config.hpp"

namespace {
  static const std::string sChoice_Mode(  "mode" );
  static const std::string sChoice_sFileTraining( "file_training" );
  static const std::string sChoice_sFileValidate( "file_validate" );
  static const std::string sChoice_sLearningRate( "learning_rate" );
  static const std::string sChoice_sNumEpochs( "num_epochs" );
  static const std::string sChoice_sFileModelLoad( "file_model_load" );
  static const std::string sChoice_sFileModelSave( "file_model_save" );
  static const std::string sChoice_sFlagEnableBidAskPrice( "flag_enable_bid_ask_price" );
  static const std::string sChoice_sFlagEnableBidAskVolume( "flag_enable_bid_ask_volume" );
  static const std::string sChoice_sFlagEnableImbalance( "flag_enable_imbalance" );
  static const std::string sChoice_sFlagEnablePrediction( "flag_enable_prediction" );
  static const std::string sChoice_sFlagEnableAdvDec( "flag_enable_advdec" );
  static const std::string sChoice_sGPUDevice( "torch_device" );
  static const std::string sChoice_sGPUInstance( "torch_instance" );


  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, bool bRequired, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = std::move( vm[name].as<T>() );
      if constexpr( std::is_same<T, config::Choices::vFileTraining_t>::value ) {
        for ( const auto& item: dest ) {
          BOOST_LOG_TRIVIAL(info) << name << " = " << item;
        }
      }
      else {
        BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
      }
    }
    else {
      if ( bRequired ) {
        BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
        bOk = false;
      }
    }
  return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Choices& choices ) {

  bool bOk( true );
  std::string sMode;

  ou::KeyWordMatch<config::Choices::EMode> kwmMode( config::Choices::EMode::unknown, 10 );
  kwmMode.AddPattern( "view_training_data", config::Choices::EMode::view_training );
  kwmMode.AddPattern( "view_validate_data", config::Choices::EMode::view_validate );
  kwmMode.AddPattern( "train/validate", config::Choices::EMode::train_then_validate );
  kwmMode.AddPattern( "train/save/validate", config::Choices::EMode::train_save_validate );
  kwmMode.AddPattern( "train/save", config::Choices::EMode::train_then_save );
  kwmMode.AddPattern( "train/runlive", config::Choices::EMode::train_then_run_live );
  kwmMode.AddPattern( "load/validate", config::Choices::EMode::load_then_validate );
  kwmMode.AddPattern( "load/runlive", config::Choices::EMode::load_then_run_live );
  kwmMode.AddPattern( "no_model_validate", config::Choices::EMode::no_model_validate );
  kwmMode.AddPattern( "no_model_runlive", config::Choices::EMode::no_model_run_live );

  try {

    int ix;

    po::options_description config( "SP500 config" );
    config.add_options()
    ( sChoice_Mode.c_str(), po::value<std::string>( &sMode ), "mode setting: view_training_data, view_validate_data, train/validate, train/golive" )
    ( sChoice_sFileTraining.c_str(), po::value<Choices::vFileTraining_t>( &choices.m_vFileTraining ), "training file" )
    ( sChoice_sFileValidate.c_str(), po::value<std::string>( &choices.m_sFileValidate )->default_value( "" ), "validation file" )
    ( sChoice_sLearningRate.c_str(), po::value<double>( &choices.m_hp.m_dblLearningRate )->default_value( 0.01 ), "learning rate" )
    ( sChoice_sNumEpochs.c_str(), po::value<int>( &choices.m_hp.m_nEpochs )->default_value( 1000 ), "number of epochs" )
    ( sChoice_sFileModelLoad.c_str(), po::value<std::string>( &choices.m_sFileModelLoad )->default_value( "sp500.pt" ), "model file to be loaded" )
    ( sChoice_sFileModelSave.c_str(), po::value<std::string>( &choices.m_sFileModelSave )->default_value( "sp500.pt" ), "model file to be saved" )
    ( sChoice_sFlagEnableBidAskPrice.c_str(), po::value<bool>( &choices.m_flags.bEnableBidAskPrice )->default_value( false ), "enable bid/ask price on chart" )
    ( sChoice_sFlagEnableBidAskVolume.c_str(), po::value<bool>( & choices.m_flags.bEnableBidAskVolume )->default_value( false ), "enalbe bid/ask volume on chart" )
    ( sChoice_sFlagEnableImbalance.c_str(), po::value<bool>( &choices.m_flags.bEnableImbalance )->default_value( false ), "enable imbalance indicator" )
    ( sChoice_sFlagEnablePrediction.c_str(), po::value<bool>( &choices.m_flags.bEnablePrediction )->default_value( false ), "enable prediction" )
    ( sChoice_sFlagEnableAdvDec.c_str(), po::value<bool>( &choices.m_flags.bEnableAdvDec )->default_value( false ), "enable adv/dec" )
    ( sChoice_sGPUDevice.c_str(), po::value<std::string>( &choices.m_sTorchDevice )->default_value( "cpu" ), "torch device name" )
    ( sChoice_sGPUInstance.c_str(), po::value<int>( &ix )->default_value( 0 ), "torch device instance" )
    ;
    po::variables_map vm;

    std::ifstream ifs( sFileName.c_str() );
    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "SP500 config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sChoice_Mode, false, sMode );
      bOk &= parse<Choices::vFileTraining_t>( sFileName, vm, sChoice_sFileTraining, true, choices.m_vFileTraining );
      bOk &= parse<std::string>( sFileName, vm, sChoice_sFileValidate, true, choices.m_sFileValidate );
      bOk &= parse<double>( sFileName, vm, sChoice_sLearningRate, false, choices.m_hp.m_dblLearningRate );
      bOk &= parse<int>( sFileName, vm, sChoice_sNumEpochs, false, choices.m_hp.m_nEpochs );
      bOk &= parse<std::string>( sFileName, vm, sChoice_sFileModelLoad, true, choices.m_sFileModelLoad );
      bOk &= parse<std::string>( sFileName, vm, sChoice_sFileModelSave, true, choices.m_sFileModelSave );
      bOk &= parse<bool>( sFileName, vm, sChoice_sFlagEnableBidAskPrice, false, choices.m_flags.bEnableBidAskPrice );
      bOk &= parse<bool>( sFileName, vm, sChoice_sFlagEnableBidAskVolume, false, choices.m_flags.bEnableBidAskVolume );
      bOk &= parse<bool>( sFileName, vm, sChoice_sFlagEnableImbalance, false, choices.m_flags.bEnableImbalance );
      bOk &= parse<bool>( sFileName, vm, sChoice_sFlagEnablePrediction, false, choices.m_flags.bEnablePrediction );
      bOk &= parse<bool>( sFileName, vm, sChoice_sFlagEnableAdvDec, false, choices.m_flags.bEnableAdvDec );
      bOk &= parse<std::string>( sFileName, vm, sChoice_sGPUDevice, false, choices.m_sTorchDevice );

      bOk &= parse<int>( sFileName, vm, sChoice_sGPUInstance, false, ix );
      choices.m_ixTorchDevice = ix; // todo: deal with overflow (int8_t is char, so parser treats it incorrectly)
    }

    if ( 100.0 > choices.m_hp.m_nEpochs ) {
      BOOST_LOG_TRIVIAL(error)
        << sFileName << ' '
        << sChoice_sNumEpochs << " (" << choices.m_hp.m_nEpochs << ") < 100";
      bOk = false;
    }

  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " config parse error: " << e.what();
    bOk = false;
  }
  catch(...) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " config unknown error";
    bOk = false;
  }

  choices.eMode = kwmMode.FindMatch( sMode );
  if ( config::Choices::EMode::unknown == choices.eMode ) {
    bOk = false;
    BOOST_LOG_TRIVIAL(error) << sFileName << " unknown mode setting: " << sMode;
  }

  return bOk;

}

} // namespace config