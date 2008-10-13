#pragma once
//#include "afxwin.h"

// can be used in IQFeed, PerioidicRefresh

#define WM_GUITHREADCROSSING (WM_APP + 1)

// creation/construction needs to occur in GUI process main/gui thread
class CGuiThreadCrossing :  public CWnd {
  DECLARE_DYNAMIC(CGuiThreadCrossing)
public:
  CGuiThreadCrossing(void);
  virtual ~CGuiThreadCrossing(void);
protected:
	DECLARE_MESSAGE_MAP()
  virtual LRESULT OnCrossThreadArrival( WPARAM w, LPARAM l );
  virtual afx_msg void OnDestroy();
private:
  CWinThread *m_pCreationThread;
};
