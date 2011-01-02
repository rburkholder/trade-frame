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

#include "FieldDef.h"

namespace ou {
namespace db {

class SqlInsert: public Action_AddFields {
public:

  SqlInsert( void );
  ~SqlInsert( void );

  void ComposeStatement( const std::string& sTableName, std::string& sStatement );

protected:
private:
};

} // db
} // ou
