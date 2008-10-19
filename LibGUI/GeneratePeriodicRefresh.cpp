#include "StdAfx.h"
//#include "GTWindowsConstants.h"
#include "GeneratePeriodicRefresh.h"

#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGeneratePeriodicRefresh, CGuiThreadCrossing)

#define WM_PERIODICREFRESH (WM_GUITHREADCROSSING + 1)

//int CGeneratePeriodicRefresh::m_cntInstances = 0;

CGeneratePeriodicRefresh::CGeneratePeriodicRefresh( void ): 
CGuiThreadCrossing(), m_bKeepTimerActive( true )
{
  //if ( 0 == m_cntInstances ) {
  //}
  //++m_cntInstances;

  m_pThreadPeriodic = AfxBeginThread( 
    &CGeneratePeriodicRefresh::TriggerRefresh, this, 0, 0, CREATE_SUSPENDED );
  if ( NULL == m_pThreadPeriodic ) {
    //DWORD err = GetLastError();
    throw "CGeneratePeriodicRefresh Thread creation problem";
  }
  else {
    // don't start this until we've got data to draw.
    m_hWaitEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    m_pThreadPeriodic->ResumeThread();
  }
}

CGeneratePeriodicRefresh::~CGeneratePeriodicRefresh(void) {
  m_bKeepTimerActive = false;
  PulseEvent( m_hWaitEvent ); // trigger end of thread
  DWORD dw = WaitForSingleObject( m_pThreadPeriodic->m_hThread, 750 );
  if ( WAIT_TIMEOUT == dw ) {
    TerminateThread( m_pThreadPeriodic->m_hThread, 0 ); // not supposed to be called this way
  }
  CloseHandle( m_hWaitEvent );
  //delete m_pThreadPeriodic;
  assert( 0 == m_OnRefresh.Size() );  // test for code cleanup elsewhere
  //assert( 0 != m_cntInstances );
  //--m_cntInstances;
  //if ( 0 == m_cntInstances ) {
  //}
}

void CGeneratePeriodicRefresh::Add( delegateRefresh_t::OnMessageHandler handler ) {
  //if ( 0 == m_OnRefresh.Size() ) {
  //}
  m_OnRefresh.Add( handler ); 
}

void CGeneratePeriodicRefresh::Remove( delegateRefresh_t::OnMessageHandler handler ) {
  m_OnRefresh.Remove( handler );
  //if ( 0 == m_OnRefresh.Size() ) {
  //}
}

UINT CGeneratePeriodicRefresh::TriggerRefresh( LPVOID lpParameter ) {
  CGeneratePeriodicRefresh *p = reinterpret_cast<CGeneratePeriodicRefresh *>( lpParameter );
  while ( p->m_bKeepTimerActive ) {
    p->SendMessage( WM_PERIODICREFRESH );
    WaitForSingleObject( p->m_hWaitEvent, 250 );
  }
  return 0;
}

BEGIN_MESSAGE_MAP(CGeneratePeriodicRefresh, CGuiThreadCrossing)
//	ON_WM_DESTROY()
  ON_MESSAGE( WM_PERIODICREFRESH, &CGeneratePeriodicRefresh::OnPeriodicRefresh )
END_MESSAGE_MAP()

//void CGeneratePeriodicRefresh::OnDestroy()  {
//  CGuiThreadCrossing::OnDestroy();
//}

LRESULT CGeneratePeriodicRefresh::OnPeriodicRefresh( WPARAM w, LPARAM l ) {
  if ( !m_OnRefresh.IsEmpty() ) m_OnRefresh( this );
  return 1;
}

