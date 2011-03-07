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

  HWND Create( HWND hWndParent, LPARAM dwInitParam = NULL );

	BEGIN_MSG_MAP_EX(CTradeGldOptionsView)
    COMMAND_HANDLER(IDC_BTN_SIM_CONNECT, BN_CLICKED, OnBnClickedBtnSimConnect)
    COMMAND_HANDLER(IDC_BTN_SIM_DISCONNECT, BN_CLICKED, OnBnClickedBtnSimDisconnet)
    COMMAND_HANDLER(IDC_BTN_SIM_START, BN_CLICKED, OnBnClickedBtnSimStart)
    COMMAND_HANDLER(IDC_BTN_SIM_STOP, BN_CLICKED, OnBnClickedBtnSimStop)
    COMMAND_HANDLER(IDC_BTN_IB_CONNECT, BN_CLICKED, OnBnClickedBtnIbConnect)
    COMMAND_HANDLER(IDC_BTN_IB_DISCONNECT, BN_CLICKED, OnBnClickedBtnIbDisconnect)
    COMMAND_HANDLER(IDC_BTN_IQFEED_CONNECT, BN_CLICKED, OnBnClickedBtnIqfeedConnect)
    COMMAND_HANDLER(IDC_BTN_IQFEED_DISCONNECT, BN_CLICKED, OnBnClickedBtnIqfeedDisconnect)
    COMMAND_HANDLER(IDC_BTN_START_TRADING, BN_CLICKED, OnBnClickedBtnStartTrading)
    COMMAND_HANDLER(IDC_BTN_STOP_TRADING, BN_CLICKED, OnBnClickedBtnStopTrading)
    COMMAND_HANDLER(IDC_BTN_START_WATCH, BN_CLICKED, OnBnClickedBtnStartWatch)
    COMMAND_HANDLER(IDC_BTN_STOP_WATCH, BN_CLICKED, OnBnClickedBtnStopWatch)
    COMMAND_HANDLER(IDC_BTN_SAVE_SERIES, BN_CLICKED, OnBnClickedBtnSaveSeries)
    COMMAND_HANDLER(IDC_BTN_EMIT_STATS, BN_CLICKED, OnBnClickedBtnEmitStats)
    COMMAND_HANDLER(IDC_RDIO_MODE_LIVE, BN_CLICKED, OnBnClickedRdioModeLive)
    COMMAND_HANDLER(IDC_RDIO_MODE_SIM, BN_CLICKED, OnBnClickedRdioModeSim)
    COMMAND_HANDLER(IDC_RDIO_DATA_IQFEED, BN_CLICKED, OnBnClickedRdioDataIqfeed)
    COMMAND_HANDLER(IDC_RDIO_DATA_IB, BN_CLICKED, OnBnClickedRdioDataIb)
    COMMAND_HANDLER(IDC_BTN_SAVEUNDERLYING, BN_CLICKED, OnBnClickedBtnSaveUnderlying)
    COMMAND_HANDLER(IDC_BTN_SAVEOPTIONS, BN_CLICKED, OnBnClickedBtnSaveOptions)
    MSG_WM_MOVE(OnMove)
    MSG_WM_SIZE(OnSize)
  END_MSG_MAP()

protected:
private:

  CProcess m_process;

  CButton* m_prdioDataIQFeed;
  CButton* m_prdioDataIB;
  CButton* m_pbtnSimConnect;
  CButton* m_pbtnSimStart;
  CButton* m_pbtnSimStop;
  CButton* m_pbtnSimDisconnect;
  CButton* m_pbtnWatchStart;
  CButton* m_pbtnWatchStop;
  CButton* m_pbtnSaveSeries;
  CButton* m_pbtnEmitStats;
  CButton* m_pbtnIBConnect;
  CButton* m_pbtnIBDisconnect;
  CButton* m_pbtnIQConnect;
  CButton* m_pbtnIQDisconnect;

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
 
  LRESULT OnBnClickedBtnSaveSeries(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnEmitStats(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  LRESULT OnBnClickedBtnSimConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnSimDisconnet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnSimStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnSimStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  void OnMove(CPoint ptPos);
  void OnSize(UINT nType, CSize size);
//  BEGIN_MSG_MAP(CTradeGldOptionsView)
//  END_MSG_MAP()
  LRESULT OnBnClickedRdioModeLive(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedRdioModeSim(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedRdioDataIqfeed(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedRdioDataIb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnSaveUnderlying(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBtnSaveOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
