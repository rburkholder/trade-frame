#include "StdAfx.h"
#include "CrossThreadMerge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_CARRIEREVENT ( WM_GUITHREADCROSSING + 1)

IMPLEMENT_DYNAMIC(CCrossThreadMerge, CGuiThreadCrossing)

CCrossThreadMerge::CCrossThreadMerge(void) 
: CMergeDatedDatums(), CGuiThreadCrossing()
{
  //m_pMainThread = AfxGetThread(); // comparison for crossing
  m_hSignal = CreateEvent( NULL, FALSE, FALSE, "" );
  assert( NULL != m_hSignal );
}

CCrossThreadMerge::~CCrossThreadMerge(void) {
  CloseHandle( m_hSignal );
}

BEGIN_MESSAGE_MAP(CCrossThreadMerge, CGuiThreadCrossing)
  ON_MESSAGE( WM_CARRIEREVENT, OnCrossThreadArrival )
END_MESSAGE_MAP()

void CCrossThreadMerge::ProcessCarrier( CMergeCarrierBase *pCarrier ) {
  // happens in background thread
  //BOOL b = PostMessage( WM_CARRIEREVENT, reinterpret_cast<WPARAM>( pCarrier ), reinterpret_cast<LPARAM>( this ) );
  BOOL b = PostMessage( WM_CARRIEREVENT, reinterpret_cast<WPARAM>( pCarrier ) );
  assert( b );
  DWORD dw = WaitForSingleObject( m_hSignal, INFINITE);
  assert( WAIT_OBJECT_0 == dw );
}

LRESULT CCrossThreadMerge::OnCrossThreadArrival( WPARAM w, LPARAM l ) {
  // happens in foreground thread
  //CCrossThreadMerge *pMerge = reinterpret_cast<CCrossThreadMerge *>( l );
  CMergeCarrierBase *pCarrier = reinterpret_cast<CMergeCarrierBase *>( w );
  pCarrier->ProcessDatum();  // process the datum in the gui thread
  BOOL b = SetEvent( m_hSignal ); // set after so carrier is available through full sequence
  assert( b );
  return 1;
}