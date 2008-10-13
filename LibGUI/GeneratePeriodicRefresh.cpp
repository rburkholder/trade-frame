#include "StdAfx.h"
#include "GTWindowsConstants.h"
#include "GeneratePeriodicRefresh.h"

#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGeneratePeriodicRefresh, CGuiThreadCrossing)

#define WM_PERIODICREFRESH (WM_GUITHREADCROSSING + 1)

CGeneratePeriodicRefresh::CGeneratePeriodicRefresh( void ): 
CGuiThreadCrossing(), m_bKeepTimerActive( true )
{
  m_hTimerQueue = CreateTimerQueue();
  ASSERT( NULL != m_hTimerQueue );
  BOOL b = CreateTimerQueueTimer( &m_hTimer, m_hTimerQueue, &CGeneratePeriodicRefresh::TriggerRefresh,
    this, 250, 250, WT_EXECUTEINTIMERTHREAD );
  ASSERT( b );
}

CGeneratePeriodicRefresh::~CGeneratePeriodicRefresh(void) {
  m_bKeepTimerActive = false;
  assert( 0 == m_OnRefresh.Size() );  // test for code cleanup elsewhere
  HANDLE h = CreateEvent( NULL, FALSE, FALSE, NULL );
  ASSERT( NULL != h );
  BOOL b = DeleteTimerQueueTimer( m_hTimerQueue, m_hTimer, h );
  ASSERT( 0 != b );
  WaitForSingleObject( h, 0 );
  //ResetEvent( h );
  b = DeleteTimerQueueEx( m_hTimerQueue, h );
  ASSERT( 0 != b );
  WaitForSingleObject( h, 0 );
  CloseHandle( h );
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

VOID CALLBACK CGeneratePeriodicRefresh::TriggerRefresh( LPVOID lpParameter, BOOLEAN TimerOrWaitFired) {
  CGeneratePeriodicRefresh *p = reinterpret_cast<CGeneratePeriodicRefresh *>( lpParameter );
  if ( p->m_bKeepTimerActive ) {
    p->SendMessage( WM_PERIODICREFRESH );
  }
}

BEGIN_MESSAGE_MAP(CGeneratePeriodicRefresh, CGuiThreadCrossing)
  ON_MESSAGE( WM_PERIODICREFRESH, &CGeneratePeriodicRefresh::OnPeriodicRefresh )
END_MESSAGE_MAP()

void CGeneratePeriodicRefresh::OnDestroy()  {
  CGuiThreadCrossing::OnDestroy();
}

LRESULT CGeneratePeriodicRefresh::OnPeriodicRefresh( WPARAM w, LPARAM l ) {
  if ( !m_OnRefresh.IsEmpty() ) m_OnRefresh( this );
  return 1;
}

