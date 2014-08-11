/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// Started 2014/08/10

#include <map>
#include <string>

#include <boost/cstdint.hpp>

namespace ou { // One Unified

class SicCodeList {
public:
  SicCodeList( const std::string& sFileName );
  ~SicCodeList(void);

  const std::string& LookupCode( boost::uint32_t );
protected:
private:

  typedef std::map<boost::uint32_t, std::string> mapSic_t;

  mapSic_t m_mapSic;

};

} // namespace ou