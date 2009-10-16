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

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// custom off

#include "TapeReaderView.h"

CTapeReaderView::CTapeReaderView( void ) 
: CDialogImpl<CTapeReaderView>(), CDialogResize<CTapeReaderView>(),
  m_Destinations( this, WM_IQFEED_INITIALIZED, WM_IQFEED_CONNECTED, WM_IQFEED_SENDDONE, WM_IQFEED_DISCONNECTED, WM_IQFEED_ERROR )
{
}

CTapeReaderView::~CTapeReaderView( void ) {
}

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

  m_pIQFeed = new CIQFeed<CTapeReaderView>( &_Module, m_Destinations );

  return h;
}

void CTapeReaderView::OnDestroy( void ) {
  m_pIQFeed->Disconnect();
  delete m_pIQFeed;
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

LRESULT CTapeReaderView::OnIQFeedInitialized( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  m_pIQFeed->Connect();
  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedConnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedDisconnected( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedSendDone( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}

LRESULT CTapeReaderView::OnIQFeedError( UINT, WPARAM, LPARAM, BOOL& bHandled ) {
  bHandled = true;
  return 1;
}
