/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/09/21

#include "stdafx.h"

#include <ExcelFormat/ExcelFormat.h>

#include "ReadCboeWeeklyOptions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace cboe {

bool AssignCellContent( const ExcelFormat::BasicExcelCell* cell, std::string& s ) {
  bool bProcess( true );
  if ( ExcelFormat::BasicExcelCell::UNDEFINED == cell->Type() ) {
    bProcess = false;
  }
  else {
    if ( ExcelFormat::BasicExcelCell::STRING != cell->Type() ) {
      bProcess = false;
    }
    else {
      s = cell->GetString();
    }
  }
  return bProcess;
}

bool AssignCellContent( const ExcelFormat::BasicExcelCell* cell, bool& b ) {
  bool bProcess( true );
  if ( ExcelFormat::BasicExcelCell::UNDEFINED == cell->Type() ) {
  }
  else {
    if ( ExcelFormat::BasicExcelCell::STRING != cell->Type() ) {
      bProcess = false;
    }
    else {
      std::string s = cell->GetString();
      if ( "X" == s ) b = true;
    }
  }
  return bProcess;
}

boost::gregorian::date ConvertDate( int intDate ) {
  boost::gregorian::date::day_type day = intDate % 100;
  boost::gregorian::date::month_type month = ( ( intDate - day ) / 100 ) %100;
  boost::gregorian::date::year_type year =  ( intDate - month ) / 100;
  return boost::gregorian::date( year, month, day );
}

void ReadCboeWeeklyOptions( vExpiries_t& vExpiries, vUnderlyinginfo_t& vui ) {
  ExcelFormat::BasicExcel xls;
  if ( !xls.Load( "weeklysmf.xls" ) ) {
    throw std::runtime_error( "file read issue" );
  }
  else {

    ExcelFormat::BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
    int x = sheet->GetTotalCols();
    int y = sheet->GetTotalRows();

    ExcelFormat::BasicExcelCell* cell;

    // skip: LIST OF AVAILABLE WEEKLYS OPTIONS (updated 09-19-2013)			
    // skip: LIST OF AVAILABLE WEEKLY EXPIRATIONS IN COLUMNS E THRU J 
    cell = sheet->Cell( 2, 0 );
    if ( ExcelFormat::BasicExcelCell::STRING == cell->Type() ) {
      std::string s( sheet->Cell( 2, 0 )->GetString() );
      if ( "Ticker Symbol" == s ) {
        for ( int ix = 4; ix <= 9; ++ix ) {
          cell = sheet->Cell( 2, ix );
          if ( ExcelFormat::BasicExcelCell::INT == cell->Type() ) {
            vExpiries.push_back( ConvertDate( cell->GetInteger() ) );
          }
        }
        int iy = 3; // beginning row of info.
        bool bProcess( true );
        while ( bProcess ) {
          UnderlyingInfo ui;
          cell = sheet->Cell( iy, 0 );
          switch( cell->Type() ) {
            case ExcelFormat::BasicExcelCell::STRING:
              ui.sSymbol = cell->GetString();
              for ( int ix = 1; ix <= 9; ++ix ) {
                cell = sheet->Cell( iy, ix );
                switch ( ix ) {
                case 1: bProcess = AssignCellContent( cell, ui.sDescription );
                  break;
                case 2: bProcess = AssignCellContent( cell, ui.sProductType );
                  break;
                case 3: 
                    if ( ExcelFormat::BasicExcelCell::UNDEFINED == cell->Type() ) {
                      bProcess = false;
                    } 
                    else {
                      if ( ExcelFormat::BasicExcelCell::INT != cell->Type() ) {
                        bProcess = false;
                      }
                      else {
                        ui.dateInitialList = ConvertDate( cell->GetInteger() );
                      }
                    }
                  break;
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                  bProcess = AssignCellContent( cell, ui.rbExpires[ ix - 4 ] );
                  break;
                }
              }
              break;
            case ExcelFormat::BasicExcelCell::UNDEFINED:
              bProcess = false;
              break;
            default:
              bProcess = false;
              break;
          }
          if ( bProcess ) vui.push_back( ui );  // push only if successfully parsed.
          ++iy;
        }
      }
    }

/*    
    for ( int iy = 0; iy < y; ++iy ) {
      for ( int ix = 0; ix < x; ++ix ) {
        cell = sheet->Cell( iy, ix );
        if ( 0 != ix ) std::cout << ",";
        switch ( cell->Type() ) {
        case ExcelFormat::BasicExcelCell::UNDEFINED:
          std::cout << "1:N/A";
          break;
        case ExcelFormat::BasicExcelCell::INT:
          std::cout << cell->GetInteger();
          break;
        case ExcelFormat::BasicExcelCell::DOUBLE:
          std::cout << cell->GetDouble();
          break;
        case ExcelFormat::BasicExcelCell::STRING:
          std::cout << "'" << cell->GetString() << "'";
          break;
        case ExcelFormat::BasicExcelCell::WSTRING:
          std::cout << cell->GetWString();
          break;
        case ExcelFormat::BasicExcelCell::FORMULA:
          std::cout << "2:N/A";
          break;
        }
      }
      std::cout << std::endl;
      if ( 11 < iy ) break;
    }
    */
    
    xls.Close();
  }
}

} // namespace cboe  
} // namespace tf
} // namespace ou
