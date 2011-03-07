// TradeGldOptionsView.cpp : implementation of the CTradeGldOptionsView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TradeGldOptionsView.h"

BOOL CTradeGldOptionsView::PreTranslateMessage(MSG* pMsg) {
	return CWindow::IsDialogMessage(pMsg);
}

HWND CTradeGldOptionsView::Create( HWND hWndParent, LPARAM dwInitParam ) {
  HWND h = CDialogImpl<CTradeGldOptionsView>::Create( hWndParent, dwInitParam );
  //m_prdioDataIQFeed = (CButton*) GetDlgItem( IDC_RDIO_DATA_IQFEED );
  //m_prdioDataIB = (CButton*) GetDlgItem( IDC_RDIO_DATA_IB );
  return h;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIbConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.IBConnect();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIbDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.IBDisconnect();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIqfeedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.IQFeedConnect();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIqfeedDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.IQFeedDisconnect();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnStartTrading(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.StartTrading();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnStopTrading(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.StopTrading();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnStartWatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.StartWatch();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnStopWatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.StopWatch();
  return 0;
}

void CTradeGldOptionsView::OnMove(CPoint ptPos) {
}

void CTradeGldOptionsView::OnSize(UINT nType, CSize size) {
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnSaveSeries(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.SaveSeries();

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnSimConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.SimConnect();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnSimDisconnet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.SimDisconnect();
  return 0;
}


LRESULT CTradeGldOptionsView::OnBnClickedBtnEmitStats(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.EmitStats();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnSimStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.SimStart();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnSimStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  m_process.SimStop();
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedRdioModeLive(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedRdioModeSim(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedRdioDataIqfeed(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedRdioDataIb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnSaveUnderlying(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnSaveOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {

  return 0;
}
