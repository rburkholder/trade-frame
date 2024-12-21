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
 * File:    LuaControl.cpp
 * Author:  raymond@burkholder.net
 * Project: MarketTrader
 * Created: 2024/12/15 11:58:12
 */

#include <boost/log/trivial.hpp>

extern "C" {
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
}

#include "LuaControl.hpp"

namespace {
  static const std::filesystem::path c_pathScriptExt( ".lua" );
}

namespace lua {

Control::Control() {

}

Control::~Control() {}

void Control::AddPath( const std::string& sPath, ou::FileNotify::fNotify_t&& f ) {
  Load( sPath );
  m_fn.AddWatch( sPath, std::move( f ) );
}

void Control::DelPath( const std::string& sPath ) {
  m_fn.DelWatch( sPath );
  // unload() ?
}

void Control::Load( const std::string& sPath ) {
  static const std::filesystem::path pathScript( sPath );
  for ( auto const& dir_entry: std::filesystem::recursive_directory_iterator{ pathScript } ) {
    if ( dir_entry.is_regular_file() ) {
      if ( TestExtension( dir_entry.path() ) ) {
        //std::cout << dir_entry << '\n';
        //m_lua.Load( dir_entry.path() );
        //script.Run( dir_entry.path().string() );
      }
    }
  }

}

bool Control::TestExtension( const std::filesystem::path& path ) {
  bool bResult( false );
  if ( path.has_extension() ) {
    if ( c_pathScriptExt == path.extension() ) {
      bResult = true;
    }
  }
  return bResult;
}

Control::mapScript_t::iterator Control::Parse( const std::string& sPath ) {

  mapScript_t::iterator iterScript( m_mapScript.end() );

  Lua lua;

  /* Load the file containing the script to be run */
  int status = luaL_loadfile( lua(), sPath.c_str() );
  if ( LUA_OK == status ) {
    auto result = m_mapScript.emplace(
      mapScript_t::value_type(
        sPath,
        std::move( lua )
      ) );
    assert( result.second );

    iterScript = result.first;
  }
  else {
    /* If something went wrong, error message is at the top of the stack */
    BOOST_LOG_TRIVIAL(error)
      << "lua::control::Parse error with loading file "
      << sPath << ": "
      << '(' << status << ')' << ' '
      << lua_tostring( lua(), -1 )
      ;
  }

  return iterScript;
}

void Control::Load( const std::filesystem::path& path ) {

  const std::string sPath( path );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  if ( m_mapScript.end() != iterScript ) {
    Modify( path );
  }
  else {
    mapScript_t::iterator iterScript = Parse( sPath );
    BOOST_LOG_TRIVIAL(info) << "ScriptLua::Load - loaded " << sPath;
    Attach( iterScript );
  }
}

void Control::Modify( const std::filesystem::path& path ) {

  const std::string sPath( path );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  if ( m_mapScript.end() == iterScript ) {
    Load( path );
  }
  else {
    // TODO: undo existing config first
    Delete( path );
    Load( path );
  }
}

void Control::Delete( const std::filesystem::path& path ) {
  const std::string sPath( path );
  mapScript_t::iterator iterScript = m_mapScript.find( sPath );
  if ( m_mapScript.end() == iterScript ) {
    BOOST_LOG_TRIVIAL(warning) << "ScriptLua::Delete - no script to delete - " << sPath;
  }
  else {
    Detach( iterScript );
    m_mapScript.erase( iterScript );
    BOOST_LOG_TRIVIAL(info) << "ScriptLua::Delete - deleted " << sPath;
  }
}

void Control::Attach( mapScript_t::iterator iterScript ) {

  //BOOST_LOG_TRIVIAL(info) << "Attach";
  Lua& lua( iterScript->second );
  //lua_State* pLua( script.pLua );
  int result {};
  int test {};

  // first pass: register endpoints, initialize variables
  result = lua_pcall( lua(), 0, 0, 0 );
  if ( LUA_OK != result ) {
    BOOST_LOG_TRIVIAL(error)
      << "ScriptLua::Attach0 failed to run script 1: "
      << lua_tostring( lua(), -1 )
      ;
    lua_pop( lua(), 1 );
  }
  else {
    // second pass: call the attachment function
    lua_getglobal( lua(), "attach" ); // page 242 of 2016 Programming in Lua
    //test = lua_isnil( pLua, -1 ); // page 227
    lua_pushlightuserdata( lua() , this );
    //test = lua_isuserdata( pLua, -1 );
    //test = lua_isnil( pLua, -2 );

    result = lua_pcall( lua(), 1, 0, 0 );
    if ( LUA_OK != result ) {
      BOOST_LOG_TRIVIAL(error)
        << "ScriptLua::Attach1 failed to run script 2: "
        << lua_tostring( lua(), -1 )
        ;
      lua_pop( lua(), 1 );
    }
    else {
      // no return value
    }
  }


}

void Control::Detach( mapScript_t::iterator iterScript ) {
  //BOOST_LOG_TRIVIAL(info) << "Detach";
  Lua& lua( iterScript->second );
  //lua_State* pLua( script.pLua );

  lua_getglobal( lua(), "detach" ); // page 242 of 2016 Programming in Lua
  lua_pushlightuserdata( lua() , this );

  int result = lua_pcall( lua(), 1, 0, 0 );
  if ( LUA_OK != result ) {
    BOOST_LOG_TRIVIAL(error)
      << "ScriptLua::Detach failed to run script: "
      << lua_tostring( lua(), -1 )
      ;
    lua_pop( lua(), 1 );
  }
  else {
    // no return value
  }
}

} // namespace lua