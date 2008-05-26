#include "StdAfx.h"
#include "IQ32.H"
#include "IQFeed.h"
#include "..\LibCommon\GTWindowsConstants.h"  // based upon WM_USER for cross-thread messaging

#include <sstream>
#include <stdexcept>

// http://www.dtn.com/trading.cfm?sidenav=sn_trading&content=pr_nxcore

void __stdcall IQFeedCallBack( int x, int y ) {
  stringstream ss;
  ss << "IQFeed Callback" << x << ", " << y;
}

IMPLEMENT_DYNAMIC( CIQFeed, CWnd )

CIQFeed::CIQFeed( CWnd *pParent ) {

  BOOL b = CWnd::Create( NULL, "IQFeed", WS_CHILD, CRect( 0, 0, 20, 20 ), ::AfxGetMainWnd(), 1 );

  SetCallbackFunction( &IQFeedCallBack );
  int i = RegisterClientApp( this->m_hWnd, _T("ONE_UNIFIED"), _T("0.11111111"), _T("2.0") );
}

CIQFeed::~CIQFeed(void) {
  RemoveClientApp( NULL );
}

void CIQFeed::Connect() {
  IQConnect.SetOnPreThreadCrossingResponse( MakeDelegate( this, &CIQFeed::OnPreCrossThreadResponse ) );
  IQConnect.SetOnPostThreadCrossingResponse( MakeDelegate( this, &CIQFeed::OnNewResponsePort5009 ) );
  IQConnect.Activate();
  IQConnect.OpenSocket( "127.0.0.1", 5009 );
}

void CIQFeed::Disconnect() {

  // turn this into an async termination so can wait for socket to sent it's termination events 
  //   create the termination events if they don't exist

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

}

void CIQFeed::StartQuoteTradeWatch( CIQFeedSymbol *pSymbol ) {
  if ( !pSymbol->GetQuoteTradeWatchInProgress() ) {
    std::string s = "w" + pSymbol->Name() + "\n";
    //s.Format( "w%s\n", pSymbol->Name().c_str() );
    //IQConnect.SendToSocket( (char*) LPCTSTR( s ) );
    IQConnect.SendToSocket( s.c_str() );
    pSymbol->SetQuoteTradeWatchInProgress();
  }
}

void CIQFeed::StopQuoteTradeWatch( CIQFeedSymbol *pSymbol ) {
  if ( pSymbol->QuoteWatchNeeded() || pSymbol->TradeWatchNeeded() ) {
    // don't do anything, as stuff still active
  }
  else {
    std::string s = "r" + pSymbol->Name() + "\n";
    IQConnect.SendToSocket( s.c_str() );
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

BEGIN_MESSAGE_MAP(CIQFeed, CWnd)
  ON_MESSAGE( WM_IQFEEDCROSSTHREAD, OnCrossThreadArrival )
END_MESSAGE_MAP()

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
        HandleQMessage( &msg );
      }
      break;
    case 'P': 
      {
        CIQFSummaryMessage msg;
        msg.Assign( str );
        HandlePMessage( &msg );
      }
      break;
    case 'F': 
      {
        CIQFFundamentalMessage msg;
        msg.Assign( str );
        HandleFMessage( &msg );
      }
      break;
    case 'N': 
      {
        CIQFNewsMessage msg;
        msg.Assign( str );
        NewsMessage( &msg );
        HandleNMessage( &msg );

      }
      break;
    case 'T': 
      {
        CIQFTimeMessage msg;
        msg.Assign( str );
        TimeMessage( &msg );
        HandleTMessage( &msg );
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
        HandleSMessage( &msg );
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
 
