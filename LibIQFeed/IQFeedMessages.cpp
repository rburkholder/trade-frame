#include "StdAfx.h"
//#include "GTScalp.h"
#include "IQFeedMessages.h"

#include <iostream>
#include <sstream>

// News Message: This does appear to be an error in the documentation. 
// "SNT:AAPL::1:20070901:;" 
// Enter the date with a range, such as "20070901-20070910" to get the data you are looking for.

//**** CIQFBaseMessage

CIQFBaseMessage::CIQFBaseMessage() {
  m_pStr = NULL;
  //m_nStr = 0;
  //m_nStrMax = 0;
}

CIQFBaseMessage::~CIQFBaseMessage(void) {
  //if ( NULL != m_pStr ) delete [] m_pStr;
  //m_nStr = 0;
  //m_nStrMax = 0;
}

//void CIQFBaseMessage::Assign( unsigned short nStr, const char *str ) {
//  if ( nStr > m_nStrMax ) {
//    if ( NULL != m_pStr ) delete [] m_pStr;
//    m_pStr = new char[ nStr ];
//    m_nStrMax = nStr;
//  }
//  m_nStr = nStr;
//  strncpy( m_pStr, str, nStr );
//  Tokenize();
//}

void CIQFBaseMessage::Assign( const char *str ) {
  m_pStr = str;
  Tokenize();
}

void CIQFBaseMessage::Tokenize( void ) {
  // used in IQFeedLookupPort::Parse
  unsigned short ixFields = 1;  // use as defined in IQFeed docs
  unsigned short ixStrBegin = 0;
  unsigned short cnt = 0;
//  size_t len = m_sMsg.length();

  unsigned short ixString = 0;
  while ( 0 != m_pStr[ ixString ] ) {
    if ( ',' == m_pStr[ ixString ] ) { 
      rOffset[ ixFields ] = ixStrBegin;
      rSize[ ixFields ] = cnt;
      ixStrBegin = ixString + 1;
      cnt = 0;
      ixFields++;
    }
    else {
      cnt++;
    }
    ixString++;
  }
  rOffset[ ixFields ] = ixStrBegin;
  rSize[ ixFields ] = cnt;

  cntFieldsFound = ixFields;
}

void CIQFBaseMessage::EmitFields( void ) {

  CString s;
  for ( int i = 1; i <= cntFieldsFound; i++ ) {
    if ( 0 != rSize[ i ] ) {
      s.Format( "%s%d %s", Field( 1 ), i, Field( i ) );
      //s.Format( "%s%d %s",   m_sMsg.substr( rOffset[ 1 ], rSize[ 1 ] ), i, m_sMsg.substr( rOffset[ i ], rSize[ i ] ) );
      cout << s << endl;
    }
  }
}

void CIQFBaseMessage::EmitLine( void ) {
  cout << m_pStr << endl;
}

const string &CIQFBaseMessage::Field( unsigned short fld ) {
  //if ( 0 == fld ) return 0.0;
  ASSERT( 0 != fld );
  //if ( fld > cntFieldsFound ) return 0.0;
  ASSERT( fld <= cntFieldsFound );
  if ( 0 == rSize[ fld ] ) return sNull;
  //sField = m_sMsg.substr( rOffset[ fld ], rSize[ fld ] );
  sField.assign( m_pStr, rOffset[ fld ], rSize[ fld ] );
  return sField;
}

double CIQFBaseMessage::Double( unsigned short fld ) {
  //if ( 0 == fld ) return 0.0;
  ASSERT( 0 != fld );
  //if ( fld > cntFieldsFound ) return 0.0;
  ASSERT( fld <= cntFieldsFound );
  if ( 0 == rSize[ fld ] ) return 0.0;
  return atof( m_pStr + rOffset[ fld ] );
  //return atof( m_sMsg.substr( rOffset[ fld ], rSize[ fld ] ).c_str() );
}

int CIQFBaseMessage::Integer( unsigned short fld ) {
  //if ( 0 == fld ) return 0.0;
  ASSERT( 0 != fld );
  //if ( fld > cntFieldsFound ) return 0.0;
  ASSERT( fld <= cntFieldsFound );
  if ( 0 == rSize[ fld ] ) return 0;
  return atoi( m_pStr + rOffset[ fld ] );
  //return atoi( m_sMsg.substr( rOffset[ fld ], rSize[ fld ] ).c_str() );
}


//**** CIQFSystemMessage

CIQFSystemMessage::CIQFSystemMessage() {
}

CIQFSystemMessage::~CIQFSystemMessage() {
}

//**** CIQFTimeMessage

CIQFTimeMessage::CIQFTimeMessage() {
  m_bMarketIsOpen = false;
  m_timeMarketOpen = time_duration( 9, 30, 0 );
  m_timeMarketClose = time_duration( 16, 0, 0 );
}

