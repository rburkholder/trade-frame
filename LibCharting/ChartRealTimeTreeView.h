#pragma once
#include "GUIFrameBase.h"

class CChartRealTimeTreeView : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartRealTimeTreeView)
public:
  CChartRealTimeTreeView(void);
  ~CChartRealTimeTreeView(void);
protected:
	afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  CTreeCtrl m_Tree;
private:
  DECLARE_MESSAGE_MAP()
};
