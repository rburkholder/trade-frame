#include "StdAfx.h"
#include "GTWindowsConstants.h"
#include "GeneratePeriodicRefresh.h"

#include <stdexcept>

unsigned int CGeneratePeriodicRefresh::m_nInstances = 0;
HANDLE CGeneratePeriodicRefresh::hScreenRefreshThread = 0;
DWORD CGeneratePeriodicRefresh::RefreshThreadId = 0;
CWnd *CGeneratePeriodicRefresh::m_pWindowForThread = NULL;
Delegate<CGeneratePeriodicRefresh *> CGeneratePeriodicRefresh::OnRefresh;

CGeneratePeriodicRefresh::CGeneratePeriodicRefresh( void ) {
  ++m_nInstances;
}

CGeneratePeriodicRefresh::~CGeneratePeriodicRefresh(void) {
  --m_nInstances;
}

void CGeneratePeriodicRefresh::SetThreadWindow( CWnd *pWindowForThread ) {
  if ( NULL != m_pWindowForThread ) {
    throw std::runtime_error( "Periodic Refresh Window already set" );
  }
  m_pWindowForThread = pWindowForThread;
  hScreenRefreshThread = CreateThread( NULL, 10000, TriggerRefresh, this,
    CREATE_SUSPENDED, &RefreshThreadId );
  if ( NULL == hScreenRefreshThread ) {
    DWORD err = GetLastError();
  }
  else {
    // don't start this until we've got data to draw.
    ResumeThread( hScreenRefreshThread );
  }
}

void CGeneratePeriodicRefresh::ResetThreadWindow() {
  if ( NULL == m_pWindowForThread ) {
    throw std::runtime_error( "Periodic Refresh Window not set" );
  }
  TerminateThread( hScreenRefreshThread, 0 );
}

DWORD WINAPI CGeneratePeriodicRefresh::TriggerRefresh( LPVOID lpParameter) {

  //CGeneratePeriodicRefresh *p = ( CGeneratePeriodicRefresh * ) lpParameter;
  while ( true ) {
    Sleep( 250 ); 
    //frame->SetRefreshFlag();  // indicate time to refresh
    //p->m_pWindowForThread->SendMessage( WM_PERIODICREFRESH );
    m_pWindowForThread->SendMessage( WM_PERIODICREFRESH );
  }
  return 0;
}



  //ON_MESSAGE( WM_CUSTOMREDRAW, OnCustomRedraw )

//CTradeFrame::OnCustomRedraw