CIQFTimeMessage::~CIQFTimeMessage() {
}

void CIQFTimeMessage::Assign( const char *str ) {
  CIQFBaseMessage::Assign( str );
  stringstream ss( Field( 2 ) );
  time_input_facet *input_facet;
  input_facet = new time_input_facet();  // input facet stuff needs to be with ss.imbue, can't be reused
  input_facet->format( "%Y%m%d %H:%M:%S" );
  ss.imbue( locale( ss.getloc(), input_facet ) );
  ss >> m_dt;
  m_bMarketIsOpen = ( ( m_dt.time_of_day() >= m_timeMarketOpen ) && ( m_dt.time_of_day() < m_timeMarketClose ) );
}

//**** CIQFNewsMessage

CIQFNewsMessage::CIQFNewsMessage() {
}

CIQFNewsMessage::~CIQFNewsMessage() {
  //m_sDistributor.clear();
  //m_sStoryId.clear();
  //m_sSymbolList.clear();
  //m_sDateTime.clear();
  //m_sHeadline.clear();
}

void CIQFNewsMessage::Assign( const char *str ) {
  CIQFBaseMessage::Assign( str );

  m_sDistributor = Field( NDistributor );
  m_sStoryId = Field( NStoryId );
  m_sSymbolList = Field( NSymbolList );
  m_sDateTime = Field( NDateTime );
  //m_sHeadline = m_sMsg.substr( rOffset[ NHeadline ] );
  m_sHeadline.assign( m_pStr + rOffset[ NHeadline ] );
}

//**** CIQFFundamentalMessage

CIQFFundamentalMessage::CIQFFundamentalMessage() {
}

CIQFFundamentalMessage::~CIQFFundamentalMessage() {
}

//**** CIQFPricingMessage

CIQFPricingMessage::CIQFPricingMessage() {
}

CIQFPricingMessage::~CIQFPricingMessage() {
}

ptime CIQFPricingMessage::LastTradeTime( void ) {
  //if ( 0 == fld ) return 0.0;
  //  ASSERT( 0 != fld );
  //if ( fld > cntFieldsFound ) return 0.0;
  //  ASSERT( fld <= cntFieldsFound );
  //  if ( 0 == rSize[ fld ] ) return 0;
  if ( ( rSize[ QPLastTradeTime ] >= 8 ) && ( rSize[ QPLastTradeDate ] == 10 ) ) {
    char szDateTime[ 20 ];
    strncpy( szDateTime +  0, m_pStr + rOffset[ QPLastTradeDate ] + 6, 4 ); // yyyy
    strncpy( szDateTime +  5, m_pStr + rOffset[ QPLastTradeDate ] + 0, 2 ); // mm
    strncpy( szDateTime +  8, m_pStr + rOffset[ QPLastTradeDate ] + 3, 2 ); // dd
    strncpy( szDateTime + 11, m_pStr + rOffset[ QPLastTradeTime ]    , 8 ); // hh:mm:ss
    szDateTime[ 4 ] = szDateTime[ 7 ] = '-';
    szDateTime[ 10 ] = ' ';
    szDateTime[ 19 ] = 0;

    //  string dt;
    //  dt.assign( szDateTime );
    return boost::posix_time::time_from_string(szDateTime);
  }
  else {
    return boost::posix_time::ptime(boost::date_time::special_values::min_date_time );
  }
  //return atoi( m_sMsg.substr( rOffset[ fld ], rSize[ fld ] ).c_str() );

  //string sBaseDate = m_sMsg.substr( rOffset[ QPLastTradeDate ], rSize[ QPLastTradeDate ] );
  //string sBaseTime = m_sMsg.substr( rOffset[ QPLastTradeTime ], rSize[ QPLastTradeTime ] ).substr( 0, 8 );
  //string dt = sBaseDate.substr( 6, 4 ) + "-" + sBaseDate.substr( 0, 2 ) + "-" + sBaseDate.substr( 3, 2 ) + " " + sBaseTime;

}

//**** CIQFUpdateMessage

CIQFUpdateMessage::CIQFUpdateMessage() {
}

CIQFUpdateMessage::~CIQFUpdateMessage() {
}

/*
void CIQFUpdateMessage::Assign( const char *str ) {
  CIQFBaseMessage::Assign( str );

  bFound = true;
  if ( 0 == rSize[ CIQFPricingMessage::QPLastTradeTime ] ) {
    // probably 'Not Found' message
    if ( _T( "Not Found" ) == Field( CIQFPricingMessage::QPLast ) ) {
      bFound = false;
    }
    else {
      // LastTradeTime really shouldn't be empty otherwise
    }
  }
}
*/

//**** CIQFSummaryMessage

CIQFSummaryMessage::CIQFSummaryMessage() {
}

CIQFSummaryMessage::~CIQFSummaryMessage() {
}

