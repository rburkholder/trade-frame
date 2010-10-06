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

#include <LibSqlite/sqlite3.h>

class CTradingDb
{
public:

  CTradingDb( const char* szDbFileName );
  ~CTradingDb(void);

  sqlite3* GetDb( void ) { return m_pdbTrading; };

protected:
private:
  sqlite3* m_pdbTrading;
};

