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

#include <OUCommon/Debug.h>

#include "ValidateMktSymbolLine.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

//  FUTURE MONTH CODES
//  Jan-F    Feb-G    Mar-H    Apr-J    May-K    Jun-M   
//  Jul-N    Aug-Q    Sep-U    Oct-V    Nov-X    Dec-Z 
boost::uint8_t rFutureMonth[] = {
    0,  // A
    0,  // B
    0,  // C
    0,  // D
    0,  // E
      1,  // F
      2,  // G
      3,  // H
    0,  // I
      4,  // J
      5,  // K
    0,  // L
      6,  // M
      7,  // N
    0,  // O
    0,  // P
      8,  // Q
    0,  // R
    0,  // S
    0,  // T
      9,  // U
     10,  // V
    0,  // W
     11,  // X
    0,  // Y
     12  // Z
  };
 

ValidateMktSymbolLine::ValidateMktSymbolLine( void ) :
  kwmExchanges( 0, 200 ), // about 300 characters?  ... fast look up of index into m_rExchanges, possibly faster than std::map
  vSymbolsPerExchange( 1 ), nUnderlyingSize( 0 ),
  cntLinesTotal( 0 ), cntLinesParsed( 0 ), cntSIC( 0 ), cntNAICS( 0 ),
  vSymbolTypeStats( sc_t::_Count )
{
    kwmExchanges.AddPattern( "Unknown", 0 );
    vSymbolsPerExchange[ 0 ].s = "UNKNOWN";
}

void ValidateMktSymbolLine::PostProcess( void ) {
  for ( std::map<std::string, unsigned long>::iterator iter = mapUnderlying.begin();
        mapUnderlying.end() != iter; 
        iter++ ) {
    // iterate through map and update bHasOptions flag in each record
      // ? check if underlying is a stock/equity/future  (sc should be set as such as well)
      // if no record found for the symbol, then error:
      //std::cout << "option with " << iter->first << " in DESCRIPTION has no underlying entry" << std::endl; // no underlying listed
    if ( 0 != m_OnProcessHasOption ) m_OnProcessHasOption( iter->first );
//    if ( &ValidateMktSymbolLine<CRTP,IteratorLines>::HandleUpdateHasOption != &CRTP::HandleUpdateHasOption ) {
//      static_cast<CRTP*>( this )->HandleUpdateHasOption( iter->first );
//    }
  }
}

void ValidateMktSymbolLine::Summary( void ) {

  struct processSymbols {
    void operator()( const std::string& s, ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc ) {
      std::cout << s << "=" << v[ sc ] << std::endl;
    }
    explicit processSymbols( std::vector<size_t>& v_ ): v(v_) {};
    std::vector<size_t>& v;
  };

  std::cout << std::endl;

  if ( cntLinesTotal != cntLinesParsed ) {
    std::cout << "Warning: " << cntLinesParsed << " parsed vs " << cntLinesTotal << " total." << std::endl;
  }

  parserFullLine.symTypes.for_each( processSymbols( vSymbolTypeStats ) );
  std::cout << std::endl;

  std::cout << "Count Optionables  =" << mapUnderlying.size() << std::endl;
  std::cout << "Max Underlying Size=" << nUnderlyingSize << std::endl;
  std::cout << "cntSIC             =" << cntSIC << std::endl;
  std::cout << "cntNAICS           =" << cntNAICS << std::endl;
  std::cout << std::endl;

  std::sort( vSymbolsPerExchange.begin(), vSymbolsPerExchange.end() );
  for ( size_t ix = 0; ix < vSymbolsPerExchange.size(); ++ix ) {
    std::cout << vSymbolsPerExchange[ ix ].s << "=" << vSymbolsPerExchange[ ix ].cnt << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Symbol List Complete" << std::endl;

#ifdef _DEBUG
  DEBUGOUT( 
  "#kwmExchanges nodes " << kwmExchanges.GetNodeCount() << std::endl 
  )
#endif
}

} // namespace iqfeed
} // namespace tf
} // namespace ou

