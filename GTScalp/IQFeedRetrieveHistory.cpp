#include "StdAfx.h"
#include "IQFeedRetrieveHistory.h"
//#include "GTScalp.h"

#include <fstream>
#include <stdexcept>
using namespace std;

// IQFeedHistory

IQFeedHistory::IQFeedHistory() {
  m_bBusy = false;
  m_cntRecords = 0;
  m_stateHistory = EDoingNothing;
}

IQFeedHistory::~IQFeedHistory() {
}

void IQFeedHistory::FileRequest( const char *szPrefix, const char *szSymbol, unsigned long nCount ) {

  if ( m_bBusy ) {
    throw new runtime_error( "A Request is already in progress." );
  }
  else {
    m_bBusy = true;
    m_cntRecords = 0;
    m_stateHistory = EDoingNothing;
    m_sSymbol.assign( szSymbol );

    string s;
    s.assign( szPrefix );
    s.append( "-" );
    s.append( szSymbol );
    s.append( ".txt" );

    char buf[ 512 ];
    ifstream file;
    file.open( s.c_str() );

    m_stateHistory = EHandleResponse;
    file.getline( buf, 511 );
    while ( 0 != *buf ) {
      OnPortMessage( buf );
      file.getline( buf, 511 );
    }
    m_stateHistory = EResponseDone;
    OnPortMessage( buf );
    file.close();
  }
}

void IQFeedHistory::LiveRequest( const char *szPrefix, const char *szSymbol, unsigned long nCount ) {

  if ( m_bBusy ) {
    throw new runtime_error( "A Request is already in progress." );
  }
  else {
    m_bBusy = true;
    m_cntRecords = 0;
    m_stateHistory = EDoingNothing;
    m_sSymbol.assign( szSymbol );

    ASSERT( NULL == m_pPort );
    OpenPort();

    string s;
    char buf[ 20 ];
    s.assign( szPrefix );
    s.append( "," );
    s.append( szSymbol );
    s.append( "," );
    s.append( _itoa( nCount, buf, 10 ) );
    s.append( ";" );
    m_stateHistory = ECommandSent;
    m_pPort->SendToSocket( s.c_str() );
  }
}

void IQFeedHistory::OnNewResponse( const char *str ) {
  switch ( m_stateHistory ) {
    case EDoingNothing:
      break;
    case ECommandSent: 
      m_stateHistory = EHandleResponse;  // then fall through and process
    case EHandleResponse:
      if ( 0 == *str ) {
        m_stateHistory = EResponseExpectEndMsg;
      }
      else {
        if ( 0 == strncmp( "!SYNTAX_ERROR!", str, 14 ) ) {
          m_stateHistory = EResponseSyntaxError;
          OnPortMessage( str );
          m_stateHistory = EResponseExpectEmptyLine;
        }
        else {
          if ( 0 == strncmp( "!ERROR!", str, 7 ) ) {
            m_stateHistory = EResponseError;
            OnPortMessage( str );
            m_stateHistory = EResponseExpectEmptyLine;
          }
        }
        ASSERT( 0 != strcmp( "!ENDMSG!", str ) );
        OnPortMessage( str );
      }
      break;
    case EResponseExpectEmptyLine:
      if ( 0 == *str ) {
        m_stateHistory = EResponseExpectEndMsg;
      }
      else {
        throw runtime_error( "Expected empty line IQFeedHistory::OnNewResponse" );
      }
      break;
    case EResponseExpectEndMsg:
      if ( 0 == strcmp( "!ENDMSG!", str ) ) {
        m_stateHistory = EResponseDone;
        ASSERT( NULL != m_pPort );
        if ( NULL != m_pPort ) {
          ClosePort();
        OnPortMessage( str ); // possible deletion is handled here, so don't do anything more
        }
        else {
          throw runtime_error( "Expected !ENDMSG! in IQFeedHistory::OnNewResponse" );
        }
        break;
    case EResponseDone:
      throw runtime_error( "Getting data when do don't expect it in IQFeedHistory::OnNewResponse" );
      break;
      }
  }
}

