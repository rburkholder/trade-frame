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

bool confirm( const std::string& s, ExcelFormat::BasicExcelCell* cell ) {
  if ( ExcelFormat::BasicExcelCell::STRING != cell->Type() ) throw std::runtime_error( "not confirmed 1" );
  std::string ss( cell->GetString() );
  if ( s != ss ) throw std::runtime_error( "not confirmed 2" );
}

/*  xls2csv .... beginning of file
 "LIST OF AVAILABLE WEEKLY OPTIONS (updated 10-29-15)",,,,,,,,
,,"Standard Weeklys Available Expirations:","42307","42314"
,,"Expanded Weeklys Available Expirations:","42307","42314","42321","42335","42342","42349"
,,"End of Week (EOW) Options Available Expirations:","42307","42314","42321","42335","42342","42349"
,,"SPXW & XSP (EOW) Options Available Expirations:","42314","42321","42335","42342","42349","42362","42377","42391",,,,
,,"VIX Weekly Options","42312","42319","42332",,,,,,,,,
,,"Weeklys Deleted from the Program:",,,,,,,
,,,,,,,,
"Ticker ","New?","Name ( * Indicates weekly also trades on C2)","Product Type","List Date","Standard Weekly","Expanded Weekly","EOW",
"OEX",,"S&P 100 Index (American style)","Index, pm-settled, cash","20151029",,,"X",
 */

void ReadCboeWeeklyOptions( vExpiries_t& vExpiries, vUnderlyinginfo_t& vui ) {
  ExcelFormat::BasicExcel xls;
  if ( !xls.Load( "../weeklysmf.xls" ) ) {
    throw std::runtime_error( "file read issue" );
  }
  else {

    ExcelFormat::BasicExcelWorksheet* sheet = xls.GetWorksheet(0);
    int cntCol = sheet->GetTotalCols();
    int cntRow = sheet->GetTotalRows();
    std::cout << "Columns: " << cntCol << ", Rows: " << cntRow << std::endl;

    ExcelFormat::BasicExcelCell* cell; 

    cell = sheet->Cell( 0, 0 );  // LIST OF AVAILABLE WEEKLY OPTIONS
    if ( ExcelFormat::BasicExcelCell::STRING != cell->Type() ) throw std::runtime_error( "not found 1" );
    
    confirm( "Standard Weeklys Available Expirations:",          sheet->Cell( 1, 2 ) );
    confirm( "Expanded Weeklys Available Expirations:",          sheet->Cell( 2, 2 ) );
    confirm( "End of Week (EOW) Options Available Expirations:", sheet->Cell( 3, 2 ) );
    confirm( "SPXW & XSP (EOW) Options Available Expirations:",  sheet->Cell( 4, 2 ) );
    confirm( "VIX Weekly Options",                               sheet->Cell( 5, 2 ) );
    confirm( "Weeklys Deleted from the Program:",                sheet->Cell( 6, 2 ) );
    confirm( "Ticker ",                                          sheet->Cell( 7, 0 ) );
    
    int ixRow( 9 );
    int ixCol( 0 );
    
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
