#include "StdAfx.h"
#include "CtlListCtrlBase.h"

IMPLEMENT_DYNAMIC(CCtlListCtrlBase, CListCtrl)

CCtlListCtrlBase::CCtlListCtrlBase(void): CListCtrl() {
}

CCtlListCtrlBase::~CCtlListCtrlBase(void) {
}

void CCtlListCtrlBase::InitControl( void ) {
  SetExtendedStyle( LVS_EX_INFOTIP | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP | LVS_EX_FULLROWSELECT );
  EnableToolTips();
  m_nRowCount = 0;
}

void CCtlListCtrlBase::DrawColumns() {
  m_ColumnHeaders.Draw( this );
}

BEGIN_MESSAGE_MAP(CCtlListCtrlBase, CListCtrl)
END_MESSAGE_MAP()

