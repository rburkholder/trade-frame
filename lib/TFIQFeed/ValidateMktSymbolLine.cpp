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

#include <boost/lexical_cast.hpp>

#include <OUCommon/Debug.h>

#include <TFOptions/CalcExpiry.h>

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
    m_vSuffixesToTest.push_back( "" );
    m_vSuffixesToTest.push_back( ".X" );
    m_vSuffixesToTest.push_back( ".XI" );
    m_vSuffixesToTest.push_back( ".XO" );
}

void ValidateMktSymbolLine::PostProcess( void ) {
  for ( mapUnderlying_t::iterator iterMap = mapUnderlying.begin(); mapUnderlying.end() != iterMap; iterMap++ ) {
    // iterate through map and update bHasOptions flag in each record
      // ? check if underlying is a stock/equity/future  (sc should be set as such as well)
    bool bStatus;  // todo:  check all combinations to see if any overlap
    if ( 0 != m_OnProcessHasOption ) {
      for ( std::vector<std::string>::const_iterator interVec = m_vSuffixesToTest.begin(); m_vSuffixesToTest.end() != interVec; interVec++ ) {
        bStatus = m_OnProcessHasOption( iterMap->second + *interVec );
        if ( bStatus ) {
          if ( m_vSuffixesToTest.begin() != interVec ) {
            if ( 0 != m_OnUpdateOptionUnderlying ) m_OnUpdateOptionUnderlying( iterMap->first, iterMap->second );
          }
          break;
        }
      }
      if ( !bStatus ) {
        std::set<std::string>::iterator iterSet = m_setNoUnderlying.find( iterMap->second );
        if ( m_setNoUnderlying.end() == iterSet ) {
          m_setNoUnderlying.insert( iterMap->second );
          std::cout << "Error: can't find underlying " << iterMap->second << " for options like " << iterMap->first << std::endl;
        }
      }
    }
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

void ValidateMktSymbolLine::ParseOptionContractInformation( trd_t& trd ) {
  ou::tf::iqfeed::structParsedOptionDescription structOption( trd.sUnderlying, trd.nMonth, trd.nYear, trd.eOptionSide, trd.dblStrike );  // pass in references to final variables
  std::string::const_iterator sb( trd.sDescription.begin() );
  std::string::const_iterator se( trd.sDescription.end() );
  bool b = parse( sb, se, parserOptionDescription, structOption );
  if ( b && ( sb == se ) ) {
    if ( 0 == trd.sUnderlying.length() ) {
      std::cout << "Option Decode:  Zero length underlying for " << trd.sSymbol << std::endl;
    }
    else {
      std::string::size_type ixSlash = structOption.sUnderlying.find( "/" );
      if ( std::string::npos != ixSlash ) {
        structOption.sUnderlying.replace( ixSlash, 1, "." );
      }
      mapUnderlying[ trd.sSymbol ] = structOption.sUnderlying;  // simply create an entry for later use
    }
    nUnderlyingSize = std::max<unsigned short>( nUnderlyingSize, structOption.sUnderlying.size() );
    structParsedOptionSymbol1 pos1;
    b = parse( trd.sSymbol.begin(), trd.sSymbol.end(), parserOptionSymbol1, pos1 );
    if ( b ) {
      if ( 4 > pos1.sDigits.length() ) {  // looking for yydd
        std::cout << "Option Symbol Decode: not enough digits, " << trd.sSymbol << std::endl;
      }
      else {
        if ( 5 < pos1.sDigits.length() ) {
          // should not have this condition
          std::cout << "Option Symbol Decode:  garbage prefix yydd, ignoring" << trd.sSymbol << std::endl;
          pos1.sDigits = pos1.sDigits.substr( pos1.sDigits.length() - 4 );
        }
        if ( 5 == pos1.sDigits.length() ) {
          // has one extra preceding digit, could be '1', '7'
          char ch = pos1.sDigits[ 0 ];
          std::string sSpareChar = pos1.sDigits.substr( 0, 1 );
          pos1.sDigits.erase( 0, 1 );
          //pos1.sDigits = pos1.sDigits.substr( pos1.sDigits.length() - 4 );
          // spyj could be jumbo options: 1000 vs 100 multiplier
          switch ( ch ) {
          case '1':  // adjusted option (usually due to a split/merger of the company)
          case '2':  // numbers themselves are irrelevant 
          case '3':  // option is adjusted "in some way"
          case '4':  // 2013/09/03 dtniq forum response
          case '7':  // mini option, 10x multiplier rather than normal 100x
            pos1.sText += ch;
            mapUnderlying[ trd.sSymbol ] = trd.sUnderlying = pos1.sText;
            // do further massage on 7 later so can be tradeable
            break;
          default:
            std::cout << "Option Symbol Decode:  " << pos1.sText << " has unknown suffix " << ch << std::endl;
          }
        }
        assert( 4 == pos1.sDigits.length() );
        std::string::const_iterator iter1 = pos1.sDigits.begin();
        std::string::const_iterator iter2 = pos1.sDigits.end();
        structParsedOptionSymbol2 pos2;
        b = parse( iter1, iter2, parserOptionSymbol2, pos2 );
        if ( b ) {
          assert( ( 2000 + pos2.nYear ) == structOption.nYear );
          trd.nDay = pos2.nDay;
        }
        boost::gregorian::date date( trd.nYear, trd.nMonth, trd.nDay );
        if ( 6 /* saturday */ == date.day_of_week() ) {
          static boost::gregorian::date_duration dur( 1 );
          date = date - dur;
          boost::gregorian::date::ymd_type ymd = date.year_month_day();
          trd.nYear = ymd.year;
          trd.nMonth = ymd.month;
          trd.nDay = ymd.day;
        }
        else {
          std::cout << "Option Decode problems on date, " << trd.sSymbol << std::endl;
        }
      }
            
      std::string sTmp = structOption.sUnderlying;
      std::string::size_type ixDot = sTmp.find( "." );
      if ( std::string::npos != ixDot ) {  // remove dot as option symbol does not have '.' in it
        sTmp.erase( ixDot, 1 );
      }
      if ( pos1.sText != sTmp ) {  // check against modified underlying
        std::cout 
          << "Option Symbol Decode: changing underlying on " 
          << trd.sSymbol << " from "
          << structOption.sUnderlying << " to " << pos1.sText << std::endl;
        trd.sUnderlying = pos1.sText;
        mapUnderlying[ trd.sSymbol ] = pos1.sText;
      }
      assert( pos1.dblStrike == structOption.dblStrike );
    }
    else {
      std::cout << "Option Symbol Decode:  some sort of error, " << trd.sSymbol << std::endl;
    }
  }
  else {
    std::cout  << "Option Decode:  Incomplete, " << trd.sSymbol << ", " << trd.sDescription << std::endl;
  }
}

void ValidateMktSymbolLine::ParseFOptionContractInformation( trd_t& trd ) {
  ou::tf::iqfeed::structParsedFOptionDescription structOption( trd.sUnderlying, trd.nMonth, trd.nYear, trd.eOptionSide, trd.dblStrike );  // pass in references to final variables
  std::string::const_iterator sb( trd.sDescription.begin() );
  std::string::const_iterator se( trd.sDescription.end() );
  bool b = parse( sb, se, parserFOptionDescription, structOption );
  if ( b && ( sb == se ) ) {
    if ( 0 == trd.sUnderlying.length() ) {
      std::cout << "FOption Decode:  Zero length underlying for " << trd.sSymbol << std::endl;
    }
    else {
      std::string::size_type ixSlash = structOption.sUnderlying.find( "/" );
      if ( std::string::npos != ixSlash ) {
        structOption.sUnderlying.replace( ixSlash, 1, "." );
      }
      mapUnderlying[ trd.sSymbol ] = structOption.sUnderlying;  // simply create an entry for later use
    }
    nUnderlyingSize = std::max<unsigned short>( nUnderlyingSize, structOption.sUnderlying.size() );
    structParsedOptionSymbol1 pos1;
    b = parse( trd.sSymbol.begin(), trd.sSymbol.end(), parserFOptionSymbol1, pos1 );
    if ( b ) {
      if ( 2 != pos1.sDigits.length() ) {  // looking for yy
        std::cout << "Option Symbol Decode: not enough digits, " << trd.sSymbol << std::endl;
      }
      else {

        trd.nDay = 0;
        trd.nYear = 2000 + boost::lexical_cast<uint16_t>( pos1.sDigits );
            
        char c = pos1.sText.back();
        assert( ( 'F' <= c ) && ( c <= 'Z' ) );
        trd.nMonth = rFutureMonth[ c - 'A' ];

        boost::gregorian::date date( trd.nYear, trd.nMonth, 1 );
        date = ou::tf::option::FuturesOptionExpiry( date );
        boost::gregorian::date::ymd_type ymd = date.year_month_day();
        trd.nYear = ymd.year;
        trd.nMonth = ymd.month;
        trd.nDay = ymd.day;

        pos1.sText = pos1.sText.substr( 0, pos1.sText.length() - 1 );

        std::string sTmp = structOption.sUnderlying;

        if ( pos1.sText != sTmp ) {  // check against modified underlying
//          std::cout 
//            << "Option Symbol Decode: changing underlying on " 
//            << trd.sSymbol << " from "
//            << structOption.sUnderlying << " to " << pos1.sText << std::endl;
          trd.sUnderlying = pos1.sText;
          mapUnderlying[ trd.sSymbol ] = pos1.sText;
        }
        assert( pos1.dblStrike == structOption.dblStrike );
      }
    }
    else {
      std::cout << "Option Symbol Decode:  some sort of error, " << trd.sSymbol << std::endl;
    }
  }
  else {
    std::cout  << "Option Decode:  Incomplete, " << trd.sSymbol << ", " << trd.sDescription << std::endl;
  }
}

} // namespace iqfeed
} // namespace tf
} // namespace ou

