#pragma once
//#include "afxwin.h"

// can be used in IQFeed, PerioidicRefresh
// incorporate into our CGUIFrameBase class to act as a C#-like cross thread delegate
//   hmm, maybe not as some classes need cross thread action but don't have a CWnd base, but do operate in the GUI thread

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
  CWinThread *m_pThreadMain;
private:
};
