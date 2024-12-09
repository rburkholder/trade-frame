#pragma once

#include "Resource.h"
#include "ChartDatedDatum.h"

// CVuChart dialog

class CVuChart : public CDialog {
	DECLARE_DYNAMIC(CVuChart)

public:
	CVuChart(const std::string &sTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CVuChart();

// Dialog Data
	enum { IDD = IDD_VUCHART };

protected:
  std::string m_sChartTitle;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
  ChartDatedDatum m_chart;
  afx_msg void OnStnClickedChart();
};
