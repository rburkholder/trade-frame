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

#include <boost\shared_ptr.hpp>

namespace ou {
namespace db {

class CPreparedStatement {
public:

  typedef boost::shared_ptr<CPreparedStatement> pCPreparedStatement_t;

  CPreparedStatement(void);
  CPreparedStatement( const std::string& sSqlStatement );
  ~CPreparedStatement(void);

  void SetQueryString( const std::string& sSqlStatement );

protected:
private:
  bool m_bPrepared;
  std::string m_sSqlStatement;
};


} // db
} // ou
