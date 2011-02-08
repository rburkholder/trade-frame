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

#pragma once

// based upon getting information through IQFeed services.

#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include "OUBerkeleyDB/CommonDatabaseFunctions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CInstrumentInformation : public CCommonDatabaseFunctions<CInstrumentInformation> {
public:
  CInstrumentInformation( const std::string& sDbFileName );
  virtual ~CInstrumentInformation(void);
  static const char nMaxSymbolNameSize = 20;
  void Save( const std::string &sSymbol, const std::string &sCompanyName,
    float fltPriceEarnings, float fltEarningsPerShare, 
    float fltDividend, ptime dtLastDividend,
    float fltPctInstitutional, float fltSharesOutstanding,
    float fltAssets, float fltLiabilities, float fltLongTermDebt, ptime dtBalanceSheet,
    char nFormatCode, char nPrecision, int nSIC, char nSecurityType, char nListedMarket );
  void Retrieve( const std::string &sSymbol );
  std::string GetCompanyName( ) { std::string s( (*m_pValues).CompanyName, (*m_pValues).nCompanyNameSize ); return s; };
  ptime GetDividendDate( ) { return (*m_pValues).dtLastDividend; };
  ptime GetBalanceSheetDate( ) { return (*m_pValues).dtBalanceSheet; };
  float GetPriceEarnings( ) { return (*m_pValues).fltPriceEarnings; };
  float GetDividend( ) { return (*m_pValues).fltDividend; };
  float GetEarningsPerShare( ) { return (*m_pValues).fltEarningsPerShare; };
  float GetPctInstitutional( ) { return (*m_pValues).fltPctInstitutional; };
  float GetAssets( ) { return (*m_pValues).fltAssets; };
  float GetLiabilities( ) { return (*m_pValues).fltLiabilities; };
  float GetLongTermDebt( ) { return (*m_pValues).fltLongTermDebt; };
  float GetSharesOutstanding( ) { return (*m_pValues).fltSharesOutstanding; };
  int GetSIC( ) { return (*m_pValues).nSIC; };
  char GetFormatCode( ) { return (*m_pValues).nFormatCode; };
  char GetPrecision( ) { return (*m_pValues).nPrecision; };
  char GetSecurityType( ) { return (*m_pValues).nSecurityType; };
  char GetListedMarket( ) { return (*m_pValues).nListedMarket; };
protected:
  struct structKey {
    char nKeySize; // excludes terminating 0
    char chKey[ nMaxSymbolNameSize ];
    structKey( void ) : nKeySize( 0 ) {};
    structKey( size_t size, const char *pkey ) : nKeySize( 0 ) {
      assert( size <= nMaxSymbolNameSize );
      assert( 0 < size );
      nKeySize = (char) size;
      memcpy( chKey, pkey, size );
    }
  };
  static const unsigned char nMaxCompanyNameSize = 200;
  struct structValues {
    ptime dtLastDividend;
    ptime dtBalanceSheet;
    float fltPriceEarnings;
    float fltDividend;  // for latest quarter
    float fltEarningsPerShare;
    float fltPctInstitutional;
    float fltAssets;
    float fltLiabilities;
    float fltLongTermDebt;
    float fltSharesOutstanding;
    int nSIC;
    char nSecurityType;
    char nFormatCode; // file:///H:/Program%20Files/DTN/IQFeed/Docs/PriceFormatCodes.html
    char nPrecision;
    char nListedMarket;
    unsigned char nCompanyNameSize;
    char CompanyName[ nMaxCompanyNameSize ];
    structValues() : fltPriceEarnings( 0 ), fltDividend( 0 ),
      dtLastDividend( not_a_date_time ), fltEarningsPerShare( 0 ),
      fltPctInstitutional( 0 ), fltAssets( 0 ), fltLiabilities( 0 ),
      dtBalanceSheet( not_a_date_time ), fltLongTermDebt( 0 ),
      fltSharesOutstanding( 0 ), nFormatCode( 0 ), nPrecision( 0 ),
      nSIC( 0 ), nSecurityType( 0 ), nListedMarket( 0 ), nCompanyNameSize( 0 ) {};
    structValues( float pe, float div, ptime dtdiv, float eps,
      float pi, float asset, float liab, ptime bs, float ltd, 
      float so, char fc, char prec, int sic, char st, char lm, 
      const std::string &cn ) : fltPriceEarnings( pe ), fltDividend( div ),
      dtLastDividend( dtdiv ), fltEarningsPerShare( eps ), 
      fltPctInstitutional( pi ), fltAssets( asset ), fltLiabilities( liab ),
      dtBalanceSheet( bs ), fltLongTermDebt( ltd ), 
      fltSharesOutstanding( so ), nFormatCode( fc ), nPrecision( prec ),
      nSIC( sic ), nSecurityType( st ), nListedMarket( lm ) {
        assert( cn.size() <= nMaxCompanyNameSize );
        nCompanyNameSize = (unsigned char) cn.size();
        strncpy( CompanyName, cn.c_str(), cn.size() );
    };
  } *m_pValues;
