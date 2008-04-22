#include "StdAfx.h"
//#include "GTScalp.h"
#include "IQFeed.h"
#include "IQ32.H"
#include "GTWindowsConstants.h"  // based upon WM_USER

#include <sstream>
#include <stdexcept>

// http://www.dtn.com/trading.cfm?sidenav=sn_trading&content=pr_nxcore

void __stdcall IQFeedCallBack( int x, int y ) {
  stringstream ss;
  ss << "IQFeed Callback" << x << ", " << y;
  //theApp.pConsoleMessages->WriteLine( ss.str() );
}

IMPLEMENT_DYNAMIC( CIQFeed, CWnd )

CIQFeed::CIQFeed( CWnd *pParent ) {

  BOOL b = CWnd::Create( NULL, "IQFeed", WS_CHILD, CRect( 0, 0, 20, 20 ), pParent, 1 );

  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( this->m_hWnd, _T("ONE_UNIFIED"), _T("0.11111111"), _T("2.0") );
  
  IQConnect.SetOnPreThreadCrossingResponse( MakeDelegate( this, &CIQFeed::OnPreCrossThreadResponse ) );
  IQConnect.SetOnPostThreadCrossingResponse( MakeDelegate( this, &CIQFeed::OnNewResponsePort5009 ) );
  IQConnect.Activate();
  IQConnect.OpenSocket( "127.0.0.1", 5009 );
}

CIQFeed::~CIQFeed(void) {

  // have to turn off stuff as well.
  while ( !m_qLookupPortAvailable.empty() ) {
    CIQFeedThreadCrossing *state = m_qLookupPortAvailable.front();
    m_qLookupPortAvailable.pop();
    state->CloseSocket();
    delete state;
  }
  IQConnect.CloseSocket();
  IQConnect.Deactivate();

  // some messages may be in limbo if they havn't been returned to here before 
  // program terminates

  LP_CIQFSymbol sym;
  map<string,LP_CIQFSymbol>::iterator iter;
  iter = m_mapSymbols.begin();
  while ( m_mapSymbols.end() != iter ) {
    sym = iter->second;
    delete sym;
    sym = NULL;
    iter++;
  }
  RemoveClientApp( NULL );
}

BEGIN_MESSAGE_MAP(CIQFeed, CWnd)
  ON_MESSAGE( WM_IQFEEDCROSSTHREAD, OnCrossThreadArrival )
END_MESSAGE_MAP()

CIQFSymbol *CIQFeed::Attach( const string &symbol ) {
  m_mapSymbols_Iter = m_mapSymbols.find( symbol );
  CIQFSymbol *pSym;
  if ( m_mapSymbols.end() == m_mapSymbols_Iter ) {
    pSym = new CIQFSymbol( symbol.c_str() );
    m_mapSymbols.insert( m_mapSymbols_Pair( symbol.c_str(), pSym ) );
  }
  else {
    pSym = m_mapSymbols_Iter -> second;
  }
  return pSym;
}

CIQFSymbol *CIQFeed::Watch( const string &symbol ) {
  CIQFSymbol *pSym = Attach( symbol );
  if ( pSym->Watch() ) {
    CString s;
    s.Format( "w%s\n", symbol.c_str() );
    IQConnect.SendToSocket( (char*) LPCTSTR( s ) );
  }
  return pSym;
}

void CIQFeed::UnWatch( const string &symbol ) {
  m_mapSymbols_Iter = m_mapSymbols.find( symbol );
  CIQFSymbol *pSym;
  if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
    pSym = m_mapSymbols_Iter -> second;
    if ( pSym->UnWatch() ) {
      CString s;
      s.Format( "r%s\n", symbol );
      IQConnect.SendToSocket( (char*) LPCTSTR( s ) );
    }
  }
}

CIQFeedThreadCrossing *CIQFeed::CheckOutLookupPort( void ) {
  CIQFeedThreadCrossing *state;
  if ( m_qLookupPortAvailable.empty() ) {
    state = new CIQFeedThreadCrossing();
    state->SetOnPreThreadCrossingResponse( MakeDelegate( this, &CIQFeed::OnPreCrossThreadResponse ) );
    state->OpenSocket( "127.0.0.1", 9100 );
  }
  else {
    state = m_qLookupPortAvailable.front();
    m_qLookupPortAvailable.pop();
  }
  state->Activate();
  return state;
}

void CIQFeed::CheckInLookupPort( CIQFeedThreadCrossing *state ) {
  // could just as easily set it to null
  state->Deactivate();
  state->SetOnPostThreadCrossingResponse( MakeDelegate( this, &CIQFeed::PortNullProcessing ) );
  m_qLookupPortAvailable.push( state );
}

