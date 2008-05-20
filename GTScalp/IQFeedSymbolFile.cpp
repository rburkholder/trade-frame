#include "StdAfx.h"

#include "IQFeedSymbolFile.h"
//#include "BerkeleyDb.h"
#include "DataManager.h"
#include <fstream>
#include <iostream>
#include <map>
#include <boost/regex.hpp> 

using namespace std;

//	"select symbol from iqSymbols where "  
	//		+ " exchange in ('HOTSPOT', 'ONECH', 'NYSE', 'AMEX', 'NMS', 'CBOT', 'CME', 'DTN' )"
			// NASDAQ
			//+ " ( exchange='HOTSPOT'"
			//+ " or exchange='NASDAQ' or exchange='NYSE' or exchange='AMEX' or exchange='NMS' "
			//+ " or exchange='DTN' )"  // having a few problems with DTN (try again sometime)
//			+ " and ismutualfund = 0 and ismoneymarketfund = 0 and symbol not like 'RFC.%'"  // RFC causes abort in iqconnect

//select symbol, exchange, descr from iqsymbols where exchange in ('NMS', 'NYSE', 'AMEX')
	//			and 0=isindex and 0=iscboeindex and 0=ismutualfund and 0=ismoneymarketfund";


// symbolstats

// determine which are optionable symbols

//		string sCmd = "select distinct a.symbol from iqrootoptionsymbols as a, iqsymbols as b"
//			+ " where a.symbol=b.symbol and b.isindex=0 and b.iscboeindex=0 and b.isindicator=0";

// http://www.dtniq.com/product/mktsymbols.zip

// need a mechanism to detect lock files and then to eliminate them

CIQFeedSymbolFile::CIQFeedSymbolFile(void) : 
    pRecord( NULL ), 
    m_pdbIQFSymbols( NULL ), 
    m_pdbIxIQFSymbols_Market( NULL ), m_pdbcIxIQFSymbols_Market( NULL ),
    m_pdbIxIQFSymbols_Underlying( NULL ), m_pdbcIxIQFSymbols_Underlying( NULL ) 
    {
}

CIQFeedSymbolFile::~CIQFeedSymbolFile(void) {
  //if ( NULL != m_pdbcIxIQFSymbols_Market ) delete m_pdbcIxIQFSymbols_Market;
  //if ( NULL != m_pdbIxIQFSymbols_Market ) delete m_pdbIxIQFSymbols_Market;
  //if ( NULL != m_pdbIQFSymbols ) delete m_pdbIQFSymbols;
}

void CIQFeedSymbolFile::Open() {
  CDataManager dm;
  DbEnv *pDbEnv = dm.GetDbEnv();

  // open/create main symbol table
  m_pdbIQFSymbols = new Db( pDbEnv, 0 );
  m_pdbIQFSymbols->open( NULL, dm.GetBDBFileName(), "IQFSymbols", DB_BTREE, DB_CREATE, 0 );

  // open/create the market index
  m_pdbIxIQFSymbols_Market = new Db( pDbEnv, 0 );
  m_pdbIxIQFSymbols_Market->set_flags( DB_DUPSORT );
  m_pdbIxIQFSymbols_Market->open( NULL, dm.GetBDBFileName(), "IxIQFSymbols_Market", DB_BTREE, DB_CREATE, 0 );

  // associate the index with the main table
  m_pdbIQFSymbols->associate( NULL, m_pdbIxIQFSymbols_Market, &CIQFeedSymbolFile::GetMarketName, 0 );

  // open/create the underlying index
  m_pdbIxIQFSymbols_Underlying = new Db( pDbEnv, 0 );
  m_pdbIxIQFSymbols_Underlying->set_flags( DB_DUPSORT );
  m_pdbIxIQFSymbols_Underlying->open( NULL, dm.GetBDBFileName(), "IxIQFSymbols_Underlying", DB_BTREE, DB_CREATE, 0 );

  // associate the index with the main table
  m_pdbIQFSymbols->associate( NULL, m_pdbIxIQFSymbols_Underlying, &CIQFeedSymbolFile::GetUnderlyingName, 0 );
}

