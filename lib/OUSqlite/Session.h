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

#include <OUSQL/SessionImpl.h>

#include "ISqlite3.h"

namespace ou {
namespace db {

class CSession: public CSessionImpl<ISqlite3>, public SessionBase<CSessionImpl<ISqlite3>, CSession> {
public:
  typedef boost::shared_ptr<CSession> pSession_t;
  CSession( void ) {};
  ~CSession( void ) {};
protected:
private:
};


} // db
} // ou
