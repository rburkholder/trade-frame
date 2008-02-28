#pragma once
//#include "afxcmn.h"
#include "Resource.h"
#include "CtlMarketDepth.h"

// CVuMarketDepth dialog

class CVuMarketDepth : public CDialog {
	DECLARE_DYNAMIC(CVuMarketDepth)

public:
	CVuMarketDepth(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVuMarketDepth();

// Dialog Data
	enum { IDD = IDD_VUMARKETDEPTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  afx_msg BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
  CCtlMarketDepth m_lcMarketDepthBid;
  CCtlMarketDepth m_lcMarketDepthAsk;
};
