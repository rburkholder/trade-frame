/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

// TapeReaderView.cpp : implementation of the CTapeReaderView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TapeReaderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CTapeReaderView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

HWND CTapeReaderView::Create(HWND hWndParent, LPARAM dwInitParam) {

  HWND h;
  h = CThisClass::Create( hWndParent, dwInitParam );

  DlgResize_Init( false, true );

  m_edtSymbol = GetDlgItem( IDC_EDTSYMBOL );
  m_btnStart = GetDlgItem( IDC_BTNSTART );
  m_btnStop = GetDlgItem( IDC_BTNSTOP );
  m_lvTape = GetDlgItem( IDC_LISTTAPE );

  int i1 = m_lvTape.AddColumn( "Time", 0 );
  int i2 = m_lvTape.AddColumn( "BATE", 1 );
  int i3 = m_lvTape.AddColumn( "Size", 2 );
  int i4 = m_lvTape.AddColumn( "Price", 3 );

  return h;
}

LRESULT CTapeReaderView::OnBnClickedBtnstart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: Add your control notification handler code here

  return 0;
}

LRESULT CTapeReaderView::OnBnClickedBtnstop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: Add your control notification handler code here

  return 0;
}

LRESULT CTapeReaderView::OnEnChangeEdtsymbol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialogImpl<CTapeReaderView>::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here

  return 0;
}


LRESULT CTapeReaderView::OnLvnItemchangedListtape(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  // TODO: Add your control notification handler code here

  return 0;
}
