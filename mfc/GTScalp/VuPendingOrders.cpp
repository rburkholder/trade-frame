// VuPendingOrders.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h"
#include "Resource.h"
#include "VuPendingOrders.h"


// CVuPendingOrders dialog

IMPLEMENT_DYNAMIC(CVuPendingOrders, CDialog)

CVuPendingOrders::CVuPendingOrders(CWnd* pParent /*=NULL*/)
	: CDialog(CVuPendingOrders::IDD, pParent) {

    BOOL b = Create(IDD_ORDERS, pParent );
}

CVuPendingOrders::~CVuPendingOrders() {
  DestroyWindow();
}

BOOL CVuPendingOrders::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();

  po1.InitControl();
  po2.InitControl();

  return b;
}

void CVuPendingOrders::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_ACCT1PENDORD, po1);
  DDX_Control(pDX, IDC_ACCT2PENDORD, po2);
}


BEGIN_MESSAGE_MAP(CVuPendingOrders, CDialog)
END_MESSAGE_MAP()


// CVuPendingOrders message handlers

