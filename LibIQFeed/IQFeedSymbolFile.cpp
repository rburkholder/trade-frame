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

//#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/spirit/include/phoenix_bind.hpp>
//#include <boost/spirit/include/phoenix_operator.hpp>

//namespace qi = boost::spirit::qi;
//namespace ascii = boost::spirit::ascii;

CIQFeedSymbolFile::structExchangeInfo CIQFeedSymbolFile::m_rExchanges[] = {
  { "Unknown", InstrumentType::Unknown },  // needs to be first in table
  //  { "AMEX", InstrumentType::Stock },
  //{ "ARCA", InstrumentType::Stock },  //
  { "BARCLAYS", InstrumentType::Currency },
  { "BMF", InstrumentType::Future },  //
  { "CANADIAN", InstrumentType::Stock },
  { "CBOE", InstrumentType::Future },
  { "CBOT", InstrumentType::Future },
  //{ "CFE", InstrumentType::Future },  //
  { "CHX", InstrumentType::Future },
  { "CLEARPORT", InstrumentType::Future },
  { "CME", InstrumentType::Future },
  { "CME_GBX", InstrumentType::FuturesOption },
  { "COMEX", InstrumentType::Future },
  //{ "CVE", InstrumentType::Stock },
  //{ "DJ", InstrumentType::Index },
  { "DME", InstrumentType::Future },  //
  { "DTN", InstrumentType::Index },
  //{ "ENCOM", InstrumentType::Future },
  //{ "ENID", InstrumentType::Future },
  //{ "ENIR", InstrumentType::Future },
  { "EUREX", InstrumentType::Future },
  //{ "EUREXNDX", InstrumentType::Currency },
  { "FOREX", InstrumentType::Currency },
  { "FXCM", InstrumentType::Currency },
  { "GREENX", InstrumentType::Future },  //
  { "ICE", InstrumentType::Future },  //
  //{ "ICEFC", InstrumentType::Future },  //
  //{ "ICEFE", InstrumentType::Future },  //
  //{ "ICEFI", InstrumentType::Future },  //
  //{ "ICEFU", InstrumentType::Future },  //
  //  { "IPE", InstrumentType::Future },
  { "KCBOT", InstrumentType::Future },
  { "LME", InstrumentType::Metal },
  { "MDEX", InstrumentType::Future },  //
  { "MGE", InstrumentType::Future },
  { "MX", InstrumentType::Future },  //
  { "NASDAQ", InstrumentType::Stock },
  //{ "NMS", InstrumentType::Stock },
  //  { "NSX", InstrumentType::Future },
  //  { "NYBOT", InstrumentType::Future },
  //{ "NYLCD", InstrumentType::Future },
  //{ "NYLED", InstrumentType::Future },
  //{ "NYLID", InstrumentType::Future },
  //{ "NYLMD", InstrumentType::Future },
  { "NYMEX", InstrumentType::Future },
  //{ "NYMEXMINI", InstrumentType::Future },
  { "NYSE", InstrumentType::Stock },
  //  { "ONECH", InstrumentType::Future },
  { "OPRA", InstrumentType::Option },
  //{ "OTC", InstrumentType::Stock },
  //{ "OTCBB", InstrumentType::Stock },
  //{ "PBOT", InstrumentType::Future },
  //  { "PSE", InstrumentType::Stock },
  { "SGX", InstrumentType::Future },
  //{ "SMCAP", InstrumentType::Stock },
  //{ "TENFORE", InstrumentType::Currency }, 
  //{ "TSE", InstrumentType::Stock },
  //{ "TULLETT", InstrumentType::Currency },
  //  { "WCE", InstrumentType::Future },
  //{ "WTB", InstrumentType::Future }  //
};

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

