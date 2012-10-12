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

#include <vector>
#include <map>

#include <OUCommon/KeywordMatch.h>

#include "ParseMktSymbolLine.h"
#include "ParseOptionDescription.h"

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
 

template<typename CRTP, typename IteratorLines, typename IteratorMessages> 
class ValidateMktSymbolLine {
public:
  ValidateMktSymbolLine( void );
  virtual ~ValidateMktSymbolLine( void ) {};
  bool Parse( Iterator begin, Iterator end );
protected:
  void HandleStructure( void ) {};  // fill in type later.  override by inheriting class
private:
  typedef ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc_t;

    struct structCountPerString { // count cnt of string s
      size_t cnt;
      std::string s;
      structCountPerString( void ) : cnt( 0 ) {};
      bool operator<(const structCountPerString& rhs) { return (s < rhs.s); };
    };

  ou::KeyWordMatch<size_t> kwmExchanges;
  std::vector<structCountPerString> vSymbolsPerExchange;
  std::map<std::string, unsigned long> mapUnderlying;  // keeps track of optionable symbols, to fix bool at end

  unsigned short nUnderlyingSize;
  size_t cntLinesTotal;
  size_t cntLinesParsed;
  size_t cntSIC;
  size_t cntNAICS;

  ou::tf::iqfeed::MktSymbolLineParser<const char*> parser;
  ou::tf::iqfeed::OptionDescriptionParser<std::string::const_iterator> parserOption;
  std::vector<size_t> vSymbolTypeStats( sc_t::_Count );  // number of symbols of this SymbolType
};

template<typename CRTP, typename IteratorLines, typename IteratorMessages> 
ValidateMktSymbolLine<CRTP,IteratorLines,IteratorMessages>::ValidateMktSymbolLine( void ) :
  kwmExchanges( 0, 200 ), // about 300 characters?  ... fast look up of index into m_rExchanges, possibly faster than std::map
  vSymbolsPerExchange( 1 ), nUnderlyingSize( 0 ),
  cntLinesTotal( 0 ), cntLinesParsed( 0 ), cntSIC( 0 ), cntNAICS( 0 )
{
    kwmExchanges.AddPattern( "Unknown", 0 );
    vSymbolsPerExchange[ 0 ].s = "UNKNOWN";
}

