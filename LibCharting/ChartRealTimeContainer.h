#pragma once

// holds the ChartRealTime MVC classes together

#include "..\GTScalp\Resource.h"
#include "ChartRealTimeController.h"
#include "ChartRealTimeModel.h"
#include "ChartRealTimeView.h" 

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

class CChartRealTimeContainer : public CDialog {
  DECLARE_DYNAMIC(CChartRealTimeContainer)
public:
  CChartRealTimeContainer(const std::string &sTitle, CWnd* pParent = NULL);
  virtual ~CChartRealTimeContainer(void);
  void SetTitleBarText( const std::string &sTitle ) { m_sDialogTitle = sTitle; };

// Dialog Data
	enum { IDD = IDD_VUCHART };  // fake it with someone elses dialog description for now
protected:
  std::string m_sDialogTitle;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg BOOL OnInitDialog();
	afx_msg void OnDestroy();

  CChartRealTimeModel m_rtModel;  // needs to be created first
  CChartRealTimeView m_rtView;  // needs to be created second
  CChartRealTimeController m_rtControl;   // needs to be created third

	DECLARE_MESSAGE_MAP()
private:
};
