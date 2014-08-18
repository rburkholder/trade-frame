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

//#include "stdafx.h"

#include <boost/lexical_cast.hpp>

#include "ReadSicToNaicsCodeList.h"

namespace ou { // One Unified

ReadSicToNaicsCodeList::ReadSicToNaicsCodeList( const std::string& sFileName ) {

  ExcelFormat::BasicExcel xls;
  if ( !xls.Load( sFileName.c_str() ) ) {
    throw std::runtime_error( "file read issue" );
  }
  else {

    ExcelFormat::BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
    int x = sheet->GetTotalCols();
    int y = sheet->GetTotalRows();

    std::cout << "processing " << x << "x" << y << std::endl;

    // skip title, skip column headers, skip first column

    bool bProcess( true );
    int ixRox = 2;

    std::string sKey, sDesc;

    while ( bProcess ) {
      structBoth rec;
      std::string sTmp;
/*
      Extract( sTmp, sheet, ixRox, 0 );
      rec.nSic = boost::lexical_cast<boost::uint32_t>( sTmp );
      Extract( rec.sSicDesc , sheet, ixRox, 1 );

      Extract( sTmp, sheet, ixRox, 2 );
      rec.nNaics = boost::lexical_cast<boost::uint32_t>( sTmp );
      Extract( rec.sNaicsDesc, sheet, ixRox, 3 );

      vLU_t::size_type n = m_vLU.size();
      m_vLU.push_back( rec );
      m_mapLuSic.insert( pairLU_t( rec.nSic, n ) );
      m_mapLuNaics.insert( pairLU_t( rec.nNaics, n ) );
*/
      ixRox++;
    }
  }

  std::cout << "vector size " << m_vLU.size() << std::endl;

}

ReadSicToNaicsCodeList::~ReadSicToNaicsCodeList(void) {
}

} // namespace ou