#pragma once


// CVuPendingOrders frame

class CVuListCtl : public CFrameWnd {
	DECLARE_DYNCREATE(CVuListCtl)

protected:
	CVuListCtl();           // protected constructor used by dynamic creation
	virtual ~CVuListCtl();
  BOOL Create( CWnd* pParentWnd );

  CListCtrl list;

public:

protected:
	DECLARE_MESSAGE_MAP()
};


