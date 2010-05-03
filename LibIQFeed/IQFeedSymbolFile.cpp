/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

#include "IQFeedSymbolFile.h"

#include <LibCommon/KeywordMatch.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <boost/regex.hpp> 

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
using namespace boost::phoenix;

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

bool CIQFeedSymbolFile::Load( const std::string &filename ) {
  // mktsymbols_v2.txt

  // TODO:  need to prevent re-entrant execution:  ie, turn invoking button off
  // TODO:  need to put this inside a background thread, and lock out other database access processes while running

  static const boost::regex rxFuture( "[[:blank:]](JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]]{1}([0-9]{4})" );  // GOOG1CQ07	GOOG AUG 2007	ONECH
  static const boost::regex rxFuture2( "(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}/(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}" ); //     No future match on HOX8-HOJ9, HEATING OIL #2 NOV 08/APR 09

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
  };

  std::cout << "Initializing Structures" << std::endl;

  CKeyWordMatch<size_t> kwmExchanges( 0, 200 );  // about 300 characters?  ... fast look up of index into m_rExchanges, possibly faster than std::map
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
    { structSymbolRecord::StratSpread, "STRATSPREAD" }
  };

  size_t cntSymbolClassifiers = sizeof( rSymbolTypes ) / sizeof( structSymbolTypes );

  CKeyWordMatch<size_t> kwmSymbolType( structSymbolRecord::Unknown, 120 );
  std::vector<size_t> vSymbolTypeStats( cntSymbolClassifiers );  // number of symbols of this SymbolType
  for ( size_t ix = 0; ix < cntSymbolClassifiers; ++ix ) {
    kwmSymbolType.AddPattern( rSymbolTypes[ ix ].sSC, rSymbolTypes[ ix ].sc );
    vSymbolTypeStats[ ix ] = 0;
  }

  std::string sUnderlying;

  // define option processing rules
  qi::rule<char *, std::string()> ruleString = +(qi::char_ - qi::char_(' '));
  qi::rule<char *> ruleUnderlyingSymbol = ruleString[ref(sUnderlying)=qi::_1];
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
    [ref(dbRecord.nMonth)=bind(&DecodeMonth, qi::_1)];  // 0 is nothing, legal is 1 - 12
  qi::rule<char *> ruleYear = qi::uint_[ref(dbRecord.nYear)=qi::_1];
  qi::rule<char *> ruleStrike = qi::double_[ref(dbRecord.fltStrike)=qi::_1];
  qi::rule<char *> ruleOptionSide = 
    qi::char_( 'C' )[ref(dbRecord.nOptionSide)=OptionSide::Call] 
  | qi::char_( 'P' )[ref(dbRecord.nOptionSide)=OptionSide::Put]; // 'C' or 'P' or ' ' or 0x00
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
  static const unsigned char cntFields = 8;
  CInstrumentFile::structIndexes_t offset[cntFields];
  CInstrumentFile::structIndexes_t count[cntFields];

  try {

    std::cout << "Opening Instrument Database ... ";
    OpenIQFSymbols();
    u_int32_t countp = 0;
    std::cout << "truncating ... ";
    m_pdbSymbols->truncate( NULL, &countp, 0 );
    std::cout << "done." << std::endl;

    std::cout << "Opening Input Instrument File ";
    std::cout << filename.c_str();
    std::cout << " ... ";
    file.open( filename.c_str() );
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
      dbRecord.SIC = 0;
      dbRecord.NAICS = 0;
      dbRecord.nOptionSide = OptionSide::Unknown;
      offset[j] = k;
      while ( !bEndFound ) {
        if ( 0 == dbRecord.line[k] ) {  // end of line has been found
          count[j] = c; // size of last string
          bEndFound = true;
          if ( j != ( cntFields - 1 ) ) {
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
            if ( j >= cntFields ) {
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

      if ( 0 == count[1] ) {
        std::cout << dbRecord.line << ": no description supplied" << std::endl;
      }

      if ( ( 0 == count[0] ) || ( 0 == count[2] ) || ( 0 == count[3] ) || ( 0 == count[4] ) ) {
        std::cout << dbRecord.line << ": Field length error" << std::endl;
      }
      else {

        for ( size_t ix = 0; ix < structSymbolRecord::nMaxStrings; ++ix ) { // store symbol, desc, exch, listed into db
          dbRecord.ix[ix] = offset[ix];
          dbRecord.cnt[ix] = count[ix];
        }

        size_t ix;

        dbRecord.bufferedlength = sizeof( structSymbolRecord ) - structSymbolRecord::nMaxBufferSize + offset[3] + count[3] + 1;

        std::string sSecurityType( dbRecord.line + offset[4] );  // create string from sub-string

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
          std::string sPattern1( dbRecord.line + offset[2] );
          std::string sPattern2( dbRecord.line + offset[3] );
          std::string sPattern3;
          if ( sPattern1 == sPattern2 ) {
            sPattern3 = sPattern1;
          }
          else {
            sPattern3 = sPattern1 + "," + sPattern2;
          }
          ix = kwmExchanges.FindMatch( sPattern3 );
          if ( ( 0 == ix ) || ( sPattern3.length() != vSymbolsPerExchange[ ix ].s.length() ) ) {
            std::cout << "Adding Exchange " << sPattern3 << std::endl;
            size_t cnt = kwmExchanges.GetPatternCount();
            kwmExchanges.AddPattern( sPattern3, cnt );
            structCountPerString cps;
            vSymbolsPerExchange.push_back( cps );
            vSymbolsPerExchange[ cnt ].cnt = 1;
            vSymbolsPerExchange[ cnt ].s = sPattern3;
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

          pBegin = dbRecord.line + offset[5];  // SIC
          pEnd = pBegin + count[5];
          b = parse( pBegin, pEnd, ruleSIC );
          if ( b && ( pBegin == pEnd ) ) {
            if ( 0 != dbRecord.SIC ) {
              ++cntSIC;
            }
          }

          pBegin = dbRecord.line + offset[7];  // NAICS
          pEnd = pBegin + count[7];
          b = parse( pBegin, pEnd, ruleNAICS );
          if ( b && ( pBegin == pEnd ) ) {  
            if ( 0 != dbRecord.NAICS ) {
              ++cntNAICS;
            }
          }
        }
        
        // parse out contract expiry information
        if ( structSymbolRecord::Future == dbRecord.eInstrumentType ) {
          if ( 'Y' == *(dbRecord.line + offset[6]) ) {
            dbRecord.sc.set( structSymbolRecord::FrontMonth );
          }
          boost::cmatch what;
          if ( boost::regex_search( dbRecord.line + offset[1], what, rxFuture ) ) {
            std::string sMonth( what[1].first, what[1].second );
            std::string sYear( what[2].first, what[2].second );
            dbRecord.nMonth = DecodeMonth( sMonth );
            dbRecord.nYear = atoi( sYear.c_str() );
          }
          else {  // No future match on HOX8-HOJ9, HEATING OIL #2 NOV 08/APR 09
            if ( boost::regex_search( dbRecord.line + offset[1], what, rxFuture2 ) ) {
              // just ignore the double future set
            }
            else {
              std::cout << "No future match on " << dbRecord.line << ", " << dbRecord.line + offset[1] << std::endl;
            }
          }
        }

        // parse out contract information
        if ( structSymbolRecord::IEOption == dbRecord.eInstrumentType ) {
          if ( 'Y' == *(dbRecord.line + offset[6]) ) {
            dbRecord.sc.set( structSymbolRecord::FrontMonth );
          }
          char* pBegin = dbRecord.line + offset[1];
          char* pEnd = pBegin + count[1];
          bool b = parse( pBegin, pEnd, ruleOption );
          if ( b && ( pBegin == pEnd ) ) {
            if ( 0 == sUnderlying.size() ) {
              std::cout << "Zero length underlying for " << dbRecord.line << std::endl;
            }
            else {
              mapUnderlying[ sUnderlying ] = 1;  // simply create an entry for later use
            }
            nUnderlyingSize = max( nUnderlyingSize, sUnderlying.size() );
          }
          else {
            std::cout  << "No option match on " << dbRecord.line << ", " << dbRecord.line + offset[1] << std::endl;
          }
        }

        // update database
        Dbt key( dbRecord.line, dbRecord.cnt[0] );
        assert( dbRecord.bufferedlength <= 255 );
        Dbt value( &dbRecord, dbRecord.bufferedlength );
        int ret;
        try {
          ret = m_pdbSymbols->put( 0, &key, &value, DB_NOOVERWRITE );
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

    for ( size_t ix = 0; ix < vSymbolsPerExchange.size(); ++ix ) {
      std::cout << vSymbolsPerExchange[ ix ].s << "=" << vSymbolsPerExchange[ ix ].cnt << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Symbol List Complete" << std::endl;

#ifdef _DEBUG
  std::stringstream ss;
  ss << "#kwmExchanges nodes " << kwmExchanges.GetNodeCount() << std::endl;
  ss << "#kwmSymbolType nodes " << kwmSymbolType.GetNodeCount() << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
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
