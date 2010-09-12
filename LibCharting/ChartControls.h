#pragma once
#include "GUIFrameBase.h"
#include "Delegate.h"

// this should only be instantiate once, only one instance is needed

class CChartControls : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartControls)
public:
  CChartControls(void);
  virtual ~CChartControls(void);
  Delegate<CChartControls *> OnBtnNewMasterChart;
protected:
  CButton m_btnNewMasterChart;
  afx_msg void OnBtnNewMasterChartClicked( void );
private:
  DECLARE_MESSAGE_MAP()
};
