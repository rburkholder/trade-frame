#include "StdAfx.h"

#include "IQFeedSymbolFile.h"

#include <LibCommon/KeywordMatch.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

CIQFeedSymbolFile::structExchangeInfo CIQFeedSymbolFile::m_rExchanges[] = {
  { "Unknown", InstrumentType::Unknown },  // needs to be first in table
  //  { "AMEX", InstrumentType::Stock },
  { "ARCA", InstrumentType::Stock },  //
  { "BARCLAYS", InstrumentType::Currency },
  { "BMF", InstrumentType::Future },  //
  { "CBOT", InstrumentType::Future },
  { "CFE", InstrumentType::Future },  //
  { "CHX", InstrumentType::Future },
  { "CME", InstrumentType::Future },
  { "COMEX", InstrumentType::Future },
  { "CVE", InstrumentType::Stock },
  { "DJ", InstrumentType::Index },
  { "DME", InstrumentType::Future },  //
  { "DTN", InstrumentType::Index },
  { "ENCOM", InstrumentType::Future },
  { "ENID", InstrumentType::Future },
  { "ENIR", InstrumentType::Future },
  { "EUREX", InstrumentType::Future },
  { "EUREXNDX", InstrumentType::Currency },
  { "FXCM", InstrumentType::Currency },
  { "GREENX", InstrumentType::Future },  //
  { "ICEFC", InstrumentType::Future },  //
  { "ICEFE", InstrumentType::Future },  //
  { "ICEFI", InstrumentType::Future },  //
  { "ICEFU", InstrumentType::Future },  //
  //  { "IPE", InstrumentType::Future },
  { "KCBOT", InstrumentType::Future },
  { "LME", InstrumentType::Metal },
  { "MDEX", InstrumentType::Future },  //
  { "MGE", InstrumentType::Future },
  { "NASDAQ", InstrumentType::Stock },
  { "NMS", InstrumentType::Stock },
  //  { "NSX", InstrumentType::Future },
  //  { "NYBOT", InstrumentType::Future },
  { "NYLCD", InstrumentType::Future },
  { "NYLED", InstrumentType::Future },
  { "NYLID", InstrumentType::Future },
  { "NYLMD", InstrumentType::Future },
  { "NYMEX", InstrumentType::Future },
  { "NYMEXMINI", InstrumentType::Future },
  { "NYSE", InstrumentType::Stock },
  //  { "ONECH", InstrumentType::Future },
  { "OPRA", InstrumentType::Option },
  { "OTC", InstrumentType::Stock },
  { "OTCBB", InstrumentType::Stock },
  { "PBOT", InstrumentType::Future },
  //  { "PSE", InstrumentType::Stock },
  { "SGX", InstrumentType::Future },
  { "SMCAP", InstrumentType::Stock },
  { "TENFORE", InstrumentType::Currency }, 
  { "TSE", InstrumentType::Stock },
  { "TULLETT", InstrumentType::Currency },
  //  { "WCE", InstrumentType::Future },
  { "WTB", InstrumentType::Future }  //
};

CIQFeedSymbolFile::CIQFeedSymbolFile(void)
{
}

CIQFeedSymbolFile::~CIQFeedSymbolFile(void) {
}

bool CIQFeedSymbolFile::Load( const std::string &filename ) {
  // mktsymbols.txt

  // TODO:  need to prevent re-entrant execution:  ie, turn invoking button off
  // TODO:  need to put in back ground thread, and lock out other access processes while running

  unsigned int cntMutual = 0, cntMoneyMkt = 0, cntIndex = 0, cntCboe = 0, cntIndicator = 0, cntNotAStock = 0;
  unsigned short nUnderlyingSize = 0;

  std::ifstream file;
  size_t cntLines = 0;
//  td_structIndexes j, k, c; 
  //bool bEndFound;

  std::map<std::string, unsigned long> mapUnderlying;  // keeps track of optionable symbols, to fix bool at end
  CKeyWordMatch<unsigned long> kwm( 0, 300 );  // about 300 characters?

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

  size_t rcntContractTypes[ InstrumentType::_Count ];
  for ( size_t ix = 0; ix < InstrumentType::_Count; ++ix ) {
    rcntContractTypes[ ix ] = 0;
  }

  OutputDebugString( "Opening Input Instrument File " );
  OutputDebugString( filename );
  OutputDebugString( "\n" );

  file.open( filename.c_str() );

  OutputDebugString( "Loading Symbols\n" );

  // convert to db stuff later
  const size_t nMaxBufferSize = 1000;
  char rBuffer[ nMaxBufferSize ];

  try {
    file.getline( rBuffer, nMaxBufferSize );  // remove header line
    file.getline( rBuffer, nMaxBufferSize ); // first data line to preload
    while ( !file.fail() ) {
      ++cntLines;
    }

  }
  catch( ... ) {

  }


  return true;
}