private:
};

} // namespace tf
} // namespace ou


/*  Format Code
Code 	Formatting
1 	1/8's
2 	1/4's
3 	1/32's
4 	halves of 1/32's
5 	quarters of 1/32's
6 	decimal (display as specified by the precision)
7 	1/64's
8 	halves of 1/64's
9 	halves of 1/32's
10 	1/256's
11 	one decimal place
12 	two decimal places
13 	three decimal places
14 	four decimal places
15 	five decimal places
16 	six decimal places
17 	seven decimal places
18 	eight decimal places
19 	nine decimal places
*/

/*
Security Type
Security Type 	Security Type Name
1 	Stock
2 	Index/Equity Option
3 	Mutual Fund
4 	Money Market Fund
5 	Bond
6 	Index
7 	Market Statistic
8 	Future
9 	Future Option
10 	Future Spread
11 	Spot (Cash) Price
12 	Forward
13 	Calculated Statistic
14 	Calculated Strip
15 	Single Stock Future
16 	Foreign Monetary Exchange
*/

/*
Listed Market
Market ID 	Market Name
1 	NASDAQ National market
2 	NASDAQ Small Cap
3 	NASDAQ other OTC (PINKS)
4 	NASDAQ OTCBB
5 	NASDAQ
6 	American Stock Exchange
7 	New York Stock Exchange
8 	Chicago Stock Exchange
9 	Philadelphia Stock Exchange
10 	Cincinnati Stock Exchange
11 	Pacific Stock Exchange
12 	Boston Stock Exchange
13 	Chicago Board Options Exchange
14 	OPRA System
15 	NASDAQ Alternate Display facility
16 	International Stock Exchange
17 	Boston Options Exchange
20 	Philadelphia Board of Trade
27 	DTN(Calculated/Index/Statistic)
30 	Chicago Board Of Trade
31 	Dow Jones (CBOT)
32 	CBOE Futures Exchange
33 	Kansas City Board Of Trade
34 	Chicago Mercantile Exchange
35 	Minneapolis Grain Exchange
36 	New York Mercantile Exchange
37 	Commodities Exchange Center
38 	New York Board Of Trade
39 	Cantor Financial Futures Exchange (deprecated)
40 	One Chicago
41 	NQLX (deprecated)
42 	Chicago Board Of Trade Mini Sized Contracts
43 	Chicago Mercantile Exchange Mini Sized Contracts
44 	EUREXUS
45 	New York Mercantile Exchange Mini Sized Contracts
50 	Toronto Stock Exchange
51 	Montreal Stock Exchange
52 	Canadian Venture Exchange
53 	Winnipeg Stock Exchange
54 	Winnipeg Commodities Exchange (WPG)
60 	Alliance / CBOT / EUREX
61 	London International Financial Futures Exchange
62 	London Metals Exchange
63 	International Petroleum Exchange
64 	Baltic
65 	Deutsche Terminbourse
66 	Paris:Marche a Terme International de France
67 	Singapore International Monetary Exchange
68 	European Exchange
69 	EURONEXT Index Derivatives
70 	EURONEXT Interest Rates
71 	EURONEXT Commodities
72 	Tullett Liberty
73 	Barclays Bank
74 	Hot Spot
75 	Warenterminborse Hannover
76 	EUREX Indexes
*/

