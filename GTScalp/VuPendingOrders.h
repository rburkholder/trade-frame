#pragma once
//#include "afxcmn.h"
#include "Resource.h"
#include "ctlpendingorders.h"

// CVuPendingOrders dialog

class CVuPendingOrders : public CDialog {
	DECLARE_DYNAMIC(CVuPendingOrders)

public:
	CVuPendingOrders(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVuPendingOrders();

// Dialog Data
	enum { IDD = IDD_ORDERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
  afx_msg BOOL OnInitDialog();
  CCtlPendingOrders po1;
  CCtlPendingOrders po2;
};
