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
 * File:    Sol.cpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/22 14:16:40
 */

#include <boost/log/trivial.hpp>

#include "Sol.hpp"

namespace {
  static const std::filesystem::path c_pathScriptExt( ".lua" );
}

Sol::Sol() {
}

Sol::~Sol() {
  // TODO: empty m_setPath, m_mapScript
}

void Sol::AddPath( const std::string& sPath ) {
  setPath_t::iterator iterPath( m_setPath.find( sPath ) );
  if ( m_setPath.end() == iterPath ) {
    m_fn.AddWatch(
      sPath,
      []( ou::FileNotify::EType, const std::string& sFileName ){
        BOOST_LOG_TRIVIAL(info) << "Sol - file notify cb " << sFileName;
      } );
    Load( sPath ); // redundant?
    m_setPath.emplace( sPath );
  }
  else {
    BOOST_LOG_TRIVIAL(warning) << "Sol - add notify path " << sPath << " already added";
  }
}

void Sol::DelPath( const std::string& sPath ) {
  setPath_t::iterator iterPath( m_setPath.find( sPath ) );
  if ( m_setPath.end() == iterPath ) {
    BOOST_LOG_TRIVIAL(warning) << "Sol - del notify path " << sPath << " no available";
  }
  else {
    m_setPath.erase( iterPath );
    m_fn.DelWatch( sPath );
  }

}

void Sol::Load( const std::string& sPath ) {
  static const std::filesystem::path pathScript( sPath );
  // TODO: check that path is a directory
  for ( auto const& dir_entry: std::filesystem::recursive_directory_iterator{ pathScript } ) {
    if ( dir_entry.is_regular_file() ) {
      if ( TestExtension( dir_entry.path() ) ) {
        try {
          mapScript_t::iterator iterScript( m_mapScript.find( dir_entry.path().string() ) );
          if ( m_mapScript.end() == iterScript ) {
            sol::state sol;
            //sol.open_libraries( sol::lib::base, sol::lib::package );
            auto result = m_mapScript.emplace( dir_entry.path().string(), std::move( sol ) );
            assert( result.second );
            iterScript = result.first;
            Attach( iterScript );
          }
          else {
            BOOST_LOG_TRIVIAL(error) << "Sol::Load - script loaded - " << dir_entry.path().string();
          }
        }
        catch( ... ) {
          BOOST_LOG_TRIVIAL(error) << "Sol - script_file error on " << dir_entry.path().string();
        }
      }
    }
  }

}

bool Sol::TestExtension( const std::filesystem::path& fsPath ) {
  bool bResult( false );
  if ( fsPath.has_extension() ) {
    if ( c_pathScriptExt == fsPath.extension() ) {
      bResult = true;
    }
  }
  return bResult;
}

void Sol::Modify( const std::filesystem::path& fsPath ) {

  const std::string sPath( fsPath );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  if ( m_mapScript.end() == iterScript ) {
    Load( fsPath );
  }
  else {
    // TODO: undo existing config first
    Delete( fsPath );
    Load( fsPath );
  }
}

void Sol::Load( const std::filesystem::path& fsPath ) {

  const std::string sPath( fsPath );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  if ( m_mapScript.end() != iterScript ) {
    Modify( fsPath );
  }
  else {
    //mapScript_t::iterator iterScript = Parse( sPath );
    BOOST_LOG_TRIVIAL(info) << "Sol::Load - loaded " << sPath;
    Attach( iterScript );
  }
}

void Sol::Delete( const std::filesystem::path& fsPath ) {
  const std::string sPath( fsPath );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  if ( m_mapScript.end() == iterScript ) {
    BOOST_LOG_TRIVIAL(warning) << "Sol::Delete - no script to delete - " << sPath;
  }
  else {
    Detach( iterScript );
    m_mapScript.erase( iterScript );
    BOOST_LOG_TRIVIAL(info) << "Sol::Delete - deleted " << sPath;
  }
}

void Sol::Attach( mapScript_t::iterator iterScript ) {
  assert( m_mapScript.end() != iterScript );
  // step 1: register endpoints, initialize variables
  sol::state& sol( iterScript->second );
  sol.open_libraries( sol::lib::base );
  sol.script_file( iterScript->first );
  // step 2: call the attachment function
}

void Sol::Detach( mapScript_t::iterator iterScript ) {
  // step 1: call the detachment function
}
