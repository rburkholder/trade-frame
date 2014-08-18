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

// started 2014/08/17

#include <map>
#include <vector>
#include <string>

#include <boost/cstdint.hpp>

#include <ExcelFormat/ExcelFormat.h>

class ReadCodeListCommon {
public:

  ReadCodeListCommon(void);
  ~ReadCodeListCommon(void);

  const std::string& LookupSIC( boost::uint32_t );
  const std::string& LookupNAICS( boost::uint32_t );

protected:

  struct structItem {
    boost::uint32_t id;
    std::string sDesc;
    structItem( void ): id ( 0 ) {};
  };

  struct structBoth {
    structItem sic;
    structItem naics;
  };

  typedef std::map<boost::uint32_t, std::string> mapItem_t;
  typedef std::pair<boost::uint32_t, std::string> pairItem_t;

  mapItem_t m_mapSic;
  mapItem_t m_mapNaics;

  typedef std::vector<structBoth> vLU_t;

  vLU_t m_vLU;

  typedef std::map<boost::uint32_t, vLU_t::size_type> mapLU_t;
  typedef std::pair<boost::uint32_t, vLU_t::size_type> pairLU_t;

  //mapLU_t m_mapLuSic;
  //mapLU_t m_mapLuNaics;

  bool Extract( std::string& s, ExcelFormat::BasicExcelWorksheet* sheet, int row, int col );
  bool Extract( double& dbl, ExcelFormat::BasicExcelWorksheet* sheet, int row, int col );

private:
};

