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

#include "StdAfx.h"

#include "FieldDef.h"

namespace ou {
namespace db {

Action_AddFields::Action_AddFields( void ) 
{
}

Action_AddFields::~Action_AddFields( void ) {
}

void Action_AddFields::addField( const std::string& sField, const char* szDbFieldType ) {
  structFieldDef fd( sField, szDbFieldType );
  m_vFields.push_back( fd );
}

  

} // db
} // ou