void CIQFeedSymbolFile::Close() {
  m_pdbIxIQFSymbols_Underlying->close(0);
  m_pdbIxIQFSymbols_Market->close(0);
  m_pdbIQFSymbols->close(0);
}

int CIQFeedSymbolFile::GetMarketName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey ) {
  structIQFSymbolRecord *dbIxRecord = (structIQFSymbolRecord *) data->get_data();
  char *p = dbIxRecord->line + dbIxRecord->ix[2];  // get at the 'exchange' string
  unsigned long l = dbIxRecord->cnt[2];  // set the key and its length
  secKey->set_data( p );
  secKey->set_size( l );
  return 0;
}

int CIQFeedSymbolFile::GetUnderlyingName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey ) {
  structIQFSymbolRecord *dbIxRecord = (structIQFSymbolRecord *) data->get_data();
  char *p; 
  u_int32_t len;
  bool bUseIndex = true;
  if ( 0 != ( dbIxRecord->ucBits2 & ucOption ) ) { // OPRA option
    p = dbIxRecord->line + dbIxRecord->ix[1];  // start of description
    char *e = strchr( p, ' ' );  // find the trailing blank
    len = e - p;
    if ( 0 != len ) bUseIndex = false;
  }
  if ( bUseIndex ) {  // by default, use records symbol
    p = dbIxRecord->line;
    len = dbIxRecord->cnt[0];
  }
  secKey->set_data( p );
  secKey->set_size( len );
  return 0;
}

void CIQFeedSymbolFile::SetSearchExchange( const char *szExchange ) {
  m_szSearchKey = szExchange;
  m_lenSearchKey = strlen( szExchange );
  m_dbtKey.set_data( (void*) szExchange );
  m_dbtKey.set_size( m_lenSearchKey );
  m_pdbIxIQFSymbols_Market->cursor( NULL, &m_pdbcIxIQFSymbols_Market, 0 );
}

void CIQFeedSymbolFile::SetSearchUnderlying( const char *szUnderlying ) {
  m_szSearchKey = szUnderlying;
  m_lenSearchKey = strlen( szUnderlying );
  m_dbtKey.set_data( (void*) szUnderlying );
  m_dbtKey.set_size( m_lenSearchKey );
  m_pdbIxIQFSymbols_Underlying->cursor( NULL, &m_pdbcIxIQFSymbols_Underlying, 0 );
}

bool CIQFeedSymbolFile::RetrieveSymbolRecord( u_int32_t flags ) {
  pRecord = NULL;
  int result = m_pdbcIxIQFSymbols_Market->get( &m_dbtKey, &m_dbtData, flags );
  if ( 0 == result ) {
    pRecord = (structIQFSymbolRecord *) m_dbtData.get_data();
    UnPackBoolean( pRecord->ucBits1, pRecord->ucBits2 );
  }
  return ( 0 == result );
}

void CIQFeedSymbolFile::EndSearch( void ) {
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
  unsigned short month = 0;
  if ( s == "JAN" ) month = 1;
  if ( s == "FEB" ) month = 2;
  if ( s == "MAR" ) month = 3;
  if ( s == "APR" ) month = 4;
  if ( s == "MAY" ) month = 5;
  if ( s == "JUN" ) month = 6;
  if ( s == "JUL" ) month = 7;
  if ( s == "AUG" ) month = 8;
  if ( s == "SEP" ) month = 9;
  if ( s == "OCT" ) month = 10;
  if ( s == "NOV" ) month = 11;
  if ( s == "DEC" ) month = 12;
  assert( month > 0 );
  return month;
}

