#pragma once

// holds the ChartRealTime MVC classes together

#include "..\GTScalp\Resource.h"
#include "ChartRealTimeController.h"
#include "ChartRealTimeModel.h"
#include "ChartRealTimeView.h" 
#include "ProviderInterface.h"

//           ChartRealTimeModel
//             ^        ^
//             |        |
// ChartRealTimeView    |
//             ^        |
//             |        |
//           ChartRealTimeController
//
// up arrows are method invocations
// events:
//    ChartRealTimeModel -> ChartRealTimeView
//    ChartRealTimeView  -> ChartRealTimeController

// ChartRealTimeView are individual charts (a control, rather than a window
// ChartRealTimeContainer is an example of how to hold multiple charts, a window rather than a control

class CChartRealTimeContainer : public CFrameWnd {
  DECLARE_DYNAMIC(CChartRealTimeContainer)
public:
  CChartRealTimeContainer(
    const std::string &sSymbol,
    CProviderInterface *pProvider,
    CWnd* pParent = NULL);
  virtual ~CChartRealTimeContainer(void);
  void SetTitleBarText( const std::string &sTitle ) { m_sDialogTitle = sTitle; };

// Dialog Data
	//enum { IDD = IDD_VUCHART };  // fake it with someone elses dialog description for now
protected:
  std::string m_sDialogTitle;
  std::string m_sSymbol;
  CProviderInterface *m_pDataProvider;
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDestroy();

  CChartRealTimeModel *m_prtModel;  // needs to be created first
  CChartRealTimeView *m_prtView;  // needs to be created second
  CChartRealTimeController *m_prtControl;   // needs to be created third

	DECLARE_MESSAGE_MAP()
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
private:
};
