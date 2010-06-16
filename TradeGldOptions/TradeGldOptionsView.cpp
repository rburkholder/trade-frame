// TradeGldOptionsView.cpp : implementation of the CTradeGldOptionsView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "TradeGldOptionsView.h"

BOOL CTradeGldOptionsView::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIbConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.IBConnect();

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIbDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.IBDisconnect();

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIqfeedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.IQFeedConnect();

  return 0;
}

LRESULT CTradeGldOptionsView::OnBnClickedBtnIqfeedDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_process.IQFeedDisconnect();

  return 0;
}