bool CIQFeedSymbolFile::Load( const string &filename ) {
  // mktsymbols.txt

  // TODO:  need to prevent re-entrant execution:  ie, turn invoking button off
  // TODO:  need to put in back ground thread, and lock out other access processes while running

  static const boost::regex rxMutual(".{3}[^\\.]X$");
  static const boost::regex rxMoneyMkt(".{3}XX$");
  static const boost::regex rxIndex(".+\\.X$");
  static const boost::regex rxCboe(".+\\.XO$");
  static const boost::regex rxIndicator(".+\\.Z$");

  static const boost::regex rxOption( "^([A-Z]+){0,1}[ ]([A-Z]{3}) ([0-9]{4}) ([CP]) ([0-9]+[.][0-9]+)" ); //GM SEP 2008 P 10.000
  static const boost::regex rxFuture( "[[:blank:]](JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]]{1}([0-9]{4})" );  // GOOG1CQ07	GOOG AUG 2007	ONECH
  static const boost::regex rxFuture2( "(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}/(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)[[:blank:]][0-9]{2}" ); //     No future match on HOX8-HOJ9, HEATING OIL #2 NOV 08/APR 09

  unsigned int cntMutual = 0, cntMoneyMkt = 0, cntIndex = 0, cntCboe = 0, cntIndicator = 0;
  map<const char *, unsigned int, LessThan> map_SymbolsPerExchange;
  typedef pair<const char *, unsigned int> pair_SymbolsPerExchange;
  map<const char *, unsigned int, LessThan>::iterator map_iter_SymbolsPerExchange;

  unsigned int cntStock = 0, cntFuture = 0, cntOption = 0, cntFuturesOption = 0, cntCurrency = 0, cntBond = 0, cntETF = 0, cntMetals = 0;
  unsigned short nUnderlyingSize = 0;

  ifstream file;
  unsigned long cntLines = 0;
  td_structIndexes j, k, c; 
  bool bEndFound;

  cout << "Opening Symbol Database" << endl;
  file.open( filename.c_str() );

  Open();
  u_int32_t countp = 0;
  cout << "Truncating Symbol Database" << endl;
  m_pdbIQFSymbols->truncate( NULL, &countp, 0 );

  cout << "Loading Symbols" << endl;
  try {
    file.getline( dbRecord.line, nMaxBufferSize );  // remove header line
    file.getline( dbRecord.line, nMaxBufferSize );
    while ( !file.fail() ) {
      ++cntLines;
      bEndFound = false;
      c = j = k = 0; // c is count of char in string, j index into ix, k index into line
      dbRecord.ucBits1 = dbRecord.ucBits2 = 0;  // initialize bits to zero
      UnPackBoolean( dbRecord.ucBits1, dbRecord.ucBits2 );
      dbRecord.fltStrike = 0;
      dbRecord.nYear = 0;
      dbRecord.nMonth = 0;
      dbRecord.chDirection = ' ';
      dbRecord.ix[j] = k; // index of first string is 0
      while ( !bEndFound ) {
        if ( 0 == dbRecord.line[k] ) {
          dbRecord.cnt[j] = c; // size of last string
          bEndFound = true;
        }
        else {
          if ( '\t' != dbRecord.line[k] ) { 
            ++c;
          }
          else {
            dbRecord.cnt[j] = c;
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
      dbRecord.bufferedlength = sizeof( structIQFSymbolRecord ) - nMaxBufferSize + k + 1;  // +1 is for zero offset of k
      // process line here

      // count up number of symbols per exchange
      map_iter_SymbolsPerExchange = map_SymbolsPerExchange.find( dbRecord.line + dbRecord.ix[2] );
      if ( map_SymbolsPerExchange.end() != map_iter_SymbolsPerExchange ) {
        ++(map_iter_SymbolsPerExchange->second);
      }
      else {
        char *sExchange = new char[ dbRecord.cnt[2] + 1 ];
        strcpy( sExchange, dbRecord.line + dbRecord.ix[2] );
        map_SymbolsPerExchange.insert( pair_SymbolsPerExchange( sExchange, 1 ) );
      }

      // assign instrument types
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "WCE" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "TULLETT" ) ) { m_bCurrency = true; ++cntCurrency; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "TSE" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "SMCAP" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "SGX" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "PSE" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "PBOT" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "OTCBB" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "OTC" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "OPRA" ) ) { m_bOption = true; ++cntOption; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "ONECH" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "NYSE" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "NYMEXMINI" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "NYMEX" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "NYBOT" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "NMS" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "NASDAQ" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "MGE" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "LME" ) ) { m_bMetals = true; ++cntMetals; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "KCBOT" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "IPE" ) ) { m_bFuture = true; ++cntFuture; };
      //if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "EUREXNDX" ) ) {  = true; ++; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "EUREX" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "ENIR" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "ENID" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "ENCOM" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "DTN" ) ) { m_bIndex = true; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "DJ" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "CVE" ) ) { m_bStock = true; ++cntStock; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "COMMEX" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "CME" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "CHX" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "CBOT" ) ) { m_bFuture = true; ++cntFuture; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "BARCLAYS" ) ) { m_bCurrency = true; ++cntCurrency; };
      if ( 0 == strcmp( dbRecord.line + dbRecord.ix[2], "AMEX" ) ) { m_bStock = true; ++cntStock; };

      // parse out contract expiry information
      if ( m_bFuture ) {
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
      if ( m_bOption ) {
        boost::cmatch what;
        if ( boost::regex_search( dbRecord.line + dbRecord.ix[1], what, rxOption, boost::match_default ) ) {
          std::string sUnderlying( what[1].first, what[1].second );
          nUnderlyingSize = max( nUnderlyingSize, sUnderlying.size() );
          std::string sMonth( what[2].first, what[2].second );
          std::string sYear( what[3].first, what[3].second );
          dbRecord.chDirection = *what[4].first;
          std::string sStrike( what[5].first, what[5].second );
          dbRecord.nMonth = DecodeMonth( sMonth );
          dbRecord.nYear = atoi( sYear.c_str() );
          dbRecord.fltStrike = atof( sStrike.c_str() );
        }
        else {
          std::cout  << "No option match on " << dbRecord.line << ", " << dbRecord.line + dbRecord.ix[1] << std::endl;
        }
      }

      // identify the symbol types
      m_bMutual = m_bMoneyMkt = m_bIndex = m_bCboe = m_bIndicator = false;
      if ( boost::regex_match( dbRecord.line, rxMutual ) ) {
        m_bMutual = true;
        ++cntMutual;
      }
      if ( boost::regex_match( dbRecord.line, rxMoneyMkt ) ) {
        m_bMoneyMkt = true;
        ++cntMoneyMkt;
      }
      if ( boost::regex_match( dbRecord.line, rxIndex ) ) {
        m_bIndex = true;
        ++cntIndex;
      }
      if ( boost::regex_match( dbRecord.line, rxCboe ) ) {
        m_bCboe = true;
        ++cntCboe;
      }
      if ( boost::regex_match( dbRecord.line, rxIndicator ) ) {
        m_bIndicator = true;
        ++cntIndicator;
      }
      PackBoolean();

      // update database
      //Dbt key( dbRecord.line, dbRecord.cnt[0]+1 );
      Dbt key( dbRecord.line, dbRecord.cnt[0] );
      assert( dbRecord.bufferedlength <= 255 );
      Dbt value( &dbRecord, dbRecord.bufferedlength );
      int ret = m_pdbIQFSymbols->put( 0, &key, &value, DB_NOOVERWRITE );
      // get next line from text file
      file.getline( dbRecord.line, nMaxBufferSize );
    }
  }
  catch( DbException &dbex ) {
    cout << "IQFSymbols exception: " << dbex.what() << endl;
  }

  Close();
  file.close();

  cout << "Count Mutual:     " << cntMutual << endl;
  cout << "Count MoneyMkt:   " << cntMoneyMkt << endl;
  cout << "Count Index:      " << cntIndex << endl;
  cout << "Count CBOE:       " << cntCboe << endl;
  cout << "Count Indicator:  " << cntIndicator << endl;
  cout << "Count Currency:   " << cntCurrency << endl;
  cout << "Count Stock:      " << cntStock << endl;
  cout << "Count Future:     " << cntFuture << endl;
  cout << "Count Option:     " << cntOption << endl;
  cout << "Count Metals:     " << cntMetals << endl;
  cout << "Max Underlying    " << nUnderlyingSize << endl;

  map_iter_SymbolsPerExchange = map_SymbolsPerExchange.begin();
  while ( map_SymbolsPerExchange.end() != map_iter_SymbolsPerExchange ) {
    cout << map_iter_SymbolsPerExchange->first << "=" << map_iter_SymbolsPerExchange->second << endl;
    delete [] map_iter_SymbolsPerExchange->first;
    ++map_iter_SymbolsPerExchange;
  }
  map_SymbolsPerExchange.clear();

  return true;
}

