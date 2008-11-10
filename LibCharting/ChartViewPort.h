#pragma once

#include "ChartDataView.h"
#include "ChartMaster.h"
#include "GUIFrameBase.h"

// CChartViewPort is the window used to hold the master chart

class CChartViewPort : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartViewPort)
public:
  CChartViewPort(CChartDataView *cdv, CWnd* pParent = NULL);
  virtual ~CChartViewPort( void );
  void SetChartDataView( CChartDataView *cdv ) { m_cm.SetChartDataView( cdv ); };
  CChartDataView *GetChartDataView( void ) { return m_cm.GetChartDataView(); };
protected:
  CChartMaster m_cm;
  void SetChartMasterSize( void );
	afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
  afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );



  afx_msg void OnViewPortChanged();

private:
	DECLARE_MESSAGE_MAP()
};

// have the refresh on only when there is a DataView available