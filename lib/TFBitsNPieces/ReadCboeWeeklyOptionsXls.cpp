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

#include <boost/date_time/gregorian/gregorian.hpp>

#include <boost/lexical_cast.hpp>

#include <ExcelFormat/ExcelFormat.h>

#include "ReadCboeWeeklyOptionsXls.h"

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

// "yyyymmdd"
boost::gregorian::date ConvertDate( const std::string& sDate ) {
  boost::gregorian::date::year_type year = boost::lexical_cast<int>( sDate.substr( 0, 4 ) );
  boost::gregorian::date::month_type month = boost::lexical_cast<int>( sDate.substr( 4, 2 ) );
  boost::gregorian::date::day_type day = boost::lexical_cast<int>( sDate.substr( 6, 2 ) );
  return boost::gregorian::date( year, month, day );
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
  return true;  // what should be returned?
}

void ExtractDates( ExcelFormat::BasicExcelWorksheet* sheet, int row, int col, vOptionExpiryDates_t& v ) {
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
  //assert( 0 < v.size() );  // standard doesn't have dates
}

/*  xls2csv .... beginning of file
"LIST OF AVAILABLE WEEKLY OPTIONS (updated 05-18-16)",,,,,,,,
,,"Standard Weeklys Available Expirations:",,
,,"Expanded Weeklys Available Expirations:","42517","42524","42531","42545","42552"
,,"End of Week (EOW) Options Available Expirations:","42517","42524","42531","42545","42552"
,,"SPX & XSP (EOW) Options Available Expirations:","42517","42524","42531","42545","42552","42559","42573",,,
,,"SPX Wednesday Weeklys (WW) Available Expirations:","42515","42522","42529",,,,,,,
,,"VIX Weekly Options","42515","42522","42529","42543",,,,,,,
,,"Weeklys Deleted from the Program:",,,,,,,
,,,,,,,,
"Ticker ","New?","Name","Product Type","List Date","Standard Weekly","Expanded Weekly","EOW","WW","VIX"
"OEX",,"S&P 100 Index (American style)","Index, pm-settled, cash","20160512",,,"X",,

 */

// provide legacy means of access
void ReadCboeWeeklyOptions( OptionExpiryDates_t& expiries, vUnderlyinginfo_t& vui ) {
  ReadCboeWeeklyOptions(
    expiries,
    [&vui](const UnderlyingInfo& ui){
      vui.push_back( std::move( ui ) );
  } );
}

// newer means of access
void ReadCboeWeeklyOptions( OptionExpiryDates_t& expiries, fUnderlyingInfo_t fUnderlyingInfo ) {
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

    try {

      // doesn't have dates, maybe copy from expanded
      Confirm( "Standard Weeklys Available Expirations:",          sheet->Cell( 1, 2 ) );
      ExtractDates( sheet, 1, 3, expiries.vExpiriesStandardWeeklies );

      Confirm( "Expanded Weeklys Available Expirations:",          sheet->Cell( 2, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesExpandedWeeklies );

      Confirm( "End of Week (EOW) Options Available Expirations:", sheet->Cell( 3, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesEndOfWeek );

      Confirm( "SPX & XSP (EOW) Options Available Expirations:",  sheet->Cell( 4, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesSpxwXsp );

      Confirm( "XSP Wednesday Weeklys (W) Available Expirations:",  sheet->Cell( 5, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesXspWednesday );

      Confirm( "SPX Monday & Wednesday Weeklys (M,W) Available Expirations:",  sheet->Cell( 6, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesSpxMonWed );

      Confirm( "Monday Equity/ETF/ETN Weeklys (M) Available Expirations:",  sheet->Cell( 7, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesEtfMonday );

      Confirm( "Wednesday Equity/ETF/ETN Weeklys (W) Available Expirations:",  sheet->Cell( 8, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesEtfWedndesday );

      Confirm( "VIX Weekly Options",                               sheet->Cell( 9, 2 ) );
      ExtractDates( sheet, 2, 3, expiries.vExpiriesVixWeeklies );

      Confirm( "Weeklys Deleted from the Program:",                sheet->Cell( 10, 2 ) );

      Confirm( "Ticker ",                                          sheet->Cell( 12, 0 ) );  // adjust ixRow if this row changes

    }
    catch (...) {
      std::cout << "confirm issue" << std::endl;
    }


    if ( 0 == expiries.vExpiriesStandardWeeklies.size() ) {
      expiries.vExpiriesStandardWeeklies = expiries.vExpiriesExpandedWeeklies;
    }

    int ixRow( 13 );  // based upon where Ticker ends
    int ixCol(  0 );

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
              if ( ExcelFormat::BasicExcelCell::DOUBLE == cell->Type() ) {
                ui.dateListed = ConvertDate( cell->GetDouble() );
              }
              else {
                if ( ExcelFormat::BasicExcelCell::STRING == cell->Type() ) {
                  ui.dateListed = ConvertDate( cell->GetString() );
                }
                else {
                  if ( ExcelFormat::BasicExcelCell::INT == cell->Type() ) {
                    ui.dateListed = ConvertDate( cell->GetInteger() );
                  }
                  else {
                    if ( ExcelFormat::BasicExcelCell::UNDEFINED == cell->Type() ) {
                      // don't do anything, current spreadsheets have empty cell
                    }
                    else {
                      bProcess = false;
                    }

                  }
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
        case ExcelFormat::BasicExcelCell::WSTRING:
          std::cout << "row: " << ixRow << " is WSTRING" << std::endl;
          break;
        case ExcelFormat::BasicExcelCell::UNDEFINED:
          bProcess = false;
          break;
        default:
          bProcess = false;
          break;
      }
      if ( bProcess && !ui.sSymbol.empty() ) {
        //std::cout << "fUnderlyingInfo: " << ui.sSymbol << std::endl;
        fUnderlyingInfo( ui );  // push only if successfully parsed.
      }
      ++ixRow;
    }

    std::cout << "Last Row: " << ixRow << std::endl;
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
