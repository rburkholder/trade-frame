/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

// LibSqlite/Actions.h

#pragma once

#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <LibSQL\Actions.h>

namespace ou {
namespace db {
namespace sqlite {

class Action_AddFields: public ou::db::Action_AddFields {
public:

  const char* FieldType( char key );
  const char* FieldType( bool key );
  const char* FieldType( boost::int64_t key );
  const char* FieldType( boost::int32_t key );
  const char* FieldType( boost::int16_t key );
  const char* FieldType( boost::int8_t key );
  const char* FieldType( std::string& key );
  const char* FieldType( double key );
  const char* FieldType( boost::posix_time::ptime& key ); // don't use julian as ptime has no representation earlier than 1400 AD

  Action_AddFields( void ) {};
  ~Action_AddFields( void ) {};

  template<typename T>
  void registerField( const std::string& sFieldName, T& var ) {
    addField( sFieldName, FieldType( var ) );
  }

  template<typename T>
  void registerField( const std::string& sFieldName, T& var, const std::string& sFieldType ) {
    addField( sFieldName, sFieldType.c_str() );
  }

protected:
private:
};

} // sqlite
} // db
} // ou
