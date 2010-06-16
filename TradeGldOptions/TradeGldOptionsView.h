// TradeGldOptionsView.h : interface of the CTradeGldOptionsView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Process.h"

class CTradeGldOptionsView : public CDialogImpl<CTradeGldOptionsView>
{
public:
	enum { IDD = IDD_TRADEGLDOPTIONS_FORM };

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CTradeGldOptionsView)
    COMMAND_HANDLER(IDC_BTN_IB_CONNECT, BN_CLICKED, OnBnClickedBtnIbConnect)
    COMMAND_HANDLER(IDC_BTN_IB_DISCONNECT, BN_CLICKED, OnBnClickedBtnIbDisconnect)
    COMMAND_HANDLER(IDC_BTN_IQFEED_CONNECT, BN_CLICKED, OnBnClickedBtnIqfeedConnect)
    COMMAND_HANDLER(IDC_BTN_IQFEED_DISCONNECT, BN_CLICKED, OnBnClickedBtnIqfeedDisconnect)
  END_MSG_MAP()

protected:
private:

  CProcess m_process;

public:

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
LRESULT OnBnClickedBtnIbConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
LRESULT OnBnClickedBtnIbDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
LRESULT OnBnClickedBtnIqfeedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
LRESULT OnBnClickedBtnIqfeedDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
