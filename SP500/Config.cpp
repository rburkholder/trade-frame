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

  ou::KeyWordMatch<config::Choices::EMode> kwmMode( config::Choices::EMode::unknown, 3 );
  kwmMode.AddPattern( "view_training_data", config::Choices::EMode::view_training );
  kwmMode.AddPattern( "view_validate_data", config::Choices::EMode::view_validate );
  kwmMode.AddPattern( "train/validate", config::Choices::EMode::train_and_validate );

  try {

    po::options_description config( "SP500 config" );
    config.add_options()

    ( sChoice_Mode.c_str(), po::value<std::string>( &sMode ), "mode setting: view_training_data, view_validate_data, train/validate" )
    ( sChoice_sFileTraining.c_str(), po::value<Choices::vFileTraining_t>( &choices.m_vFileTraining ), "training file" )
    ( sChoice_sFileValidate.c_str(), po::value<std::string>( &choices.m_sFileValidate )->default_value( "" ), "validation file" )
    ( sChoice_sLearningRate.c_str(), po::value<double>( &choices.m_hp.m_dblLearningRate )->default_value( 0.01 ), "learning rate" )
    ( sChoice_sNumEpochs.c_str(), po::value<int>( &choices.m_hp.m_nEpochs )->default_value( 1000 ), "number of epochs" )
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