/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

//#include "StdAfx.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;

#include <OUCommon/KeywordMatch.h>
#include <OUCommon/Debug.h>

#include "IQFeedSymbolFile.h"

// for options, need to decode symbol name in order to get expiry day.

namespace ou { // One Unified
namespace tf { // TradeFrame

CIQFeedSymbolFile::CIQFeedSymbolFile(void)
: CInstrumentFile()
{
}

CIQFeedSymbolFile::~CIQFeedSymbolFile(void) {
}

// http://www.relisoft.com/book/tech/6lib.html
class LessThan
    : public std::binary_function<char const *, char const *, bool> {
public:
    bool operator () (char const * str1, 
                      char const * str2) const {
        return strcmp (str1, str2) < 0;
    }
};

unsigned char DecodeMonth( const std::string &s ) {
  unsigned char month2 = 0;
  if ( 'J' < s[0] ) {
    if ( 'N' < s[0] ) {
      month2 = ( 'O' == s[0] ) ? 10 : 9;
    }
    else {
      if ( 'M' == s[0] ) {
        month2 = ( 'R' == s[2] ) ? 3 : 5;
      }
      else {
        month2 = 11;
      }
    }
  }
  else {
    if ( 'J' == s[0] ) {
      if ( 'U' == s[1] ) {
        month2 = ( 'N' == s[2] ) ? 6 : 7;
      }
      else {
        month2 = 1;
      }
    }
    else {
      if ( 'A' == s[0] ) {
        month2 = ( 'P' == s[1] ) ? 4 : 8;
      }
      else {
        month2 = ( 'D' == s[0] ) ? 12 : 2;
      }
    }
  }
  assert( month2 > 0 );
  return month2;
}

bool CIQFeedSymbolFile::Load( const std::string& sTxtFileName, const std::string& sDbFileName ) {
  // mktsymbols_v2.txt

  // TODO:  need to prevent re-entrant execution:  ie, turn invoking button off
  // TODO:  need to put this inside a background thread, and lock out other database access processes while running

  //static const boost::regex rxFuture0( "[[:blank:]](JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]]{1}([0-9]{4})" );  // GOOG1CQ07	GOOG AUG 2007	ONECH
  //static const boost::regex rxFuture1( "[[:blank:]](JANUARY|FEBRUARY|MARCH|APRIL|MAY|JUNE|JULY|AUGUST|SEPTEMBER|OCTOBER|NOVEMBER|DECEMBER)[[:blank:]]{1}([0-9]{4})" );  // GOOG1CQ07	GOOG AUG 2007	ONECH
  //static const boost::regex rxFuture2( "(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}/(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}" ); //     No future match on HOX8-HOJ9, HEATING OIL #2 NOV 08/APR 09
//  static const boost::regex rxFuture2( "(JANUARY|FEBRUARY|MARCH|APRIL|MAY|JUNE|JULY|AUGUST|SEPTEMBER|OCTOBER|NOVEMBER|DECEMBER)[[:blank:]][0-9]{2}/(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]]20[0-9]{2}" ); //     No future match on HOX8-HOJ9, HEATING OIL #2 NOV 08/APR 09

  std::ifstream file;

  size_t cntLines = 0;
  size_t cntSIC = 0;
  size_t cntNAICS = 0;
  CInstrumentFile::structIndexes_t j, k, c; 
  bool bEndFound;

  unsigned short nUnderlyingSize = 0;
  std::map<std::string, unsigned long> mapUnderlying;  // keeps track of optionable symbols, to fix bool at end

  struct structCountPerString { // count cnt of string s
    size_t cnt;
    std::string s;
    structCountPerString( void ) : cnt( 0 ) {};
    bool operator<(const structCountPerString& rhs) { return (s < rhs.s); };
  };

  std::cout << "Initializing Structures" << std::endl;

  ou::KeyWordMatch<size_t> kwmExchanges( 0, 200 );  // about 300 characters?  ... fast look up of index into m_rExchanges, possibly faster than std::map
  kwmExchanges.AddPattern( "Unknown", 0 );
  std::vector<structCountPerString> vSymbolsPerExchange( 1 );
  vSymbolsPerExchange[ 0 ].s = "UNKNOWN";

  struct structSymbolTypes {
    unsigned char sc;
    std::string sSC;
  } rSymbolTypes[] = {
    { structSymbolRecord::Unknown, "UNKNOWN" },
    { structSymbolRecord::Bonds, "BONDS" },
    { structSymbolRecord::Calc , "CALC" },
    { structSymbolRecord::Equity , "EQUITY" },
    { structSymbolRecord::FOption , "FOPTION" },
    { structSymbolRecord::Forex , "FOREX" },
    { structSymbolRecord::Forward , "FORWARD" },
    { structSymbolRecord::Future , "FUTURE" },
    { structSymbolRecord::ICSpread , "ICSPREAD" },
    { structSymbolRecord::IEOption , "IEOPTION" },
    { structSymbolRecord::Index , "INDEX" },
    { structSymbolRecord::MktStats , "MKTSTATS" },
    { structSymbolRecord::Money , "MONEY" },
    { structSymbolRecord::Mutual , "MUTUAL" },
    { structSymbolRecord::PrecMtl , "PRECMTL" },
    { structSymbolRecord::Spot , "SPOT" },
    { structSymbolRecord::Spread , "SPREAD" },
    { structSymbolRecord::StratSpread, "STRATSPREAD" },
    { structSymbolRecord::Swaps, "SWAPS" },
    { structSymbolRecord::Treasuries, "TREASURIES" }
  };

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

  size_t cntSymbolClassifiers = sizeof( rSymbolTypes ) / sizeof( structSymbolTypes );

  ou::KeyWordMatch<size_t> kwmSymbolType( structSymbolRecord::Unknown, 120 );
  std::vector<size_t> vSymbolTypeStats( cntSymbolClassifiers );  // number of symbols of this SymbolType
  for ( size_t ix = 0; ix < cntSymbolClassifiers; ++ix ) {
    kwmSymbolType.AddPattern( rSymbolTypes[ ix ].sSC, rSymbolTypes[ ix ].sc );
    vSymbolTypeStats[ ix ] = 0;
  }

  std::string sUnderlying;

  // define option processing rules
  qi::rule<char *, std::string()> ruleString = +(qi::char_ - qi::char_(' '));
  qi::rule<char *> ruleUnderlyingSymbol = ruleString[boost::phoenix::ref(sUnderlying)=qi::_1];
  qi::rule<char *> ruleMonth = (
    qi::string("JAN")
    |qi::string("FEB")
    |qi::string("MAR")
    |qi::string("APR")
    |qi::string("MAY")
    |qi::string("JUN")
    |qi::string("JUL")
    |qi::string("AUG")
    |qi::string("SEP")
    |qi::string("OCT")
    |qi::string("NOV")
    |qi::string("DEC"))
    [boost::phoenix::ref(dbRecord.nMonth)=boost::phoenix::bind(&DecodeMonth, qi::_1)];  // 0 is nothing, legal is 1 - 12
  qi::rule<char *> ruleYear = qi::uint_[ref(dbRecord.nYear)=qi::_1];
  qi::rule<char *> ruleStrike = qi::float_[ref(dbRecord.fltStrike)=qi::_1];
  qi::rule<char *> ruleOptionSide = 
    qi::char_( 'C' )[boost::phoenix::ref(dbRecord.nOptionSide)=OptionSide::Call] 
  | qi::char_( 'P' )[boost::phoenix::ref(dbRecord.nOptionSide)=OptionSide::Put]; // 'C' or 'P' or ' ' or 0x00
  qi::rule<char *> ruleOption = qi::eps 
       >> ruleUnderlyingSymbol
       >> qi::space >> ruleMonth
       >> qi::space >> ruleYear
       >> qi::space >> ruleOptionSide
       >> qi::space >> ruleStrike
    ;

  qi::rule<char *, unsigned int> ruleSIC = qi::uint_[ref(dbRecord.SIC)=qi::_1];
  qi::rule<char *, unsigned int> ruleNAICS = qi::uint_[ref(dbRecord.NAICS)=qi::_1];

  // offset and size of each string in the inbound file record
  // use these as not all fields are kept within dbRecord
  CInstrumentFile::structIndexes_t offset[structSymbolRecord::_IXCount];
  CInstrumentFile::structIndexes_t count[structSymbolRecord::_IXCount];

  try {

    std::cout << "Opening Instrument Database ... ";
    OpenIQFSymbols( sDbFileName );
    u_int32_t countp = 0;
    std::cout << "truncating ... ";
    m_pdbSymbols->truncate( NULL, &countp, 0 );
    std::cout << "done." << std::endl;

    std::cout << "Opening Input Instrument File ";
    std::cout << sTxtFileName.c_str();
    std::cout << " ... ";
    file.open( sTxtFileName.c_str() );
    std::cout << std::endl;

    std::cout << "Loading Symbols ..." << std::endl;

    file.getline( dbRecord.line, structSymbolRecord::nMaxBufferSize );  // remove header line
    file.getline( dbRecord.line, structSymbolRecord::nMaxBufferSize );
    while ( !file.fail() ) {

      ++cntLines;  // number data lines processed

      bEndFound = false;
      c = j = k = 0; // c is count of char in string, j index into ix, k index into line

      dbRecord.sc.reset();  // initialize bits to zero
      dbRecord.fltStrike = 0;
      dbRecord.nYear = 0;
      dbRecord.nMonth = 0;
      dbRecord.nDay = 0;
      dbRecord.SIC = 0;
      dbRecord.NAICS = 0;
      dbRecord.nOptionSide = OptionSide::Unknown;
      offset[j] = k; // this initialization doesn't make much sense, could be offset[0]=0;?
      while ( !bEndFound ) {
        if ( 0 == dbRecord.line[k] ) {  // end of line has been found
          count[j] = c; // size of last string
          bEndFound = true;
          if ( j != ( structSymbolRecord::_IXCount - 1 ) ) {
            std::cout << "Not enough fields for: " << dbRecord.line << std::endl;
          }
        }
        else {
          if ( '\t' != dbRecord.line[k] ) { // look for tab field separator
            ++c;  // increment character count if no tab found
          }
          else {  // otherwise perform field end processing
            count[j] = c;
            c = 0;
            ++j;
            if ( j >= structSymbolRecord::_IXCount ) {
              bEndFound = true;
            }
            else {
              offset[j] = k+1; // beginning of new string
            }
            dbRecord.line[k] = 0;  // replace tab character with string terminator
          }
          ++k;
        }
      }

      if ( 0 == count[structSymbolRecord::IXDesc] ) {
        std::cout << dbRecord.line << ": warning, no description supplied" << std::endl;
      }

      if ( ( 0 == count[structSymbolRecord::IXSymbol] ) 
        || ( 0 == count[structSymbolRecord::IXExchange] ) 
        || ( 0 == count[structSymbolRecord::IXListedMarket] ) 
        || ( 0 == count[structSymbolRecord::IXSecurityType] ) ) {
        std::cout << dbRecord.line << ": Field length error" << std::endl;
      }
      else {

        for ( size_t ix = 0; ix < structSymbolRecord::nMaxStrings; ++ix ) { // store symbol, desc, exch, listed into db
          dbRecord.ix[ix] = offset[ix];
          dbRecord.cnt[ix] = count[ix];
        }

        size_t ix;

        dbRecord.bufferedlength = sizeof( structSymbolRecord ) 
          - structSymbolRecord::nMaxBufferSize 
          + offset[structSymbolRecord::IXListedMarket] + count[structSymbolRecord::IXListedMarket] + 1;  // 1 for last terminator

        std::string sSecurityType( dbRecord.line + offset[structSymbolRecord::IXSecurityType] );  // create string from sub-string

        ix = kwmSymbolType.FindMatch( sSecurityType );
        if ( structSymbolRecord::Unknown == ix ) {
          std::cout << sSecurityType << ": Unknown Security Type" << std::endl;
        }
        else {
          dbRecord.sc.set( ix );
        }

        dbRecord.eInstrumentType = ix;
        ++vSymbolTypeStats[ ix ];

        try {
          if ( 0 == strcmp( 
            dbRecord.line + offset[structSymbolRecord::IXExchange], 
            dbRecord.line + offset[structSymbolRecord::IXListedMarket] ) ) {
            // two fields re same, so just first field goes into key
            dbRecord.lenExchangeKey = count[structSymbolRecord::IXExchange];
          }
          else {
            // different, both fields make up key, so put comma between Exchange and Listed Market
            *(dbRecord.line + offset[structSymbolRecord::IXExchange] + count[structSymbolRecord::IXExchange]) = ',';
            dbRecord.lenExchangeKey = count[structSymbolRecord::IXExchange] + 1 + count[structSymbolRecord::IXListedMarket];
          }

          std::string sPattern( dbRecord.line + offset[structSymbolRecord::IXExchange] );

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
        catch ( std::exception e ) {
          std::cout << dbRecord.line << ": zero length pattern" << std::endl;
        }

        if ( structSymbolRecord::Equity == dbRecord.eInstrumentType ) {
          char* pBegin;
          char* pEnd;
          bool b;

          pBegin = dbRecord.line + offset[structSymbolRecord::IXSIC];  // SIC
          pEnd = pBegin + count[structSymbolRecord::IXSIC];
          b = parse( pBegin, pEnd, ruleSIC );
          if ( b && ( pBegin == pEnd ) ) {
            if ( 0 != dbRecord.SIC ) {
              ++cntSIC;
            }
          }

          pBegin = dbRecord.line + offset[structSymbolRecord::IXNAICS];  // NAICS
          pEnd = pBegin + count[structSymbolRecord::IXNAICS];
          b = parse( pBegin, pEnd, ruleNAICS );
          if ( b && ( pBegin == pEnd ) ) {  
            if ( 0 != dbRecord.NAICS ) {
              ++cntNAICS;
            }
          }
        }
        
        // parse out contract expiry information
        // � For combined session symbols, the first character is "+".
        //� For Night/Electronic sessions, the first character is "@".
        // � Replace the Month and Year code with "#" for Front Month (ie. @ES# instead of @ESU10).
        // � NEW!-Replace the Month and Year code with "#C" for Front Month back-adjusted history (ie. @ES#C instead of @ESU10). 
        // http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=commod&web=IQFeed&symbolguide=guide&displayaction=support&section=guide&type=comex&type2=comex_gbx
        if ( structSymbolRecord::Future == dbRecord.eInstrumentType ) {
          if ( 'Y' == *(dbRecord.line + offset[structSymbolRecord::IXFrontMonth]) ) {
            dbRecord.sc.set( structSymbolRecord::FrontMonth );
          }
          std::string sSymbol( dbRecord.line );
          bool bDecode = true;
//          if ( '+' == sSymbol[0] ) {
//          }
//          if ( '@' == sSymbol[0] ) {
//          }
          if ( '#' == sSymbol[ sSymbol.length() - 1 ] ) {
            bDecode = false;
          }
          if ( bDecode ) {
            std::string sYear = sSymbol.substr( sSymbol.length() - 2 );
            char mon = sSymbol[ sSymbol.length() - 3 ];
            if ( ( 'F' > mon ) || ( 'Z' < mon ) || ( 0 == rFutureMonth[ mon - 'A' ] ) ) {
              std::cout << "Bad future month on " << dbRecord.line << ", " << dbRecord.line + offset[structSymbolRecord::IXDesc] << std::endl;
            }
            else {
              dbRecord.nMonth = rFutureMonth[ mon - 'A' ];
              dbRecord.nYear = 2000 + atoi( sYear.c_str() );
            }
          }
        }

        // parse out contract information
        if ( structSymbolRecord::IEOption == dbRecord.eInstrumentType ) {
          if ( 'Y' == *(dbRecord.line + offset[structSymbolRecord::IXFrontMonth]) ) {
            dbRecord.sc.set( structSymbolRecord::FrontMonth );
          }
          char* pBegin = dbRecord.line + offset[structSymbolRecord::IXDesc];
          char* pEnd = pBegin + count[structSymbolRecord::IXDesc];
          bool b = parse( pBegin, pEnd, ruleOption );
          if ( b && ( pBegin == pEnd ) ) {
            if ( 0 == sUnderlying.size() ) {
              std::cout << "Zero length underlying for " << dbRecord.line << std::endl;
            }
            else {
              mapUnderlying[ sUnderlying ] = 1;  // simply create an entry for later use
            }
            nUnderlyingSize = std::max<unsigned short>( nUnderlyingSize, sUnderlying.size() );
          }
          else {
            std::cout  << "No option match on " << dbRecord.line << ", " << dbRecord.line + offset[structSymbolRecord::IXDesc] << std::endl;
          }
        }

        // update database
        Dbt key( dbRecord.line, dbRecord.cnt[structSymbolRecord::IXSymbol] );
        assert( dbRecord.bufferedlength <= 255 );
        Dbt value( &dbRecord, dbRecord.bufferedlength );
        int ret;
        try {
          ret = m_pdbSymbols->put( 0, &key, &value, DB_NOOVERWRITE );
          if ( 0 != ret ) std::cout << "non return put value " << ret << std::endl;
        }
        catch (...) {
          std::cout << "db write problem with: " << dbRecord.line << std::endl;
        }
      }

      // get next line from text file
      file.getline( dbRecord.line, structSymbolRecord::nMaxBufferSize );
    }

    {
      structSymbolRecord rec;
      Dbt k;
      Dbt v;
      v.set_flags( DB_DBT_USERMEM );
      v.set_data( &rec );  
      int ret;
      for ( std::map<std::string, unsigned long>::iterator iter = mapUnderlying.begin();
        mapUnderlying.end() != iter; ++iter ) {
        // iterate through map and update ucHasOptions flag in each record
        k.set_data( (void*) iter->first.c_str() );
        k.set_size( iter->first.size() );
        v.set_ulen( sizeof( structSymbolRecord ) );
        v.set_size( sizeof( structSymbolRecord ) );
        ret = m_pdbSymbols->get( 0, &k, &v, 0 );
        if ( 0 == ret ) {
          rec.sc.set( structSymbolRecord::HasOptions );  // check if underlying is a stock/equity/future  (sc should be set as such as well)
          k.set_data( (void*) iter->first.c_str() );
          k.set_size( iter->first.size() );
          ret = m_pdbSymbols->put( 0, &k, &v, 0 );
          if ( 0 != ret ) {
            std::cout << "failed write on " << iter->first << std::endl;
          }
        }
        else {
          std::cout << "option with " << iter->first << " in DESCRIPTION has no underlying entry" << std::endl; // no underlying listed
        }

      }
    }

    std::cout << std::endl;
    for ( size_t ix = 0; ix < cntSymbolClassifiers; ++ix ) {
      std::cout << rSymbolTypes[ ix ].sSC << "=" << vSymbolTypeStats[ ix ] << std::endl;
    }
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
    "#kwmExchanges nodes " << kwmExchanges.GetNodeCount() << std::endl <<
    "#kwmSymbolType nodes " << kwmSymbolType.GetNodeCount() << std::endl
    )
#endif

  }
  catch( DbRunRecoveryException &e ) {
    std::cout << "DbRunRecoveryException exception " << e.what() << std::endl;
  }
  catch( DbException &dbex ) {
    std::cout << "IQFSymbols exception: " << dbex.what() << std::endl;
  }

  CloseIQFSymbols();
  file.close();

  return true;
}

} // namespace tf
} // namespace ou
