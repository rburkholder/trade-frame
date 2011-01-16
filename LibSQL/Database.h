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

// inherited by ISqlite3 and IPostgresql for specialization

#pragma once

#include <string>
#include <list>

namespace ou {
namespace db {

enum enumOpenFlags { 
  EOpenFlagsZero = 0, 
  EOpenFlagsAutoCreate = 0x1 
};

class Database {
public:

  Database(void);
  virtual ~Database(void);

  virtual void Open( const std::string& sDbFileName, enumOpenFlags = EOpenFlagsZero ) {};
  virtual void Close( void ) {};

protected:

  bool m_bDbOpened;

  std::string m_sDbFileName;

private:
};

} // db
} // ou
