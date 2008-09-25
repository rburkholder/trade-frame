#include "StdAfx.h"
#include "GuiThreadCrossing.h"

#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGuiThreadCrossing, CWnd)

CGuiThreadCrossing::CGuiThreadCrossing(void): m_pCreationThread( ::AfxGetThread() ) {
  assert( NULL != m_pCreationThread );  // obviously, this needs creating in the GUI thread
  BOOL b = CWnd::Create( NULL, "GuiThreadCrossing", WS_CHILD, CRect( 0, 0, 10 , 10), ::AfxGetMainWnd(), 1 );
  assert( b );
}

CGuiThreadCrossing::~CGuiThreadCrossing() {
  BOOL b = CWnd::DestroyWindow();  // may need to send event instead
  assert( b );
}

BEGIN_MESSAGE_MAP(CGuiThreadCrossing, CWnd)
	ON_WM_DESTROY()
  ON_MESSAGE( WM_GUITHREADCROSSING, OnCrossThreadArrival )
END_MESSAGE_MAP()

void CGuiThreadCrossing::OnDestroy()  {
  CWnd::OnDestroy();
}

LRESULT CGuiThreadCrossing::OnCrossThreadArrival( WPARAM w, LPARAM l ) {
  return 1;
}

// SendMessage( WM_GUITHREADCROSSING, (WPARAM) state );  // calls function
// PostMessage( WM_GUITHREADCROSSING, (WPARAM) state );  // puts into window queue  (don't use when pointers involved)