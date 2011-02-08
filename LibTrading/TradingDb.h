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
#include <stdexcept>

namespace ou { // One Unified
namespace tf { // TradeFrame

//class CTradingDb: public ou::db::CSession
class CTradingDb
{
public:

  CTradingDb( const char* szDbFileName );
  ~CTradingDb(void);

protected:
private:
};

} // namespace tf
} // namespace ou