template<typename CRTP, typename IteratorLines, typename IteratorMessages> 
ValidateMktSymbolLine<CRTP,IteratorLines,IteratorMessages>::Parse( Iterator begin, Iterator end ) {

      ou::tf::iqfeed::MarketSymbol::TableRowDef trd;

      ++cntLinesTotal;  // number data lines processed

      // try http://stackoverflow.com/questions/2291802/is-there-a-c-iterator-that-can-iterate-over-a-file-line-by-line

      try {
        bool b = qi::parse( pLine1, pLine2, parser, trd );
        if ( !b ) {
          std::cout << "problems parsing" << std::endl;
        }
        else {

          cntLinesParsed++;

          size_t ix;

          vSymbolTypeStats[ trd.sc ]++;
          if ( sc_t::Unknown == trd.sc ) {
            // set marker not to save record?
            std::cout << "Unknown symbol type for:  " << trd.sSymbol << std::endl;
          }

          std::string sPattern( trd.sExchange );
          if ( trd.sExchange == trd.sListedMarket ) {
          }
          else {
            sPattern += "," + trd.sListedMarket;
          }

          if ( 0 == sPattern.length() ) {
            std::cout << trd.sSymbol << " has zero length exchange,market" << std::endl;
          }
          else {
            ix = kwmExchanges.FindMatch( sPattern );
            if ( ( 0 == ix ) || ( sPattern.length() != vSymbolsPerExchange[ ix ].s.length() ) ) {
              std::cout << "Adding Exchange " << sPattern << std::endl;
              size_t cnt = kwmExchanges.GetPatternCount();
              kwmExchanges.AddPattern( sPattern, cnt );
              structCountPerString cps;
              vSymbolsPerExchange.push_back( cps );
              vSymbolsPerExchange[ cnt ].cnt = 1;
              vSymbolsPerExchange[ cnt ].s = sPattern;
            }
            else {
              ++( vSymbolsPerExchange[ ix ].cnt );
            }
          }

          if ( ou::tf::iqfeed::MarketSymbol::Equity == trd.sc ) {
            if ( 0 != trd.sSIC.length() ) cntSIC++;
            if ( 0 != trd.sNAICS.length() ) cntNAICS++;
          }

          // parse out contract expiry information
          // � For combined session symbols, the first character is "+".
          //� For Night/Electronic sessions, the first character is "@".
          // � Replace the Month and Year code with "#" for Front Month (ie. @ES# instead of @ESU10).
          // � NEW!-Replace the Month and Year code with "#C" for Front Month back-adjusted history (ie. @ES#C instead of @ESU10). 
          // http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=commod&web=IQFeed&symbolguide=guide&displayaction=support&section=guide&type=comex&type2=comex_gbx
          if ( ou::tf::iqfeed::MarketSymbol::Future == trd.sc ) {
            bool bDecode = true;
  //          if ( '+' == sSymbol[0] ) {
  //          }
  //          if ( '@' == sSymbol[0] ) {
  //          }
            if ( '#' == trd.sSymbol[ trd.sSymbol.length() - 1 ] ) {
              bDecode = false;
            }
            if ( bDecode ) {
              std::string sYear = trd.sSymbol.substr( trd.sSymbol.length() - 2 );
              char mon = trd.sSymbol[ trd.sSymbol.length() - 3 ];
              if ( ( 'F' > mon ) || ( 'Z' < mon ) || ( 0 == rFutureMonth[ mon - 'A' ] ) ) {
                std::cout << "Bad futures month on " << trd.sSymbol << ": " << trd.sDescription << std::endl;
              }
              else {
                trd.nMonth = rFutureMonth[ mon - 'A' ];
                trd.nYear = 2000 + atoi( sYear.c_str() );
              }
            }
          }

          // parse out option contract information
          if ( ou::tf::iqfeed::MarketSymbol::IEOption == trd.sc ) {
            ou::tf::iqfeed::structParsedOptionDescription structOption( trd.nMonth, trd.nYear, trd.eOptionSide, trd.dblStrike );
            std::string::const_iterator sb( trd.sDescription.begin() );
            std::string::const_iterator se( trd.sDescription.end() );
            bool b = parse( sb, se, parserOption, structOption );
            if ( b && ( sb == se ) ) {
              if ( 0 == structOption.sUnderlying ) {
                std::cout << "Option Decode:  Zero length underlying for " << trd.sSymbol << std::endl;
              }
              else {
                mapUnderlying[ structOption.sUnderlying ] = 1;  // simply create an entry for later use
              }
              nUnderlyingSize = std::max<unsigned short>( nUnderlyingSize, structOption.sUnderlying.size() );
            }
            else {
              std::cout  << "Option Decode:  Incomplete, " << trd.sSymbol << ", " << trd.sDescription << std::endl;
            }
          }

        }
        if ( 0 == trd.sDescription.length() ) {
          std::cout << trd.sSymbol << ": missing description" << std::endl;
        }
      }
      catch (...) {
        std::cout << "broken" << std::endl;
      }

      if ( cntLinesTotal > 1000 ) break;

    struct processSymbols {
      void operator()( const std::string& s, ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc ) {
        std::cout << s << "=" << v[ sc ] << std::endl;
      }
      processSymbols( std::vector<size_t>& v_ ): v(v_) {
      }
      std::vector<size_t>& v;
    };

    std::cout << std::endl;
//    for ( size_t ix = 0; ix < ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier_count; ++ix ) {
//      std::cout << rSymbolTypes[ ix ].sSC << "=" << vSymbolTypeStats[ ix ] << std::endl;
//    }
    parser.symTypes.for_each( processSymbols( vSymbolTypeStats ) );
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