void IQFeedHistory::Parse(const char *str, string *results, unsigned short nStrings ) {
  // pass in an array of strings to accept from parse statement
  // taken from CIQFBaseMessage::Tokenize
  //string *result = results;
  unsigned short cntChar = 0;  // number of characters found
  unsigned short cntStrings = 0; // number of strings assigned
  const char *ixStrBegin = str;
  const char *ixString = str;
  if ( 0 != *ixString ) {
    do {
      if ( ( ',' == *ixString ) || ( 0 == *ixString ) ) {
        results[ cntStrings ].assign( ixStrBegin, cntChar );
        ++cntStrings;
        if ( cntStrings == nStrings ) break; // stop once we have required number of strings
        ixStrBegin = ixString + 1;
        cntChar = 0;
        //++result;  
      }
      else {
        ++cntChar;
      }
    }
    while ( 0 != *ixString++ );
  }
}

// IQFeedHistoryHD

IQFeedHistoryHD::IQFeedHistoryHD( CBars *pBars ): IQFeedHistory() {
  m_pBars = pBars;
}

IQFeedHistoryHD::~IQFeedHistoryHD() {
}

void IQFeedHistoryHD::FileRequest(const char *szSymbol, unsigned long nCount) {
  IQFeedHistory::FileRequest( "HD", szSymbol, nCount );
}

void IQFeedHistoryHD::LiveRequest(const char *szSymbol, unsigned long nCount) {
  IQFeedHistory::LiveRequest( "HD", szSymbol, nCount );
}

void IQFeedHistoryHD::OnPortMessage( const char *str ) {
  //CBar *pBar;
  switch ( m_stateHistory ) {
    case EHandleResponse: {
      Parse( str, &fields[0], nFields );
      //pBar = new CBar( fields[0].substr(0, 10) + " 00:00:00", fields[3], fields[1], fields[2], fields[4], fields[5] );
      CBar bar( fields[0].substr(0, 10) + " 00:00:00", fields[3], fields[1], fields[2], fields[4], fields[5] );
      m_pBars->AppendDatum( bar );
      m_cntRecords++;
      }
      break;
    case EResponseSyntaxError:
    case EResponseError:
      cout << "HD " << m_sSymbol << " " << str << endl;
      break;
    case EResponseDone:
      m_pBars->Flip();
      m_bBusy = false;
      // check size of Time Series to match count
      cout << "HD " << m_sSymbol << " done " << m_cntRecords << endl;
      if ( NULL != OnRequestComplete ) OnRequestComplete( this ); // don't do anything after this as we are deleted
      break;
  }
}

// IQFeedHistoryHT

IQFeedHistoryHT::IQFeedHistoryHT( CQuotes *pQuotes, CTrades *pTrades ): IQFeedHistory() {
  m_pQuotes = pQuotes;
  m_pTrades = pTrades;
}

IQFeedHistoryHT::~IQFeedHistoryHT() {
}

void IQFeedHistoryHT::FileRequest(const char *szSymbol, unsigned long nCount) {
  IQFeedHistory::FileRequest( "HT", szSymbol, nCount );
}

void IQFeedHistoryHT::LiveRequest(const char *szSymbol, unsigned long nCount) {
  IQFeedHistory::LiveRequest( "HT", szSymbol, nCount );
}

void IQFeedHistoryHT::OnPortMessage(const char *str) {
  //CTrade *pTrade;
  //CQuote *pQuote;
  switch ( m_stateHistory ) {
    case EHandleResponse: {
      Parse( str, &fields[0], nFields );
      //pTrade = new CTrade( fields[0], fields[1], fields[2] );
      CTrade trade( fields[0], fields[1], fields[2] );
      m_pTrades->AppendDatum( trade );
      //pQuote = new CQuote( fields[0], fields[4], fields[7], fields[5], fields[8] );
      CQuote quote( fields[0], fields[4], fields[7], fields[5], fields[8] );
      m_pQuotes->AppendDatum( quote );
      m_cntRecords++;
      }
      break;
    case EResponseSyntaxError:
    case EResponseError:
      cout << "HT " << m_sSymbol << " " << str << endl;
      break;
    case EResponseDone:
      m_bBusy = false;
      // check size of Time Series to match count
      cout << "HT " << m_sSymbol << " done " << m_cntRecords << endl;
      if ( NULL != OnRequestComplete ) OnRequestComplete( this );  // don't do anything after this as we are deleted
      break;
  }
}





//Result Format for Errors:
//!SYNTAX_ERROR!<CR><LF> 
//Or
//
//!ERROR! [error text]<CR><LF>
//<CR><LF>!ENDMSG!<CR><LF> 

// IQFeedHistory

