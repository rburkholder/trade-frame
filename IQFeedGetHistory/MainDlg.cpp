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

// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

  m_MsgIdsForIQFeedHistoryQuery.owner = this;
  m_MsgIdsForIQFeedHistoryQuery.msgConnected = WM_IQFEEDHISTORY_CONNECTED;
  m_MsgIdsForIQFeedHistoryQuery.msgSendComplete = WM_IQFEEDHISTORY_SENDCOMPLETE;
  m_MsgIdsForIQFeedHistoryQuery.msgDisconnected = WM_IQFEEDHISTORY_DISCONNECTED;
  m_MsgIdsForIQFeedHistoryQuery.msgError = WM_IQFEEDHISTORY_ERROR;
  m_MsgIdsForIQFeedHistoryQuery.msgHistoryTickDataPoint = WM_IQFEEDHISTORY_TICKDATAPOINT;
  m_MsgIdsForIQFeedHistoryQuery.msgHistoryIntervalData = WM_IQFEEDHISTORY_INTERVALDATA;
  m_MsgIdsForIQFeedHistoryQuery.msgHistorySummaryData = WM_IQFEEDHISTORY_SUMMARYDATA;
  m_MsgIdsForIQFeedHistoryQuery.msgHistoryRequestDone = WM_IQFEEDHISTORY_REQUESTDONE;

  m_pIQFeedHistoryQuery = new ou::tf::CIQFeedHistoryQueryMsgShim<CMainDlg>( m_MsgIdsForIQFeedHistoryQuery );

  m_pIQFeedHistoryQuery->Connect();

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

  delete m_pIQFeedHistoryQuery;

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
  m_DialogCloseCode = nVal;

  m_pIQFeedHistoryQuery->Disconnect();

//	DestroyWindow();
//	::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnIQFeedHistoryConnected( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
//  m_pIQFeedHistoryQuery->RetrieveNEndOfDays( "GLD", 10, 1 );
  m_process.Start();
	return 0;
}

LRESULT CMainDlg::OnIQFeedHistorySendComplete( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
	return 0;
}

LRESULT CMainDlg::OnIQFeedHistoryDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
	DestroyWindow();
	::PostQuitMessage(m_DialogCloseCode);
	return 0;
}

LRESULT CMainDlg::OnIQFeedHistoryError( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
	return 0;
}

LRESULT CMainDlg::OnIQFeedHistoryTickDataPoint( UINT, WPARAM w, LPARAM l, BOOL &bHandled ) {
  m_pIQFeedHistoryQuery->ReQueueTickDataPoint( reinterpret_cast<history_query_t::structTickDataPoint*>( w ) );
	return 0;
}

LRESULT CMainDlg::OnIQFeedHistoryIntervalData( UINT, WPARAM w, LPARAM l, BOOL &bHandled ) {
  m_pIQFeedHistoryQuery->ReQueueInterval( reinterpret_cast<history_query_t::structInterval*>( w ) );
	return 0;
}

LRESULT CMainDlg::OnIQFeedHistorySummaryData( UINT, WPARAM w, LPARAM l, BOOL &bHandled ) {
  m_pIQFeedHistoryQuery->ReQueueSummary( reinterpret_cast<history_query_t::structSummary*>( w ) );
	return 0;
}

LRESULT CMainDlg::OnIQFeedHistoryRequestDone( UINT, WPARAM, LPARAM, BOOL &bHandled ) {
	return 0;
}

