#pragma once

#include "GUIFrameBase.h"

// CChartViewPort is used to hold the master chart

class CChartViewPort : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartViewPort)
public:
  CChartViewPort(size_t ix, CWnd* pParent = NULL);
  virtual ~CChartViewPort( void );
  //void SetViewPortIndex( size_t ix ) { m_ixViewPort = ix; };
  size_t GetViewPortIndex( void ) { return m_ixViewPort; };
protected:
  size_t m_ixViewPort;
	afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	DECLARE_MESSAGE_MAP()
};
