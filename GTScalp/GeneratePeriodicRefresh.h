#pragma once

#include "Delegate.h"

// should do our own CWnd at some time, use CScalpDlg currently

class CGeneratePeriodicRefresh {
public:
  CGeneratePeriodicRefresh(CWnd *pWindowForThread);
  virtual ~CGeneratePeriodicRefresh(void);
  void HandleRefresh( void ) { // window calls this on each send message to get the calls invoked
    if ( !OnRefresh.IsEmpty() ) OnRefresh( this );
  }
  Delegate<CGeneratePeriodicRefresh *> OnRefresh;
protected:
  HANDLE hScreenRefreshThread;
  DWORD RefreshThreadId;
  CWnd *m_pWindowForThread;
private:
  static DWORD WINAPI TriggerRefresh( LPVOID );

};
