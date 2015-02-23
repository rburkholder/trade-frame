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
#include <string>
#include <set>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <OUCommon/KeyWordMatch.h>

#include "ParseMktSymbolLine.h"
#include "ParseOptionDescription.h"
#include "ParseFOptionDescription.h"
#include "ParseOptionSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class ValidateMktSymbolLine {
public:

  typedef ou::tf::iqfeed::MarketSymbol::TableRowDef trd_t;
  typedef FastDelegate1<const trd_t&> OnProcessLine_t;
  typedef FastDelegate1<const std::string&,bool> OnProcessHasOption_t;
  typedef FastDelegate2<const std::string&,const std::string&> OnUpdateOptionUnderlying_t; // option name, underlying name

  void SetOnProcessLine( OnProcessLine_t function ) {
    m_OnProcessLine = function;
  }
  void SetOnProcessHasOption( OnProcessHasOption_t function ) {
    m_OnProcessHasOption = function;
  }
  void SetOnUpdateOptionUnderlying( OnUpdateOptionUnderlying_t function ) {
    m_OnUpdateOptionUnderlying = function;
  }

  ValidateMktSymbolLine( void );
  virtual ~ValidateMktSymbolLine( void ) {};

  template<typename Iterator>
  void Parse( Iterator& begin, Iterator& end );

  template<typename Iterator>
  void ParseHeaderLine( Iterator& begin, Iterator& end );

  void PostProcess( void );

  void Summary( void );

  size_t LinesProcessed( void ) const { return cntLinesTotal; };
  void InsertParsedStructure( trd_t& trd ) {};  // override by inheriting class
  bool HandleUpdateHasOption( const std::string& ) {}; // override by inheriting class
protected:
private:

  typedef ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc_t;

  OnProcessLine_t m_OnProcessLine;
  OnProcessHasOption_t m_OnProcessHasOption;
  OnUpdateOptionUnderlying_t m_OnUpdateOptionUnderlying;

  struct structCountPerString { // count cnt of string s
    size_t cnt;
    std::string s;
    structCountPerString( void ) : cnt( 0 ) {};
    bool operator<(const structCountPerString& rhs) { return (s < rhs.s); };
  };

  ou::KeyWordMatch<size_t> kwmExchanges;
  std::vector<structCountPerString> vSymbolsPerExchange;
  typedef std::map<std::string,std::string> mapUnderlying_t;  // option name, underlying name
  mapUnderlying_t mapUnderlying;  // keeps track of optionable symbols, to fix bool at end

  unsigned short nUnderlyingSize;
  size_t cntLinesTotal;
  size_t cntLinesParsed;
  size_t cntSIC;
  size_t cntNAICS;

  ou::tf::iqfeed::MktSymbolLineParser<const char*> parserFullLine;
  ou::tf::iqfeed::OptionDescriptionParser<std::string::const_iterator> parserOptionDescription;
  ou::tf::iqfeed::FOptionDescriptionParser<std::string::const_iterator> parserFOptionDescription;
  ou::tf::iqfeed::OptionSymbolParser1<std::string::const_iterator> parserOptionSymbol1;
  ou::tf::iqfeed::FOptionSymbolParser1<std::string::const_iterator> parserFOptionSymbol1;
  ou::tf::iqfeed::OptionSymbolParser2<std::string::const_iterator> parserOptionSymbol2;
  ou::tf::iqfeed::FOptionSymbolParser3<std::string::const_iterator> parserFOptionSymbol3;
  std::vector<size_t> vSymbolTypeStats;  // number of symbols of this SymbolType

  std::vector<std::string> m_vSuffixesToTest;
  std::set<std::string> m_setNoUnderlying;

  void ParseOptionContractInformation( trd_t& trd );
  void ParseFOptionContractInformation( trd_t& trd );

};

extern boost::uint8_t rFutureMonth[];

template<typename Iterator>
void ValidateMktSymbolLine::ParseHeaderLine( Iterator& begin, Iterator& end ) {
  bool b = qi::parse( begin, end, qi::lexeme[ +( qi::char_ - qi::eol ) ] >> qi::eol );
}

template<typename Iterator>
void ValidateMktSymbolLine::Parse( Iterator& begin, Iterator& end ) {

  ++cntLinesTotal;  // number data lines processed
  Iterator b( begin );  // keep in case of exception

  try {

    ou::tf::iqfeed::MarketSymbol::TableRowDef trd;

    bool b = qi::parse( begin, end, parserFullLine, trd );
    if ( !b ) {
      std::cout << "problems parsing" << std::endl;
    }
    else {

      //std::cout << "* " << trd.sSymbol << std::endl;

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

      bool bDecode( true );
      switch ( trd.sc ) {
      case ou::tf::iqfeed::MarketSymbol::Equity:
        if ( 0 != trd.nSIC ) cntSIC++;
        if ( 0 != trd.nNAICS ) cntNAICS++;
        break;
      case ou::tf::iqfeed::MarketSymbol::Future:
        // parse out contract expiry information
        // � For combined session symbols, the first character is "+".
        //� For Night/Electronic sessions, the first character is "@".
        // � Replace the Month and Year code with "#" for Front Month (ie. @ES# instead of @ESU10).
        // � NEW!-Replace the Month and Year code with "#C" for Front Month back-adjusted history (ie. @ES#C instead of @ESU10). 
        // http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=commod&web=IQFeed&symbolguide=guide&displayaction=support&section=guide&type=comex&type2=comex_gbx
        bDecode = true;
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
        break;
      case ou::tf::iqfeed::MarketSymbol::FOption:
        ParseFOptionContractInformation( trd );
        break;
      case ou::tf::iqfeed::MarketSymbol::IEOption:
        ParseOptionContractInformation( trd );
        break;
      } // switch( trd.sc )


      if ( 0 == trd.sDescription.length() ) {
        std::cout << trd.sSymbol << ": missing description" << std::endl;
      }

      if ( 0 != m_OnProcessLine ) m_OnProcessLine( trd );
//      if ( &ValidateMktSymbolLine<CRTP,IteratorLines>::InsertParsedStructure != &CRTP::InsertParsedStructure ) {
//        static_cast<CRTP*>( this )->InsertParsedStructure( trd );
//      }
    }
  }
  catch (...) {
    //std::cout << "parserFullLine broken" << std::endl;  // commented out with too much crap from futures parsing
    if ( b == begin ) { // nothing was processed, so skip over crap
      std::cout << "parserFullLine serious fail" << std::endl;
      while ( ( end != begin ) && ( '\n' != *begin )  && ( 0 != *begin ) ) ++begin;
      if ( '\n' == *begin ) ++begin; // one last character which should be the \n
    }
  }

}



} // namespace iqfeed
} // namespace tf
} // namespace ou
