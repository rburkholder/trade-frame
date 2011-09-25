#pragma once
#include "GTOrderX.h"
#include "FastDelegate.h"
using namespace fastdelegate;

// CCtlPendingOrders view

class CCtlPendingOrders : public CListCtrl {
	DECLARE_DYNCREATE(CCtlPendingOrders)

public:
	CCtlPendingOrders();           // protected constructor used by dynamic creation
	virtual ~CCtlPendingOrders();

  void InitControl();

  typedef FastDelegate1<DWORD_PTR> OnCancelHandler; // has pointer to CGTOrderX 
  void SetOnCancelHandler( OnCancelHandler function ) {
    OnCancel = function;
  }

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif

#endif

public:

  void InsertOrder( const char* szID, const char* szTime, 
    const char* chSide, 
    const char* szQuan, const char* szPrice, const char* szStatus, DWORD_PTR object );
  void UpdateOrderStatus( const char* szID, const char* szStatus );
  void DeleteOrder( const char* szID );

protected:

	DECLARE_MESSAGE_MAP()

  CGTOrderX *pOrder;

  OnCancelHandler OnCancel;

  afx_msg void OnLButtonUp( UINT, CPoint );

};


