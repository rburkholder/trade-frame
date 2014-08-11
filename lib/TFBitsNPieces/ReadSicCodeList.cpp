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

#include "StdAfx.h"

#include <boost/lexical_cast.hpp>

#include <ExcelFormat/ExcelFormat.h>

#include "ReadSicCodeList.h"

namespace ou { // One Unified

SicCodeList::SicCodeList( const std::string& sFileName ) {

  ExcelFormat::BasicExcel xls;
  if ( !xls.Load( sFileName.c_str() ) ) {
    throw std::runtime_error( "file read issue" );
  }
  else {

    ExcelFormat::BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
    int x = sheet->GetTotalCols();
    int y = sheet->GetTotalRows();

    ExcelFormat::BasicExcelCell* cell;

    // skip title, skip column headers, skip first column

    bool bProcess( true );
    int iy = 2;

    std::string sKey, sDesc;

    while ( bProcess ) {
      cell = sheet->Cell( iy, 1 );
      switch( cell->Type() ) {
      case ExcelFormat::BasicExcelCell::STRING:
        sKey = cell->GetString();
        cell = sheet->Cell( iy, 2 );
        sDesc = cell->GetString();
        m_mapSic.insert( std::pair<boost::uint32_t, std::string>( boost::lexical_cast<boost::uint32_t>( sKey ), sDesc ) );
        break;
      case ExcelFormat::BasicExcelCell::UNDEFINED:
      default:
        bProcess = false;
        break;
      }
      iy++;
    }
  }

  std::cout << "map size " << m_mapSic.size() << std::endl;

}

SicCodeList::~SicCodeList(void) {
}

const std::string& SicCodeList::LookupCode( boost::uint32_t key ) {
  static const std::string s( "" );
  mapSic_t::const_iterator iter = m_mapSic.find( key );
  return ( m_mapSic.end() == iter ) ? s : iter->second;
}

} // namespace ou