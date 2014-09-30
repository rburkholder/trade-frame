/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <fstream>
#include <iostream>

#include "ParseMktSymbolDiskFile.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

ParseMktSymbolDiskFile::ParseMktSymbolDiskFile( void ) {
}

// try http://stackoverflow.com/questions/2291802/is-there-a-c-iterator-that-can-iterate-over-a-file-line-by-line

void ParseMktSymbolDiskFile::Run( const std::string& sName ) {

  std::ifstream file;
  //char* name = "mktsymbols_v2.txt";
  std::cout << "Opening Input Symbol File ";
  std::cout << sName;
  std::cout << " ... ";
  file.open( sName.c_str() );
  if ( file.bad() ) {
    throw  std::runtime_error( "Can't open input file" );
  }
  std::cout << std::endl;

  std::cout << "Loading Symbols ..." << std::endl;

  char line[ 500 ];
  size_t cntLines( 0 );

  file.getline( line, 500 );  // remove header line
  file.getline( line, 500 );
  while ( !file.fail() ) {

    cntLines++;

    iterator_t pLine1( line );
    iterator_t pLine2( line + 500 );

    if ( 0 != m_OnProcessLine ) m_OnProcessLine( pLine1, pLine2 );

    file.getline( line, 500 );

//    if ( 1000 < cntLines ) break;

  }

  std::cout << cntLines << " lines written" << std::endl;

  file.close();

}

} // namespace iqfeed
} // namespace tf
} // namespace ou

