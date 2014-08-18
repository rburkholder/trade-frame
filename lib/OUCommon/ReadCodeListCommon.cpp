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

#include "ReadCodeListCommon.h"

ReadCodeListCommon::ReadCodeListCommon(void) {
}

ReadCodeListCommon::~ReadCodeListCommon(void) {
}

bool ReadCodeListCommon::Extract( std::string& s, ExcelFormat::BasicExcelWorksheet* sheet, int row, int col ) {

  bool bReturn( true );
  ExcelFormat::BasicExcelCell* cell;

  cell = sheet->Cell( row, col );
  switch( cell->Type() ) {
  case ExcelFormat::BasicExcelCell::WSTRING:
    {
    char DefChar = ' ';
    int nStringLength;
    nStringLength = cell->GetStringLength();
    int cnt = WideCharToMultiByte( CP_ACP, 0, cell->GetWString(), nStringLength, 0, 0, &DefChar, NULL );
    char* ch = new char[ cnt + 1 ];
    WideCharToMultiByte( CP_ACP, 0, cell->GetWString(), nStringLength, ch, cnt + 1, &DefChar, NULL );
    std::string ss( ch, cnt );
    delete[] ch;
    ch = 0;
    s = ss;
    }
    break;
  case ExcelFormat::BasicExcelCell::STRING: 
    s = cell->GetString();
    break;
  case ExcelFormat::BasicExcelCell::DOUBLE:
  case ExcelFormat::BasicExcelCell::INT:
  case ExcelFormat::BasicExcelCell::UNDEFINED:
  default:
    bReturn = false;
    break;
  }

  return bReturn;

}

bool ReadCodeListCommon::Extract( double& dbl, ExcelFormat::BasicExcelWorksheet* sheet, int row, int col ) {

  bool bReturn( true );
  ExcelFormat::BasicExcelCell* cell;

  cell = sheet->Cell( row, col );
  switch( cell->Type() ) {
  case ExcelFormat::BasicExcelCell::DOUBLE:
    dbl = cell->GetDouble();
    break;
  case ExcelFormat::BasicExcelCell::UNDEFINED:
  default:
    bReturn = false;
    break;
  }

  return bReturn;

}

const std::string& ReadCodeListCommon::LookupSIC( boost::uint32_t key ) {
  static const std::string s( "" );
  mapItem_t::const_iterator iter = m_mapSic.find( key );
  //return ( m_mapLuSic.end() == iter ) ? s : m_vLU[ iter->second ].sSicDesc;
  return ( m_mapSic.end() == iter ) ? s : iter->second;
}

const std::string& ReadCodeListCommon::LookupNAICS( boost::uint32_t key ) {
  static const std::string s( "" );
  mapItem_t::const_iterator iter = m_mapNaics.find( key );
  //return ( m_mapLuNaics.end() == iter ) ? s : m_vLU[ iter->second ].sNaicsDesc;
  return ( m_mapNaics.end() == iter ) ? s : iter->second;
}

