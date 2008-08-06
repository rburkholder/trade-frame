#pragma once
//#include "afxwin.h"

// can be used in IQFeed, PerioidicRefresh

// creation/construction needs to occur in GUI process main/gui thread
class CGuiThreadCrossing :  public CWnd {
  DECLARE_DYNAMIC(CGuiThreadCrossing)
public:
  CGuiThreadCrossing(void);
  ~CGuiThreadCrossing(void);
protected:
  LRESULT OnCrossThreadArrival( WPARAM w, LPARAM l );
  afx_msg void OnDestroy();
private:
	DECLARE_MESSAGE_MAP()
};
