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

#include "stdafx.h"

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <TFBitsNPieces/ReadCboeWeeklyOptions.h>
#include <TFBitsNPieces/InstrumentFilter.h>

#include "SignalGenerator.h"

SignalGenerator::SignalGenerator(void) 
  : m_fmt_mgr( m_xls)
{
}

SignalGenerator::~SignalGenerator(void) {
}

void SignalGenerator::Run( void ) {

  ou::tf::cboe::OptionExpiryDates_t expiries;
  ou::tf::cboe::vUnderlyinginfo_t vui;

  try {
    ou::tf::cboe::ReadCboeWeeklyOptions( expiries, vui );
  }
  catch(...) {
  }

  typedef ou::tf::cboe::vUnderlyinginfo_t::const_iterator vUnderlyinginfo_citer_t;

  for ( vUnderlyinginfo_citer_t iter = vui.begin(); vui.end() != iter; ++iter ) {
//    std::cout << 
//	    iter->sSymbol
//	    << "," << iter->bAdded
//	    << "," << iter->bStandardWeekly
//	    << "," << iter->bExpandedWeekly
//	    << "," << iter->bEOW
//	    << "," << iter->sProductType
//	    << "," << iter->sDescription
//	    << std::endl;
	    
//    if ( ( "Equity" == iter->sProductType ) || ( "ETF" == iter->sProductType ) ) { 
      //ScanBars( iter->sSymbol );
      BarSummary bs;
      bs.sType = iter->sProductType;
      m_mapSymbol.insert( mapSymbol_t::value_type( iter->sSymbol, bs ) );
//    }
  }

  if ( 0 != m_mapSymbol.size() ) {
    ScanBars();
  }
  
  std::cout << "SignalGenerator Complete" << std::endl;
  
}

void SignalGenerator::ScanBars( void ) {
  namespace args = boost::phoenix::placeholders;
  ou::tf::InstrumentFilter<mapSymbol_t::iterator,ou::tf::Bars> filter( 
    "/bar/86400",  // at least a year's worth of bars
    ptime( date( 2015,  1,  1 ), time_duration( 0, 0, 0 ) ),
    ptime( date( 2016,  6, 21 ), time_duration( 0, 0, 0 ) ), 
    200,
    boost::phoenix::bind( &SignalGenerator::HandleCallBackUseGroup, this, args::arg1, args::arg2, args::arg3 ),
    boost::phoenix::bind( &SignalGenerator::HandleCallBackFilter, this, args::arg1, args::arg2, args::arg3 ),
    boost::phoenix::bind( &SignalGenerator::HandleCallBackResults, this, args::arg1, args::arg2, args::arg3 )
    );
  try {

    m_xls.New( 1 );
    m_sheet = m_xls.GetWorksheet( 0 );
    ExcelFormat::BasicExcelCell* cell;
    ExcelFormat::CellFormat fmt(m_fmt_mgr);
    fmt.set_format_string( XLS_FORMAT_DECIMAL );
    fmt.set_alignment( ExcelFormat::EXCEL_HALIGN_CENTRED );
    int ix( 0 );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "Type" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "Symbol" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "Price" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "BolRange" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA Sig" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "BolUpr" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "BolLwr" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA1" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA2" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA3" );

    filter.Run();

    m_xls.SaveAs( "weeklies.xls" );
    m_xls.Close();  }

  catch( ... ) {
    std::cout << "Scan Problems" << std::endl;
  }
}

bool SignalGenerator::HandleCallBackUseGroup( mapSymbol_t::iterator& iter, const std::string& sPath, const std::string& sGroup ) {
  return true;
}

bool SignalGenerator::HandleCallBackFilter( mapSymbol_t::iterator& iter, const std::string& sObject, ou::tf::Bars& bars ) {
  iter = m_mapSymbol.find( sObject );
  return m_mapSymbol.end() != iter;
}

void SignalGenerator::HandleCallBackResults( mapSymbol_t::iterator& iter, const std::string& sObject, ou::tf::Bars& bars ) {
  // process bars here
  for ( ou::tf::Bars::const_iterator iterBars = bars.begin(); bars.end() != iterBars; ++iterBars ) {
    ou::tf::Price price( iterBars->DateTime(), iterBars->Close() );
    iter->second.prices.Append( price );
//    iter->second.pricesBollinger20.Update();
//    iter->second.pricesSMA1.Update();
//    iter->second.pricesSMA2.Update();
//    iter->second.pricesSMA3.Update();
  }
  //iter->second.pricesBollinger20.Update();

  ExcelFormat::CellFormat fmtNum(m_fmt_mgr);
  fmtNum.set_format_string( XLS_FORMAT_DECIMAL );

  ExcelFormat::CellFormat fmtCenterTxt(m_fmt_mgr);
  fmtCenterTxt.set_alignment( ExcelFormat::EXCEL_HALIGN_CENTRED );

  int iy = m_sheet->GetTotalRows();
  ExcelFormat::BasicExcelCell* cell;
  int ix( 0 );

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetString( iter->second.sType.c_str() );

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetString( sObject.c_str() );

  double last = bars.Last()->Close();
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetDouble( last );
  cell->SetFormat( fmtNum );

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtCenterTxt );
  double upper = iter->second.pricesBollinger20.BBUpper();
  double lower = iter->second.pricesBollinger20.BBLower();
  double third = ( upper - lower ) / 3.0;
  if ( last > ( upper - third ) ) {
    std::cout << "u";
    cell->SetString( "u" );
  }
  else {
    if ( last < ( lower + third ) ) {
      std::cout << "l";
      cell->SetString( "l" );
    }
    else {
      std::cout << "m";
      cell->SetString( "m" );
    }
  }

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtCenterTxt );
  double sma1 = iter->second.pricesSMA1.MeanY();
  double sma2 = iter->second.pricesSMA2.MeanY();
  double sma3 = iter->second.pricesSMA3.MeanY();
  if ( ( last > sma1 ) && ( sma1 > sma2 ) && ( sma2 > sma3 ) ) {
    std::cout << ">";
    cell->SetString( ">" );
  }
  if ( ( last < sma1 ) && ( sma1 < sma2 ) && ( sma2 < sma3 ) ) {
    std::cout << "<";
    cell->SetString( "<" );
  }
  
  std::cout << sObject 
    << ": boll(" << lower << "," << upper
    << "),sma1(" << sma1
    << "),sma2(" << sma2
    << "),sma3(" << sma3
    << ")"
    << std::endl;

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( upper );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( lower );

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( sma1 );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( sma2 );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( sma3 );
  
}




