// TradeGldOptionsView.h : interface of the CTradeGldOptionsView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Process.h"

class CTradeGldOptionsView : 
  public CDialogImpl<CTradeGldOptionsView>
{
public:
	enum { IDD = IDD_TRADEGLDOPTIONS_FORM };

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(CTradeGldOptionsView)
    COMMAND_HANDLER(IDC_BTN_IB_CONNECT, BN_CLICKED, OnBnClickedBtnIbConnect)
    COMMAND_HANDLER(IDC_BTN_IB_DISCONNECT, BN_CLICKED, OnBnClickedBtnIbDisconnect)
    COMMAND_HANDLER(IDC_BTN_IQFEED_CONNECT, BN_CLICKED, OnBnClickedBtnIqfeedConnect)
    COMMAND_HANDLER(IDC_BTN_IQFEED_DISCONNECT, BN_CLICKED, OnBnClickedBtnIqfeedDisconnect)
    COMMAND_HANDLER(IDC_BTN_START_TRADING, BN_CLICKED, OnBnClickedBtnStartTrading)
    COMMAND_HANDLER(IDC_BTN_STOP_TRADING, BN_CLICKED, OnBnClickedBtnStopTrading)
    COMMAND_HANDLER(IDC_BTN_START_WATCH, BN_CLICKED, OnBnClickedBtnStartWatch)
    COMMAND_HANDLER(IDC_BTN_STOP_WATCH, BN_CLICKED, OnBnClickedBtnStopWatch)
    COMMAND_HANDLER(IDC_BTN_SAVE_SERIES, BN_CLICKED, OnBnClickedBtnSaveSeries)
    MSG_WM_MOVE(OnMove)
    MSG_WM_SIZE(OnSize)
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
  LRESULT OnBnClickedBtnStartTrading(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnStopTrading(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnStartWatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnStopWatch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
 
  void OnMove(CPoint ptPos);
  void OnSize(UINT nType, CSize size);
  LRESULT OnBnClickedBtnSaveSeries(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
