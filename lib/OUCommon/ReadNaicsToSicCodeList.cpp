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

#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "ReadNaicsToSicCodeList.h"

namespace ou { // One Unified

ReadNaicsToSicCodeList::ReadNaicsToSicCodeList( const std::string& sFileName ) {
  ExcelFormat::BasicExcel xls;
  if ( !xls.Load( sFileName.c_str() ) ) {
    throw std::runtime_error( "file read issue" );
  }
  else {

    ExcelFormat::BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
    int nRows = sheet->GetTotalRows();
    int nCols = sheet->GetTotalCols();

    std::cout << "processing " << nRows << "x" << nCols << std::endl;

    // skip title, skip column headers, skip first column

    //bool bProcess( true );
    int ixRow = 2;

    std::string sKey, sDesc;

    struct ErrLoc0: public std::exception  {};
    struct ErrLoc1: public std::exception  {};
    struct ErrLoc2: public std::exception  {};
    struct ErrLoc3: public std::exception  {};

    while ( ixRow < nRows ) {

      //structRecord rec;
      std::string sTmp;
      double dbl;
      boost::uint32_t id;
      bool b;

      try {

        b = Extract( dbl, sheet, ixRow, 0 );
        if ( !b ) throw( ErrLoc0() );
        id = boost::lexical_cast<boost::uint32_t>( dbl );
        b = Extract( sDesc , sheet, ixRow, 1 );
        if ( !b ) throw( ErrLoc1() );

        m_mapNaics.insert( pairItem_t( dbl, sDesc ) );

      }
      catch( ErrLoc0& e ) {
      }
      catch( ErrLoc1& e ) {
      }
      catch (...) {
      }
        
      try {

        b = Extract( dbl, sheet, ixRow, 2 );
        if ( !b ) throw( ErrLoc2() );
        id = boost::lexical_cast<boost::uint32_t>( dbl );
        b = Extract( sDesc, sheet, ixRow, 3 );
        if ( !b ) throw( ErrLoc3() );

        m_mapSic.insert( pairItem_t( dbl, sDesc ) );

      }

      catch( ErrLoc2& e ) {
      }
      catch( ErrLoc3& e ) {
      }
      catch (...) {
      }

//      vLU_t::size_type n = m_vLU.size();
//          m_vLU.push_back( rec );
//          m_mapLuSic.insert( pairLU_t( rec.nSic, n ) );
//          m_mapLuNaics.insert( pairLU_t( rec.nNaics, n ) );
//        }
//      }

      ixRow++;
    }
  }

//  std::cout << "vector size " << m_vLU.size() << std::endl;

}

ReadNaicsToSicCodeList::~ReadNaicsToSicCodeList(void) {
}

} // namespace ou