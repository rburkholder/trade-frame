#include "StdAfx.h"
#include "ConsoleCoutMessages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CConsoleCoutMessages::CConsoleCoutMessages(CWnd* pParent /*=NULL*/)
: CConsoleMessages( pParent ), CCrossThreadCout() {

  m_ConsoleStream.SetOnNewString( MakeDelegate( this, &CConsoleCoutMessages::HandleLine ) );
  m_ConsoleStream.SetOnFlushString( MakeDelegate( this, &CConsoleCoutMessages::HandleEndOfLine ) );

  m_pOldStreambuf = cout.rdbuf();
  cout.rdbuf( &m_ConsoleStream ); 
}

CConsoleCoutMessages::~CConsoleCoutMessages(void) {
  cout.rdbuf( m_pOldStreambuf );
}

void CConsoleCoutMessages::HandleLine(const char *s, int n ) {
  // TODO:  scan string for 0x0a and turn into separate lines
  if ( AfxGetThread() == m_pThreadMain ) {
    std::string _s( s, n );
    Write( _s );
  }
  else {
    std::string *ps = new std::string( s, n );
    CCrossThreadCout::SendLineXThread( ps );
  }
}

void CConsoleCoutMessages::HandleEndOfLine() {
  //WriteLine( "" );
  if ( AfxGetThread() == m_pThreadMain ) {
    WriteLine();
  }
  else {
    CCrossThreadCout::SendNewLineXThread();
  }
}

void CConsoleCoutMessages::HandleXThreadLine( const std::string &s ) {
  Write( s );
}

void CConsoleCoutMessages::HandleXThreadNewLine() {
  WriteLine();
}

//std::ios_base::sync_with_stdio(true); 