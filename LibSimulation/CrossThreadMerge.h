#pragma once

#include "mergedateddatums.h"
#include <GuiThreadCrossing.h>

class CCrossThreadMerge : public CMergeDatedDatums, public CGuiThreadCrossing {
  DECLARE_DYNAMIC(CCrossThreadMerge)
public:
  CCrossThreadMerge(void);
  virtual ~CCrossThreadMerge(void);
protected:
  HANDLE m_hSignal;
  LRESULT OnCrossThreadArrival( WPARAM w, LPARAM l );
  inline virtual void ProcessCarrier( CMergeCarrierBase *pCarrier );  // overload, sends message for OnCrossThreadArrival
private:
  DECLARE_MESSAGE_MAP()
  //CWinThread *m_pMainThread;
};
