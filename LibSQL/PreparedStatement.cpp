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

#include "PreparedStatement.h"

namespace ou {
namespace db {

CPreparedStatement::CPreparedStatement(void) 
  : m_bPrepared( false )
{
}

CPreparedStatement::CPreparedStatement( const std::string& sSqlStatement ) 
  : m_sSqlStatement( sSqlStatement ), m_bPrepared( false )
{
}

CPreparedStatement::~CPreparedStatement(void) {
}

void CPreparedStatement::SetQueryString( const std::string& sSqlStatement ) {
  m_sSqlStatement = sSqlStatement;
  m_bPrepared = false;
}

} // db
} // ou
