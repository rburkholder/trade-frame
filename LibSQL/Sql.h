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

#include "IDatabase.h"

namespace ou {
namespace db {

//
// =====
//

class CSqlBase {
public:

  typedef boost::shared_ptr<CSqlBase> pCSqlBase_t;

  CSqlBase(void);
  virtual ~CSqlBase(void);

  void PrepareStatement( void );

protected:

  virtual void ComposeStatement( std::string& sStatement );

private:
  bool m_bPrepared;
  std::string m_sSqlStatement;
};

//
// =====
//

template<typename F>  // f: Field definitions
class CSql: public CSqlBase {
public:

  typedef typename boost::shared_ptr<CSql> pCSql_t;

  CSql(void);
  ~CSql(void);

protected:
private:
};

template<typename F>
CSql<F>::CSql( void ): CSqlBase() {
}

template<typename F>
CSql<F>::~CSql( void ) {
}

} // db
} // ou
