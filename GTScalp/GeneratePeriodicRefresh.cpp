#include "StdAfx.h"
//#include "GTScalp.h"
#include "GTWindowsConstants.h"
#include "GeneratePeriodicRefresh.h"

CGeneratePeriodicRefresh::CGeneratePeriodicRefresh(CWnd *pWindowForThread) {
//  bRefresh = true;
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

CGeneratePeriodicRefresh::~CGeneratePeriodicRefresh(void) {
  TerminateThread( hScreenRefreshThread, 0 );
}

DWORD WINAPI CGeneratePeriodicRefresh::TriggerRefresh( LPVOID lpParameter) {

  CGeneratePeriodicRefresh *p = ( CGeneratePeriodicRefresh * ) lpParameter;
  while ( true ) {
    Sleep( 250 ); 
    //frame->SetRefreshFlag();  // indicate time to refresh
    p->m_pWindowForThread->SendMessage( WM_PERIODICREFRESH );
  }
  return 0;
}



  //ON_MESSAGE( WM_CUSTOMREDRAW, OnCustomRedraw )

//CTradeFrame::OnCustomRedraw