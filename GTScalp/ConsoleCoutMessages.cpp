#include "StdAfx.h"
#include "ConsoleCoutMessages.h"

CConsoleCoutMessages::CConsoleCoutMessages(CWnd* pParent /*=NULL*/)
: CConsoleMessages( pParent ) {

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
  string _s( s, n );
  Write( _s );
}

void CConsoleCoutMessages::HandleEndOfLine() {
  WriteLine( "" );
}

//std::ios_base::sync_with_stdio(true); 