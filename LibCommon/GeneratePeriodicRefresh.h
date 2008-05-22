#pragma once

#include "Delegate.h"

// should do our own CWnd at some time, use CScalpDlg currently

class CGeneratePeriodicRefresh {
public:
  CGeneratePeriodicRefresh( void );
  virtual ~CGeneratePeriodicRefresh(void);
  void HandleRefresh( void ) { // window calls this on each send message to get the calls invoked
    if ( !OnRefresh.IsEmpty() ) OnRefresh( this );
  }
  void SetThreadWindow( CWnd *pWindowForThread );
  void ResetThreadWindow( void );
  static Delegate<CGeneratePeriodicRefresh *> OnRefresh;
protected:
  static HANDLE hScreenRefreshThread;
  static DWORD RefreshThreadId;
  static CWnd *m_pWindowForThread;
private:
  static DWORD WINAPI TriggerRefresh( LPVOID );
  static unsigned int m_nInstances;

};
