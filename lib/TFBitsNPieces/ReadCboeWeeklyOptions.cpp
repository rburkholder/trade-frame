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

#include <boost/date_time/gregorian/gregorian.hpp>

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

boost::gregorian::date ConvertDate( double dblDate ) {
  //boost::gregorian::date::day_type day = intDate % 100;
  //boost::gregorian::date::month_type month = ( ( intDate - day ) / 100 ) %100;
  //boost::gregorian::date::year_type year =  ( intDate - month ) / 100;
  boost::gregorian::date base( 1899, 12, 30 );  // excel base date
  //boost::gregorian::date::duration_type dur( intDate );
  boost::gregorian::date_duration dur( (int) dblDate );
  base = base + dur;
  //boost::gregorian::date::day_type day = base.day_number();
  //boost::gregorian::date::month_type month = base.month();
  //boost::gregorian::date::year_type year = base.year();
  try {
    //std::cout << base << std::endl;
  }
  catch (...) {
    //std::cout << "date failure" << std::endl;
  }
  return base;
}

bool Confirm( const std::string& s, ExcelFormat::BasicExcelCell* cell ) {
  if ( ExcelFormat::BasicExcelCell::STRING != cell->Type() ) throw std::runtime_error( "not confirmed 1" );
  std::string ss( cell->GetString() );
  if ( s != ss ) throw std::runtime_error( "not confirmed 2" );
}

void ExtractDates( ExcelFormat::BasicExcelWorksheet* sheet, int row, int col, vExpiries_t& v ) {
  assert( 0 != sheet );
  assert( 0 == v.size() );
  ExcelFormat::BasicExcelCell* cell;
  bool bProcess( true );
  while ( bProcess ) {
    cell = sheet->Cell( row, col );
    switch ( cell->Type() ) {
      case ExcelFormat::BasicExcelCell::INT: 
	v.push_back( ConvertDate( cell->GetInteger() ) );
	break;
      case ExcelFormat::BasicExcelCell::DOUBLE:
	v.push_back( ConvertDate( cell->GetDouble() ) );
	break;
      case ExcelFormat::BasicExcelCell::STRING:
	std::cout << "string: " << cell->GetString() << std::endl;
	break;
      default:
	bProcess = false;
	break;
    }
    ++col;
  }
  assert( 0 < v.size() );
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

void ReadCboeWeeklyOptions( Expiries_t& Expiries, vUnderlyinginfo_t& vui ) {
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
    
    Confirm( "Standard Weeklys Available Expirations:",          sheet->Cell( 1, 2 ) );
    ExtractDates( sheet, 1, 3, Expiries.vExpiriesStandardWeeklies );
    
    Confirm( "Expanded Weeklys Available Expirations:",          sheet->Cell( 2, 2 ) );
    ExtractDates( sheet, 2, 3, Expiries.vExpiriesExpandedWeeklies );
    
    Confirm( "End of Week (EOW) Options Available Expirations:", sheet->Cell( 3, 2 ) );
    ExtractDates( sheet, 2, 3, Expiries.vExpiriesEndOfWeek );
    
    Confirm( "SPXW & XSP (EOW) Options Available Expirations:",  sheet->Cell( 4, 2 ) );
    ExtractDates( sheet, 2, 3, Expiries.vExpiriesSpxwXsp );
    
    Confirm( "VIX Weekly Options",                               sheet->Cell( 5, 2 ) );
    ExtractDates( sheet, 2, 3, Expiries.vExpiriesVixWeeklies );
    
    Confirm( "Weeklys Deleted from the Program:",                sheet->Cell( 6, 2 ) );
    
    Confirm( "Ticker ",                                          sheet->Cell( 8, 0 ) );
    
    int ixRow( 9 );
    int ixCol( 0 );
    
    bool bProcess( true );
    while ( bProcess ) {
      UnderlyingInfo ui;
      cell = sheet->Cell( ixRow, 0 );
      switch( cell->Type() ) {
	case ExcelFormat::BasicExcelCell::STRING:
	  ui.sSymbol = cell->GetString();
	  //std::cout << ixRow << ": " << ui.sSymbol << std::endl;
	  for ( int ixCol = 1; ixCol <= 7; ++ixCol ) {
	    cell = sheet->Cell( ixRow, ixCol );
	    switch ( ixCol ) {
	    case 1:
	      switch ( cell->Type() ) {
		case ExcelFormat::BasicExcelCell::STRING:
		  ui.bAdded = "" != cell->GetString();
		  break;
		case ExcelFormat::BasicExcelCell::UNDEFINED:
		  break;
		default:
		  std::cout << "row: " << ixRow << ", col: " << ixCol << " - weird cell " << cell->Type() << std::endl;
	      }
	      break;
	    case 2: bProcess = AssignCellContent( cell, ui.sDescription );
	      break;
	    case 3: 
	      bProcess = AssignCellContent( cell, ui.sProductType );
	      break;
	    case 4: 
		if ( ExcelFormat::BasicExcelCell::UNDEFINED == cell->Type() ) {
		  bProcess = false;
		} 
		else {
		  if ( ExcelFormat::BasicExcelCell::INT != cell->Type() ) {
		    bProcess = false;
		  }
		  else {
		    ui.dateListed = ConvertDate( cell->GetInteger() );
		  }
		}
	      break;
	    case 5:
	      switch ( cell->Type() ) {
		case ExcelFormat::BasicExcelCell::STRING:
		  ui.bStandardWeekly = 0 == strcmp( "X", cell->GetString() );
		  //ui.bStandardWeekly = "X" == cell->GetString();
		  break;
		case ExcelFormat::BasicExcelCell::UNDEFINED:
		  break;
		default:
		  std::cout << "row: " << ixRow << ", col: " << ixCol << " - weird cell " << cell->Type() << std::endl;
	      }
	      break;
	    case 6:
	      switch ( cell->Type() ) {
		case ExcelFormat::BasicExcelCell::STRING:
		  ui.bExpandedWeekly = 0 == strcmp( "X", cell->GetString() );
		  //ui.bExpandedWeekly = "X" == cell->GetString();
		  break;
		case ExcelFormat::BasicExcelCell::UNDEFINED:
		  break;
		default:
		  std::cout << "row: " << ixRow << ", col: " << ixCol << " - weird cell " << cell->Type() << std::endl;
	      }
	      break;
	    case 7:
	      switch ( cell->Type() ) {
		case ExcelFormat::BasicExcelCell::STRING:
		  ui.bEOW = 0 == strcmp( "X", cell->GetString() );
		  //ui.bEOW = "X" == cell->GetString();
		  break;
		case ExcelFormat::BasicExcelCell::UNDEFINED:
		  break;
		default:
		  std::cout << "row: " << ixRow << ", col: " << ixCol << " - weird cell " << cell->Type() << std::endl;
	      }
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
      ++ixRow;
    }
  } // else
    
    xls.Close();
}

} // namespace cboe  
} // namespace tf
} // namespace ou



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