unsigned short DecodeMonth( const std::string &s ) {
  unsigned short month2 = 0;
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
  // TODO:  need to put in back ground thread, and lock out other access processes while running

  static const boost::regex rxMutual(".{3}[^\\.]X$");
  static const boost::regex rxMoneyMkt(".{3}XX$");
  static const boost::regex rxIndex(".+\\.X$");
  static const boost::regex rxCboe(".+\\.XO$");
  static const boost::regex rxIndicator(".+\\.Z$");
  static const boost::regex rxNotAStock("([A-Z]+[0-9]+[A-Z]*)|([A-Z]+\\.[A-Z]+)");

  static const boost::regex rxOption( "^([A-Z]+){0,1}[ ]([A-Z]{3}) ([0-9]{4}) ([CP]) ([0-9]+[.][0-9]+)" ); //GM SEP 2008 P 10.000
  static const boost::regex rxOptionName( "^[A-Z]{1,3} [A-Z][A-Z]$" );
  static const boost::regex rxFuture( "[[:blank:]](JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]]{1}([0-9]{4})" );  // GOOG1CQ07	GOOG AUG 2007	ONECH
  static const boost::regex rxFuture2( "(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}/(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}" ); //     No future match on HOX8-HOJ9, HEATING OIL #2 NOV 08/APR 09

  unsigned int cntMutual = 0, cntMoneyMkt = 0, cntIndex = 0, cntCboe = 0, cntIndicator = 0, cntNotAStock = 0;
  unsigned short nUnderlyingSize = 0;

  std::ifstream file;

  size_t cntLines = 0;
  CInstrumentFile::structIndexes_t j, k, c; 
  bool bEndFound;

  std::map<std::string, unsigned long> mapUnderlying;  // keeps track of optionable symbols, to fix bool at end
  CKeyWordMatch<size_t> kwm( 0, 300 );  // about 300 characters?  ... fast look up of index into m_rExchanges, possibly faster than std::map

  OutputDebugString( "Initializing Structures\n" );

  size_t cntExchanges = sizeof( m_rExchanges ) / sizeof( structExchangeInfo );
  std::vector<size_t> vcntInstrumentsPerExchange( cntExchanges );

  for ( size_t ix = 0; ix < cntExchanges; ++ix ) {
    vcntInstrumentsPerExchange[ ix ] = 0;
    kwm.AddPattern( m_rExchanges[ ix ].szName, ix );
  }

#ifdef _DEBUG
  std::stringstream ss;
  ss << "kwm size is " << kwm.size() << std::endl;
  OutputDebugString( ss.str().c_str() );
  ss.str() = "";
#endif

  size_t rcntContractTypes[ InstrumentType::_Count ];  // keep a record of the count of Instrument Types
  for ( size_t ix = 0; ix < InstrumentType::_Count; ++ix ) {
    rcntContractTypes[ ix ] = 0;
  }

  // convert to db stuff later
//  const size_t nMaxBufferSize = 1000;
//  char rBuffer[ nMaxBufferSize ];

  try {

    OutputDebugString( "Opening Instrument Database ... " );
    OpenIQFSymbols();
    u_int32_t countp = 0;
    OutputDebugString( "truncating ... " );
    m_pdbSymbols->truncate( NULL, &countp, 0 );
    OutputDebugString( "done.\n" );

    OutputDebugString( "Opening Input Instrument File " );
    OutputDebugString( filename.c_str() );
    OutputDebugString( " ... " );
    file.open( filename.c_str() );
    OutputDebugString( "\n" );

    OutputDebugString( "Loading Symbols\n" );

    //file.getline( rBuffer, nMaxBufferSize );  // remove header line
    //file.getline( rBuffer, nMaxBufferSize ); // first data line to preload
    file.getline( dbRecord.line, nMaxBufferSize );  // remove header line
    file.getline( dbRecord.line, nMaxBufferSize );
    while ( !file.fail() ) {
      ++cntLines;  // number data lines processed

      bEndFound = false;
      c = j = k = 0; // c is count of char in string, j index into ix, k index into line

      // load line for next trip through loop
      dbRecord.sc.reset();  // initialize bits to zero
//      UnPackBoolean( dbRecord.ucBits1 );
      dbRecord.fltStrike = 0;
      dbRecord.nYear = 0;
      dbRecord.nMonth = 0;
      dbRecord.nOptionSide = OptionSide::Unknown;
      dbRecord.ix[j] = k; // index of first string is 0
      bool bSizeError = false;
      while ( !bEndFound ) {
        if ( 0 == dbRecord.line[k] ) {
          dbRecord.cnt[j] = c; // size of last string
          if ( 0 == c ) bSizeError = true;
          bEndFound = true;
        }
        else {
          if ( '\t' != dbRecord.line[k] ) { 
            ++c;
          }
          else {
            dbRecord.cnt[j] = c;
            if ( 0 == c ) bSizeError = true;
            c = 0;
            ++j;
            if ( j >= 3 ) {
              bEndFound = true;
            }
            else {
              dbRecord.ix[j] = k+1; // beginning of new string
            }
            dbRecord.line[k] = 0;  //force a string end
          }
          ++k;
        }
      }

      if ( bSizeError ) {
        std::cout << "Field length error for:  " << dbRecord.line << std::endl;
      }
      else {

        dbRecord.bufferedlength = sizeof( structSymbolRecord ) - nMaxBufferSize + k + 1;  // +1 is for zero offset of k
        // process line here

        size_t ix;
        try {
          ix = (size_t) kwm.FindMatch( dbRecord.line + dbRecord.ix[2] );
          if ( 0 == ix ) {
            std::cout << "Unknown Exchange for:  " << dbRecord.line << std::endl;
          }
          dbRecord.eInstrumentType = m_rExchanges[ ix ].eInstrumentType;
          ++m_rExchanges[ ix ].cntInstruments;
        }
        catch ( std::exception e ) {
          std::cout << dbRecord.line << ": zero length pattern" << std::endl;
          dbRecord.eInstrumentType = InstrumentType::Unknown;
          ++m_rExchanges[ 0 ].cntInstruments;
        }
        
        ++rcntContractTypes[ dbRecord.eInstrumentType ];
        if ( InstrumentType::Unknown == dbRecord.eInstrumentType ) {
          std::cout << "Unknown contract for: " << dbRecord.line << ", " << dbRecord.line + dbRecord.ix[2] << std::endl;
        }
        // parse out contract expiry information
        if ( InstrumentType::Future == dbRecord.eInstrumentType ) {
          boost::cmatch what;
          if ( boost::regex_search( dbRecord.line + dbRecord.ix[1], what, rxFuture ) ) {
            std::string sMonth( what[1].first, what[1].second );
            std::string sYear( what[2].first, what[2].second );
            dbRecord.nMonth = DecodeMonth( sMonth );
            dbRecord.nYear = atoi( sYear.c_str() );
          }
          else {  // No future match on HOX8-HOJ9, HEATING OIL #2 NOV 08/APR 09
            if ( boost::regex_search( dbRecord.line + dbRecord.ix[1], what, rxFuture2 ) ) {
              // just ignore the double future set
            }
            else {
              std::cout << "No future match on " << dbRecord.line << ", " << dbRecord.line + dbRecord.ix[1] << std::endl;
            }
          }
        }

        // parse out contract information
        if ( InstrumentType::Option == dbRecord.eInstrumentType ) {
          boost::cmatch what1, what2;
          if ( boost::regex_search( dbRecord.line + dbRecord.ix[1], what1, rxOption, boost::match_default )
            && boost::regex_search( dbRecord.line, what2, rxOptionName, boost::match_default )
            ) {
            std::string sUnderlying( what1[1].first, what1[1].second );
            if ( 0 == sUnderlying.size() ) {
              std::cout << "Zero length underlying for " << dbRecord.line << std::endl;
            }
            else {
              mapUnderlying[ sUnderlying ] = 1;  // simply create an entry for later use
            }
            nUnderlyingSize = max( nUnderlyingSize, sUnderlying.size() );
            std::string sMonth( what1[2].first, what1[2].second );
            std::string sYear( what1[3].first, what1[3].second );
            dbRecord.nOptionSide = OptionSide::Unknown;
            if ( 'P' == *what1[4].first ) dbRecord.nOptionSide = OptionSide::Put;
            if ( 'C' == *what1[4].first ) dbRecord.nOptionSide = OptionSide::Call;
            //dbRecord.chDirection = *what[4].first;
            std::string sStrike( what1[5].first, what1[5].second );
            dbRecord.nMonth = DecodeMonth( sMonth );
            dbRecord.nYear = atoi( sYear.c_str() );
            dbRecord.fltStrike = atof( sStrike.c_str() );
            
          }
          else {
            std::cout  << "No option match on " << dbRecord.line << ", " << dbRecord.line + dbRecord.ix[1] << std::endl;
          }
        }

        // identify the symbol types
        CInstrumentFile::bitsSymbolClassifier_t sc;
        dbRecord.sc.reset();
        //m_bMutual = m_bMoneyMkt = m_bIndex = m_bCboe = m_bIndicator = m_bNotAStock= false;
        if ( boost::regex_match( dbRecord.line, rxMutual ) ) {
          dbRecord.sc.set( CInstrumentFile::Mutual );
          //m_bMutual = true;
          ++cntMutual;
        }
        if ( boost::regex_match( dbRecord.line, rxMoneyMkt ) ) {
          dbRecord.sc.set( CInstrumentFile::MoneyMarket );
          //m_bMoneyMkt = true;
          ++cntMoneyMkt;
        }
        if ( boost::regex_match( dbRecord.line, rxIndex ) ) {
          dbRecord.sc.set( CInstrumentFile::Index );
          //m_bIndex = true;
          ++cntIndex;
        }
        if ( boost::regex_match( dbRecord.line, rxCboe ) ) {
          dbRecord.sc.set( CInstrumentFile::CBOE );
          //m_bCboe = true;
          ++cntCboe;
        }
        if ( boost::regex_match( dbRecord.line, rxIndicator ) ) {
          dbRecord.sc.set( CInstrumentFile::Indicator );
          //m_bIndicator = true;
          ++cntIndicator;
        }
        if ( boost::regex_match( dbRecord.line, rxNotAStock ) ) {
          dbRecord.sc.set( CInstrumentFile::NotAStock );
          //m_bNotAStock = true;
          ++cntNotAStock;
        }
        //PackBoolean();
        
        // update database
        //Dbt key( dbRecord.line, dbRecord.cnt[0]+1 );
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
      file.getline( dbRecord.line, nMaxBufferSize );
      //file.getline( rBuffer, nMaxBufferSize ); // first data line to preload
    }

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
        rec.sc.set( CInstrumentFile::HasOptions );
        //rec.ucBits1 |= ucHasOptions;  // set the bit
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

    std::cout << "Count Mutual:     " << cntMutual << std::endl;
    std::cout << "Count MoneyMkt:   " << cntMoneyMkt << std::endl;
    std::cout << "Count Index:      " << cntIndex << std::endl;
    std::cout << "Count CBOE:       " << cntCboe << std::endl;
    std::cout << "Count Indicator:  " << cntIndicator << std::endl;
    std::cout << "Count NotAStock   " << cntNotAStock << std::endl;
    std::cout << "Count Optionables " << mapUnderlying.size() << std::endl;
    std::cout << "Max Underlying    " << nUnderlyingSize << std::endl;

    for ( unsigned long ix = 0; ix < sizeof( m_rExchanges ) / sizeof( structExchangeInfo ); ++ix ) {
      std::cout << m_rExchanges[ ix ].szName << "=" << m_rExchanges[ ix ].cntInstruments << std::endl;
    }
    std::cout << "Contract Types" << std::endl;
    for ( unsigned long ix = 0; ix < InstrumentType::_Count; ++ix ) {
      std::cout << ix << "=" << rcntContractTypes[ ix ] << std::endl;
    }


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
