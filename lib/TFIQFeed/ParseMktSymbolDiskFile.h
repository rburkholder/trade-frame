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

#pragma once

#include <fstream>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

template<typename CRTP, typename IteratorMessages, typename Parser>
class ParseMktSymbolDiskFile {
public:
  ParseMktSymbolDiskFile( void );
  ~ParseMktSymbolDiskFile( void ) {};
  bool Run( IteratorMessages );
protected:
private:
};

template<typename CRTP, typename IteratorMessages, typename Parser>
ParseMktSymbolDiskFile<CRTP, IteratorMessages, Parser>::ParseMktSymbolDiskFile( void ) {
}

template<typename CRTP, typename IteratorMessages, typename Parser>
bool ParseMktSymbolDiskFile<CRTP, IteratorMessages, Parser>::Run( IteratorMessages iter ) {

    std::ifstream file;
    char* name = "mktsymbols_v2.txt";
    std::cout << "Opening Input Instrument File ";
    std::cout << name;
    std::cout << " ... ";
    file.open( name );
    std::cout << std::endl;

    std::cout << "Loading Symbols ..." << std::endl;

    char line[ 500 ];

    file.getline( line, 500 );  // remove header line
    file.getline( line, 500 );
    while ( !file.fail() ) {

      const char* pLine1( line );
      const char* pLine2( line + 500 );

      file.getline( line, 500 );

    }

    file.close();

}

} // namespace iqfeed
} // namespace tf
} // namespace ou