void CIQFeed::OnPreCrossThreadResponse(unsigned short nStr, const char *str, LPVOID object ) {
  CIQFeedThreadCrossing *state = (CIQFeedThreadCrossing *) object;
  state->QueueResponse( nStr, str );
  SendMessage( WM_IQFEEDCROSSTHREAD, (WPARAM) state );
}

LRESULT CIQFeed::OnCrossThreadArrival( WPARAM w, LPARAM l ) {
  CIQFeedThreadCrossing *state = (CIQFeedThreadCrossing *) w;
  state->ProcessResponse();
  return 1;
}

void CIQFeed::PortNullProcessing( const char *str ) {
  throw runtime_error( "found data in CIQFeed::PortNullProcessing" );
}

void CIQFeed::OnNewResponsePort5009( const char *str ) {
  switch ( str[0] ) {
    case 'Q': 
      {
        CIQFUpdateMessage msg;
        msg.Assign( str );
        m_mapSymbols_Iter = m_mapSymbols.find( msg.Field( CIQFUpdateMessage::QPSymbol ) );
        CIQFSymbol *pSym;
        if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
          pSym = m_mapSymbols_Iter -> second;
          pSym ->HandleUpdateMessage( &msg );
        }
      }
      break;
    case 'P': 
      {
        CIQFSummaryMessage msg;
        msg.Assign( str );
        m_mapSymbols_Iter = m_mapSymbols.find( msg.Field( CIQFSummaryMessage::QPSymbol ) );
        CIQFSymbol *pSym;
        if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
          pSym = m_mapSymbols_Iter -> second;
          pSym ->HandleSummaryMessage( &msg );
        }
      }
      break;
    case 'F': 
      {
        CIQFFundamentalMessage msg;
        msg.Assign( str );
        m_mapSymbols_Iter = m_mapSymbols.find( msg.Field( CIQFFundamentalMessage::FSymbol ) );
        CIQFSymbol *pSym;
        if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
          pSym = m_mapSymbols_Iter -> second;
          pSym ->HandleFundamentalMessage( &msg );
        }

      }
      break;
    case 'N': 
      {
        CIQFNewsMessage msg;
        msg.Assign( str );
        NewsMessage( &msg );

        const char *ixFstColon = msg.m_sSymbolList.c_str();
        const char *ixLstColon = msg.m_sSymbolList.c_str();
        string s;
        __w64 int cnt;

        if ( 0 != *ixLstColon ) {
          do {
            // each symbol has a surrounding set of colons
            if ( ':' == *ixLstColon ) {
              if ( ( ixLstColon - ixFstColon ) > 1 ) {
                // extract symbol
                cnt = ixLstColon - ixFstColon - 1;
                s.assign( ++ixFstColon, cnt );

                m_mapSymbols_Iter = m_mapSymbols.find( s.c_str() );
                CIQFSymbol *pSym;
                if ( m_mapSymbols.end() != m_mapSymbols_Iter ) {
                  pSym = m_mapSymbols_Iter -> second;
                  pSym ->HandleNewsMessage( &msg );
                }
                ixFstColon = ixLstColon;
              }
              else {
                if ( 1 == ( ixLstColon - ixFstColon ) ) {
                  // no symbol, move FstColon
                  ixFstColon = ixLstColon;
                }
              }
            }
            ixLstColon++;
          } while ( 0 != *ixLstColon );
        }
      }
      break;
    case 'T': 
      {
        CIQFTimeMessage msg;
        msg.Assign( str );
        TimeMessage( &msg );
      }
      break;
    case 'S': 
      {
        CIQFSystemMessage msg;
        msg.Assign( str );
        msg.EmitLine();
        if ( "KEY" == msg.Field( 2 ) ) {
          CString s;
          s.Format( "S,KEY,%s\n", msg.Field( 3 ) );
          IQConnect.SendToSocket( (char*) LPCTSTR( s ) );
          IQConnect.SendToSocket( "S,NEWSON\n" );
        }
        if ( "CUST" == msg.Field( 2 ) ) {
          if ( "4.3.0.3" > msg.Field( 7 ) ) {
            cout << "Need IQFeed version of 4.3.0.3 or greater (" << msg.Field( 7 ) << ")" << endl;
            //throw s;  // can't throw exception, just accept it, as we are getting '2.5.3' as a return
          }
        }
      }
      break;
    default:
      throw "Unknown message type in IQFeed"; // unknown message type
      break;
  }
}

void CIQFeed::Send( char *szCommand ) {
  IQConnect.SendToSocket( szCommand ); 
  IQConnect.SendToSocket( "\n" );
}
 
