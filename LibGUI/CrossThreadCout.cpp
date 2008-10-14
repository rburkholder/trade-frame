#include "StdAfx.h"
#include "CrossThreadCout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCrossThreadCout, CGuiThreadCrossing)
#define WM_LINE (WM_GUITHREADCROSSING + 1)
#define WM_NEWLINE (WM_GUITHREADCROSSING + 2)

CCrossThreadCout::CCrossThreadCout(void): CGuiThreadCrossing() {
}

CCrossThreadCout::~CCrossThreadCout(void) {
}

void CCrossThreadCout::SendLineXThread(std::string *pLine) {
  SendMessage( WM_LINE, reinterpret_cast<WPARAM>( pLine ) );
}

void CCrossThreadCout::SendNewLineXThread() {
  SendMessage( WM_NEWLINE );
}

BEGIN_MESSAGE_MAP(CCrossThreadCout, CGuiThreadCrossing)
  ON_MESSAGE( WM_LINE, &CCrossThreadCout::OnLine )
  ON_MESSAGE( WM_NEWLINE, &CCrossThreadCout::OnNewLine )
END_MESSAGE_MAP()

LRESULT CCrossThreadCout::OnLine( WPARAM w, LPARAM l ) {
  // use boost::shared_ptr for this

  std::string *pLine = reinterpret_cast<std::string *>( w );
  HandleXThreadLine( *pLine );
  delete pLine; 
  return 1;
}

LRESULT CCrossThreadCout::OnNewLine( WPARAM w, LPARAM l ) {
  HandleXThreadNewLine();
  return 1;
}


