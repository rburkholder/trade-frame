#pragma once
#include "GUIFrameBase.h"
#include "ChartInstrumentTree.h"

class CChartRealTimeTreeView : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartRealTimeTreeView)
public:
  CChartRealTimeTreeView(void);
  virtual ~CChartRealTimeTreeView(void);
protected:
	afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  CChartInstrumentTree m_Tree;
private:
  DECLARE_MESSAGE_MAP()
};
