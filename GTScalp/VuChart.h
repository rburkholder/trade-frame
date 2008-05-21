#pragma once
//#include "afxwin.h"
//#include "ChartViewer.h" 
#include "Resource.h"
#include "ChartDatedDatum.h"

// CVuChart dialog

class CVuChart : public CDialog {
	DECLARE_DYNAMIC(CVuChart)

public:
	CVuChart(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVuChart();
  void SetTitleBarText( const std::string &sTitle ) { m_sChartTitle = sTitle; };

// Dialog Data
	enum { IDD = IDD_VUCHART };

protected:
  std::string m_sChartTitle;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
  CChartDatedDatum m_chart;
  afx_msg void OnStnClickedChart();
};
