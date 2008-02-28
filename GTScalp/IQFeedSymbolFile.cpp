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
			//+ " or exchan
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

CIQFeedSymbolFile::CIQFeedSymbolFile(void) {
  m_pdbIQFSymbols = NULL;
  m_pdbIxIQFSymbols_Market = NULL;
  m_pdbcIxIQFSymbols_Market = NULL;
}

CIQFeedSymbolFile::~CIQFeedSymbolFile(void) {
  //if ( NULL != m_pdbcIxIQFSymbols_Market ) delete m_pdbcIxIQFSymbols_Market;
  //if ( NULL != m_pdbIxIQFSymbols_Market ) delete m_pdbIxIQFSymbols_Market;
  //if ( NULL != m_pdbIQFSymbols ) delete m_pdbIQFSymbols;
}

void CIQFeedSymbolFile::Open() {
  CDataManager dm;
  DbEnv *pDbEnv = dm.GetDbEnv();

  m_pdbIQFSymbols = new Db( pDbEnv, 0 );
  m_pdbIQFSymbols->open( NULL, "TradeFrame.bdb", "IQFSymbols", DB_BTREE, DB_CREATE, 0 );

  m_pdbIxIQFSymbols_Market = new Db( pDbEnv, 0 );
  m_pdbIxIQFSymbols_Market->set_flags( DB_DUPSORT );
  m_pdbIxIQFSymbols_Market->open( NULL, "TradeFrame.bdb", "IxIQFSymbols_Market", DB_BTREE, DB_CREATE, 0 );

  m_pdbIQFSymbols->associate( NULL, m_pdbIxIQFSymbols_Market, &CIQFeedSymbolFile::GetMarketName, 0 );
}

void CIQFeedSymbolFile::Close() {
  m_pdbIxIQFSymbols_Market->close(0);
  m_pdbIQFSymbols->close(0);
}

void CIQFeedSymbolFile::SetSearchExchange( const char *szExchange ) {
  m_szSearchKey = szExchange;
  m_lenSearchKey = strlen( szExchange );
  m_dbtKey.set_data( (void*) szExchange );
  m_dbtKey.set_size( m_lenSearchKey );
  m_pdbIxIQFSymbols_Market->cursor( NULL, &m_pdbcIxIQFSymbols_Market, 0 );
}

const char *CIQFeedSymbolFile::GetSymbol( u_int32_t flags ) {
  const char *szSymbol = NULL;
  int result = m_pdbcIxIQFSymbols_Market->get( &m_dbtKey, &m_dbtData, flags );
  if ( 0 == result ) {
    structIQFSymbolRecord *pRecord = (structIQFSymbolRecord *) m_dbtData.get_data();
    szSymbol = pRecord->line;
  }
  return szSymbol;
}

void CIQFeedSymbolFile::EndSearch( void ) {
}

// http://www.relisoft.com/book/tech/6lib.html
class LessThan
    : public std::binary_function<char const *, char const *, bool>
{
public:
    bool operator () (char const * str1, 
                      char const * str2) const
    {
        return strcmp (str1, str2) < 0;
    }
};

bool CIQFeedSymbolFile::Load( const string &filename ) {
  // mktsymbols.txt

  // TODO:  need to prevent re-entrant execution:  ie, turn invoking button off
  // TODO:  need to put in back ground thread, and lock out other access processes while running

  static const boost::regex rxMutual(".{3}[^\\.]X$");
  static const boost::regex rxMoneyMkt(".{3}XX$");
  static const boost::regex rxIndex(".+\\.X$");
  static const boost::regex rxCboe(".+\\.XO$");
  static const boost::regex rxIndicator(".+\\.Z$");

  unsigned int cntMutual = 0, cntMoneyMkt = 0, cntIndex = 0, cntCboe = 0, cntIndicator = 0;
  map<const char *, unsigned int, LessThan> map_SymbolsPerExchange;
  typedef pair<const char *, unsigned int> pair_SymbolsPerExchange;
  map<const char *, unsigned int, LessThan>::iterator map_iter_SymbolsPerExchange;

  ifstream file;
  unsigned long cntLines = 0;
  td_structIndexes j, k, c; 
  bool bEndFound;

  file.open( filename.c_str() );

  Open();
  u_int32_t countp = 0;
  m_pdbIQFSymbols->truncate( NULL, &countp, 0 );

  try {
    file.getline( dbRecord.line, nMaxBufferSize );
    while ( !file.fail() ) {
      ++cntLines;
      bEndFound = false;
      c = j = k = 0; // c is count of char in string, j index into ix, k index into line
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
      Dbt value( &dbRecord, dbRecord.bufferedlength );
      int ret = m_pdbIQFSymbols->put( 0, &key, &value, DB_NOOVERWRITE );
      // get next line
      file.getline( dbRecord.line, nMaxBufferSize );
    }
  }
  catch( DbException &dbex ) {
    cout << "IQFSymbols exception: " << dbex.what() << endl;
  }

  Close();
  file.close();

  cout << "Count Mutual:  " << cntMutual << endl;
  cout << "Count MoneyMkt:  " << cntMoneyMkt << endl;
  cout << "Count Index:  " << cntIndex << endl;
  cout << "Count CBOE:  " << cntCboe << endl;
  cout << "Count Indicator:  " << cntIndicator << endl;

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
  dbRecord.ucBits = 0;
  dbRecord.ucBits |= ( m_bMutual ? ucMutual : 0 );
  dbRecord.ucBits |= ( m_bMoneyMkt ? ucMoneyMkt : 0 );
  dbRecord.ucBits |= ( m_bIndex ? ucIndex : 0 );
  dbRecord.ucBits |= ( m_bCboe ? ucCboe : 0 );
  dbRecord.ucBits |= ( m_bIndicator ? ucIndicator : 0 );
  dbRecord.ucBits |= ( m_bHasOptions ? ucHasOptions : 0 );
}

void CIQFeedSymbolFile::UnPackBoolean( void ) {
  m_bMutual = 0 != ( dbRecord.ucBits & ucMutual );
  m_bMoneyMkt = 0 != ( dbRecord.ucBits & ucMoneyMkt );
  m_bIndex = 0 != ( dbRecord.ucBits & ucIndex );
  m_bCboe = 0 != ( dbRecord.ucBits & ucCboe );
  m_bIndicator = 0 != ( dbRecord.ucBits & ucIndicator );
  m_bHasOptions = 0 != ( dbRecord.ucBits & ucHasOptions );
}

int CIQFeedSymbolFile::GetMarketName( Db *secondary, const Dbt *pKey, const Dbt *data, Dbt *secKey ) {
  structIQFSymbolRecord *dbIxRecord = (structIQFSymbolRecord *) data->get_data();
  char *p = dbIxRecord->line + dbIxRecord->ix[2];
  unsigned long l = dbIxRecord->cnt[2];
  secKey->set_data( p );
  secKey->set_size( l );
  return 0;
}
