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

#include <sol/load_result.hpp>

#include "Sol.hpp"

namespace {
  static const std::filesystem::path c_pathScriptExt( ".lua" );
}

Sol::Sol() {
}

Sol::~Sol() {
  m_mapScript.clear();
  for ( const setPath_t::value_type& vt: m_setPath ) {
    m_fn.DelWatch( vt );
  }
  m_setPath.clear();
}

// one sequence was: move from, create, modify (keep th emodify)

void Sol::AddPath( const std::string& sPath ) {
  setPath_t::iterator iterPath( m_setPath.find( sPath ) );
  if ( m_setPath.end() == iterPath ) {
    m_fn.AddWatch(
      sPath,
      [this,sPath]( ou::FileNotify::EType type, const std::string& sFileName ){

        //BOOST_LOG_TRIVIAL(info) << "Sol - file notify cb1 " << sFileName;
        std::filesystem::path fsPath( sPath );
        fsPath.append( sFileName );

        if ( TestExtension( fsPath ) ) {

          switch ( type ) {
            case ou::FileNotify::EType::create_:
              BOOST_LOG_TRIVIAL(info) << "Sol - file create " << sFileName;
              //Load( fsPath );
              break;
            case ou::FileNotify::EType::modify_:
              BOOST_LOG_TRIVIAL(info) << "Sol - file modify " << sFileName;
              Modify( fsPath );
              break;
            case ou::FileNotify::EType::delete_:
              BOOST_LOG_TRIVIAL(info) << "Sol - file delete " << sFileName;
              Delete( fsPath );
              break;
            case ou::FileNotify::EType::move_from_:
              BOOST_LOG_TRIVIAL(info) << "Sol - file move from " << sFileName;
              //Delete( fsPath );
              break;
            case ou::FileNotify::EType::move_to_:
              BOOST_LOG_TRIVIAL(info) << "Sol - file move to " << sFileName;
              Load( fsPath );
              break;
            case ou::FileNotify::EType::ignored_:
              BOOST_LOG_TRIVIAL(info) << "Sol - file ignore " << sFileName;
              //m_lua.Load( path );
              break;
            default:
              assert( false );
              break;
          }
        }
        else {
          //std::cout << "noop: " << path << std::endl;
        }
        //std::cout << s << std::endl;
      } );
    InitialLoad( sPath );
    m_setPath.emplace( sPath );
  }
  else {
    BOOST_LOG_TRIVIAL(warning) << "Sol::SetPath - " << sPath << " already added";
  }
}

void Sol::DelPath( const std::string& sPath ) {
  setPath_t::iterator iterPath( m_setPath.find( sPath ) );
  if ( m_setPath.end() == iterPath ) {
    BOOST_LOG_TRIVIAL(warning) << "Sol::DelPath - " << sPath << " no available";
  }
  else {
    m_setPath.erase( iterPath );
    m_fn.DelWatch( sPath );
  }

}

void Sol::InitialLoad( const std::string& sPath ) {
  static const std::filesystem::path pathScript( sPath );
  // TODO: check that path is a directory
  for ( auto const& dir_entry: std::filesystem::recursive_directory_iterator{ pathScript } ) {
    if ( dir_entry.is_regular_file() ) {
      if ( TestExtension( dir_entry.path() ) ) {
        try {
          mapScript_t::iterator iterScript( m_mapScript.find( dir_entry.path().string() ) );
          if ( m_mapScript.end() == iterScript ) {
            iterScript = Load( dir_entry.path().string() );
            Attach( iterScript );
          }
          else {
            BOOST_LOG_TRIVIAL(error) << "Sol::InitialLoad exists - " << dir_entry.path().string();
          }
        }
        catch( ... ) {
          BOOST_LOG_TRIVIAL(error) << "Sol::InitialLoad error - " << dir_entry.path().string();
        }
      }
    }
  }
}

Sol::mapScript_t::iterator Sol::Load( const std::string& sPath ) {
  sol::state sol;
  auto result = m_mapScript.emplace( sPath, std::move( sol ) );
  assert( result.second );
  return result.first;
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
    Delete( fsPath );
    Load( fsPath );
  }
}

void Sol::Load( const std::filesystem::path& fsPath ) {
  const std::string sPath( fsPath );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  assert( m_mapScript.end() == iterScript );
  BOOST_LOG_TRIVIAL(info) << "Sol::Load - " << sPath;
  iterScript = Load( sPath );
  Attach( iterScript );
}

void Sol::Delete( const std::filesystem::path& fsPath ) {
  const std::string sPath( fsPath.string() );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  if ( m_mapScript.end() == iterScript ) {
    BOOST_LOG_TRIVIAL(warning) << "Sol::Delete - no script to delete - " << sPath;
  }
  else {
    Detach( iterScript );
    m_mapScript.erase( iterScript );
    BOOST_LOG_TRIVIAL(info) << "Sol::Delete - " << sPath;
  }
}

void Sol::Initialize( sol::state& ) {
}

void Sol::Attach( mapScript_t::iterator iterScript ) {
  assert( m_mapScript.end() != iterScript );
  sol::state& sol( iterScript->second );
  sol.open_libraries( sol::lib::base );
  sol::load_result script = sol.load_file( iterScript->first );
  script();
  Initialize( sol );
  sol::function attach = sol[ "attach" ];
  attach();
}

void Sol::Detach( mapScript_t::iterator iterScript ) {
  assert( m_mapScript.end() != iterScript );
  sol::state& sol( iterScript->second );
  sol::function detach = sol[ "detach" ];
  detach();
}

// Embedding LuaJIT in 30 minutes (or so):
// https://en.blog.nic.cz/2015/08/12/embedding-luajit-in-30-minutes-or-so/

// references Programming in Lua:
// https://www.lua.org/pil/25.html