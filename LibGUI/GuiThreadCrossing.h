#pragma once
//#include "afxwin.h"

// can be used in IQFeed, PerioidicRefresh

#define WM_GUITHREADCROSSING (WM_APP + 1)

// creation/construction needs to occur in GUI process main/gui thread
class CGuiThreadCrossing :  public CWnd {
  DECLARE_DYNAMIC(CGuiThreadCrossing)
public:
  CGuiThreadCrossing(void);
  ~CGuiThreadCrossing(void);
protected:
  CWinThread *m_pCreationThread;
private:
	DECLARE_MESSAGE_MAP()

  LRESULT OnCrossThreadArrival( WPARAM w, LPARAM l );
  afx_msg void OnDestroy();
};
