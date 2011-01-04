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

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace ou {
namespace db {

// Action_AddFields

class Action_AddFields {
public:

  Action_AddFields( void );
  virtual ~Action_AddFields( void );

  void registerField( const std::string& sField, const char* szDbFieldType );

  // inheritor will need one of two:
  // void ComposeStatement( const std::string& sTableName, std::string& sStatement );
  // void ComposeStatement( std::string& sStatement );

protected:

  // definition of fields
  struct structFieldDef {
    std::string sFieldName;
    std::string sFieldType;
    bool bIsKeyPart;
    structFieldDef( void ): bIsKeyPart( false ) {};
    structFieldDef(const std::string& sFieldName_, const std::string& sFieldType_ ) 
      : bIsKeyPart( false ), sFieldName( sFieldName_ ), sFieldType( sFieldType_ ) {};
  };

  typedef std::vector<structFieldDef> vFields_t;
  typedef vFields_t::iterator vFields_iter_t;
  vFields_t m_vFields;

private:
};

} // db
} // ou