void CIQFeedSymbolFile::PackBoolean( void ) {
  dbRecord.ucBits1 = dbRecord.ucBits2 = 0;
  dbRecord.ucBits1 |= ( m_bMutual ? ucMutual : 0 );
  dbRecord.ucBits1 |= ( m_bMoneyMkt ? ucMoneyMkt : 0 );
  dbRecord.ucBits1 |= ( m_bIndex ? ucIndex : 0 );
  dbRecord.ucBits1 |= ( m_bCboe ? ucCboe : 0 );
  dbRecord.ucBits1 |= ( m_bIndicator ? ucIndicator : 0 );
  dbRecord.ucBits1 |= ( m_bHasOptions ? ucHasOptions : 0 );
  dbRecord.ucBits2 |= ( m_bStock ? ucStock : 0 );
  dbRecord.ucBits2 |= ( m_bFuture ? ucFuture : 0 );
  dbRecord.ucBits2 |= ( m_bOption ? ucOption : 0 );
  dbRecord.ucBits2 |= ( m_bFuturesOption ? ucFuturesOption : 0 );
  dbRecord.ucBits2 |= ( m_bCurrency ? ucCurrency : 0 );
  dbRecord.ucBits2 |= ( m_bBond ? ucBond : 0 );
  dbRecord.ucBits2 |= ( m_bETF ? ucETF : 0 );
  dbRecord.ucBits2 |= ( m_bMetals ? ucMetals : 0 );

}

void CIQFeedSymbolFile::UnPackBoolean( const unsigned char ucBits1, const unsigned char ucBits2 ) {
  m_bMutual = 0 != ( ucBits1 & ucMutual );
  m_bMoneyMkt = 0 != ( ucBits1 & ucMoneyMkt );
  m_bIndex = 0 != ( ucBits1 & ucIndex );
  m_bCboe = 0 != ( ucBits1 & ucCboe );
  m_bIndicator = 0 != ( ucBits1 & ucIndicator );
  m_bHasOptions = 0 != ( ucBits1 & ucHasOptions );
  m_bStock = 0 != ( ucBits2 & ucStock );
  m_bFuture = 0 != ( ucBits2 & ucFuture );
  m_bOption = 0 != ( ucBits2 & ucOption );
  m_bFuturesOption = 0 != ( ucBits2 & ucFuturesOption );
  m_bCurrency = 0 != ( ucBits2 & ucCurrency );
  m_bBond = 0 != ( ucBits2 & ucBond );
  m_bETF = 0 != ( ucBits2 & ucETF );
  m_bMetals = 0 != ( ucBits2 & ucMetals );
}

