// GTScalpDlg.h : header file
//

#pragma once

#include "afxwin.h"
#include "TradingLogic.h"
#include "VuIndicies.h"
#include "VuOptions.h"
#include "VuSymbolsOfInterest.h"
#include "ConsoleMessages.h"
#include "VuChartArmsIntraDay.h"
#include <vector>
#include "HDF5.h"

using namespace std;

// CGTScalpDlg dialog
class CGTScalpDlg : public CDialog {
// Construction
public:
	CGTScalpDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GTSCALP_DIALOG };

  bool bLoggedIn;


private:
  //CTradingLogic *pTradingLogic;
  CVuOptions *options;
  CVuPendingOrders *pvpo;
  CVuIndicies *pvi;
  CVuSymbolsOfInterest *psoi;
  CConsoleMessages *pNews;
  CVuChartArmsIntraDay *pvuArms;
  CHDF5 *pHdf5;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support




// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedMessage();
  //GTSessionX m_session;
  afx_msg void OnBnClickedLogin();
  afx_msg void OnBnClickedLogout();
  afx_msg void OnBnClickedStart();
  CStatic SomeTextCtrl;
  CString SomeTextVal;
  //void CleanUpStuff( void );
  afx_msg void OnDestroy( ); // isn't called?
  afx_msg void OnCancel( );
  afx_msg void OnClose( );
  afx_msg void OnOK( );
  afx_msg void OnBnClickedAccounts();
  afx_msg void OnBnClickedIqfeed();
  CListBox m_lbExecAddr;
  CListBox m_lbExecPort;
  CListBox m_lbLvl1Addr;
  CListBox m_lbLvl2Addr;
  CListBox m_lbLvl1Port;
  CListBox m_lbLvl2Port;
  afx_msg void OnBnClickedBtnpendord();
  afx_msg void OnBnClickedBtnOptions();
  void OnNewsMessage( CIQFNewsMessage *pMsg );
  afx_msg void OnBnClickedBtnhistory();
  afx_msg void OnBnClickedVuarms();
  afx_msg void OnBnClickedLive();
  CButton m_btnLive;
  CEdit m_edtDaysAgo;
  afx_msg void OnEnChangeDaysago();
  afx_msg void OnBnClickedAllowtrades();
  CButton m_cbAllowTrades;
  CComboBox m_lbSymbolList;
  vector<CTradingLogic *> m_vTradingLogic;
  afx_msg void OnCbnSelchangeSymbollist();
  afx_msg LRESULT OnPeriodicRefresh( WPARAM w, LPARAM l );
  afx_msg void OnBnClickedBtnhfs();
  afx_msg void OnBnClickedIqfeedloadsymbols();
  afx_msg void OnBnClickedLoaddailydata();
  afx_msg void OnBnClickedTesthdf5();
  afx_msg void OnBnClickedDnlddaysymbol();
  afx_msg void OnBnClickedIqfeedcmd();
  CComboBox m_lbIQCommands;
  CEdit m_edtEntry1;
  afx_msg void OnBnClickedIterate();
};
