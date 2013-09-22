// GTScalpDlg.h : header file
//

#pragma once

#include <vector>
using namespace std;

//#include "afxwin.h"
#include "TradingLogic.h"
#include "VuIndicies.h"
#include "VuOptions.h"
#include "VuSymbolsOfInterest.h"
//#include "ConsoleMessages.h"
#include "VuChartArmsIntraDay.h"
//#include "HDF5.h"
//#include "afxdtctl.h"
#include "Scripts.h"
#include "SymbolSelectionFilter.h"

#include "Pivots.h"
#include "VuChart.h"
#include "ChartDirector\FinanceChart.h"
#include "VuMarketDepth.h"
//#include "GeneratePeriodicRefresh.h" 

#include "IQFeedProviderSingleton.h"
#include "IBTWS.h"
#include "GTSessionX.h"

#include "TradingEnumerations.h"
#include "SimulationProvider.h"

#include "..\LibBasketTrading\BasketTradeContainer.h"

#include "ConsoleCoutMessages.h"

#include "CalcAboveBelow.h"
#include "PositionOptionDeltasWnd.h"

// CGTScalpDlg dialog
class CGTScalpDlg : public CDialog {
	DECLARE_DYNAMIC(CGTScalpDlg)
// Construction
public:
	CGTScalpDlg(CWnd* pParent = NULL);	// standard constructor
  virtual ~CGTScalpDlg( void );

// Dialog Data
	enum { IDD = IDD_GTSCALP_DIALOG };

  bool bLoggedIn;


private:
  //CTradingLogic *pTradingLogic;
  CVuOptions* options;
  CVuPendingOrders* pvpo;
  CVuIndicies* pvi;
  CVuSymbolsOfInterest* psoi;
  CConsoleMessages* pNews;
  CVuChartArmsIntraDay* pvuArms;
  //CHDF5 *pHdf5;

  enum enumScanType { NoScanType, Darvas, Bollinger, Breakout, Volatility, TenPercent };
  enumScanType m_eScanType;
  SymbolSelectionFilter::enumDayCalc m_eDayCalc;
  bool m_bUseDayStart;
  bool m_bUseDayEnd;
  enum enumDataSourceType { NoDS, DSIQFeed, DSIB, DSGenesis1, DSGenesis2, DSSimulation };
  enumDataSourceType m_eDataSourceType;
  enum enumExectionType { NoExec, ExecIB, ExecGenesis1, ExecGenesis2, ExecSimulation };
  enumExectionType m_eExecutionType;

  OrderSide::enumOrderSide m_eOrderSide;
  OrderType::enumOrderType m_eOrderType;

  CGTSessionX m_session1;
  CGTSessionX m_session2;

  IBTWS* m_pIB;
  CIQFeedProviderSingleton* m_pIQFeedSingleton;
  IQFeedProvider* m_pIQFeed;
  SimulationProvider* m_pSimulation;
  void HandleSymbolForBasketContainer( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );

  CConsoleCoutMessages* pConsoleMessages;

  ProviderInterface* m_pExecutionProvider;
  ProviderInterface* m_pDataProvider;

//  COrderManager m_OrderManager;  // keeps at least one instance alive

  CBasketTradeContainer* m_pBasketTrade;

  bool m_bOutsideRTH;  // m_cbOutsideRTH

  std::vector<CalcAboveBelow*> m_vCalcAB;
  //CalcAboveBelow *m_pCalcAboveBelow;
  CPositionOptionDeltasWnd* m_pwndOptiondDeltas;

  void CloseEverything( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

  ProviderInterface* GetExecutionProvider( void );
  ProviderInterface* GetDataProvider( void );

// Implementation
protected:

//  CGeneratePeriodicRefresh m_refresh;

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
  void OnNewsMessage( IQFNewsMessage *pMsg );
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
  afx_msg void OnBnClickedIqfeedloadsymbols();
  afx_msg void OnBnClickedLoaddailydata();
  afx_msg void OnBnClickedDnlddaysymbol();
  afx_msg void OnBnClickedIqfeedcmd();
  CComboBox m_lbIQCommands;
  CEdit m_edtEntry1;
  CDateTimeCtrl m_dtLastDate;
  CDateTimeCtrl m_dtLastTime;
  CDateTimeCtrl m_dtStartDate;
  CDateTimeCtrl m_dtStartTime;
  CButton m_rbEndSelector1;
  afx_msg void OnBnClickedEnddayselect();
  afx_msg void OnBnClickedEndbarcount();
  afx_msg void OnBnClickedEnddaycount();
  CStatic m_grpEndDay;
  afx_msg void OnBnClickedBtnscan();
  afx_msg void OnBnClickedRbdarvas();
  afx_msg void OnBnClickedRbbollinger();
  afx_msg void OnBnClickedRbbreakout();
  CStatic m_grpScanType;
  afx_msg void OnBnClickedUsedaystart();
  afx_msg void OnBnClickedUsedayend();
  CButton m_cbUseDayStart;
  CButton m_cbUseDayEnd;
  CButton m_rbSelectByDay;
  CButton m_rbSelectByBarCount;
  CButton m_rbSelectByDayCount;
  afx_msg void OnBnClickedOpenib();
  CEdit m_edtIBAcctCode;
  afx_msg void OnBnClickedRadio1();
  afx_msg void OnBnClickedRadio2();
  afx_msg void OnBnClickedIbwatch();
  afx_msg void OnBnClickedIbunwatch();
  afx_msg void OnBnClickedIbclose();
  afx_msg void OnBnClickedDsiqfeed();
  afx_msg void OnBnClickedDsib();
  afx_msg void OnBnClickedDsgt1();
  afx_msg void OnBnClickedDsgt2();
  CStatic m_grpDataSource;
  afx_msg void OnBnClickedChartsymbol();
  afx_msg void OnEnChangeIbacct();
  afx_msg void OnBnClickedRtchart();
  afx_msg void OnBnClickedBasket();
  CEdit m_edtFunds;
  afx_msg void OnBnClickedBasketprepare();
  afx_msg void OnBnClickedAddtestsymbols();
  afx_msg void OnBnClickedExecib();
  afx_msg void OnBnClickedExecgenesis1();
  afx_msg void OnBnClickedExecgenesis2();
  afx_msg void OnBnClickedExec4();
  afx_msg void OnBnClickedExec5();
  CStatic m_grpExecution;
  afx_msg void OnBnClickedCboutsiderth();
  CButton m_cbOutsideRTH;
  CStatic m_grpOrderSide;
  CStatic m_grpTradeType;
  CEdit m_ebLimitPrice;
  CEdit m_ebStopPrice;
  afx_msg void OnBnClickedRadio11();
  afx_msg void OnBnClickedRbosbuy();
  afx_msg void OnBnClickedRbossell();
  afx_msg void OnBnClickedRbttmkt();
  afx_msg void OnBnClickedRbttlmt();
  afx_msg void OnBnClickedRbttstp();
  afx_msg void OnBnClickedRbttstplmit();
  afx_msg void OnBnClickedRbtt01();
  afx_msg void OnBnClickedRbtt02();
  afx_msg void OnBnClickedBtnorder();
  CEdit m_edtPassword;
  CButton m_btnIBExecution;
  CButton m_btnIBData;
  CButton m_btnIQFeedData;
  afx_msg void OnBnClickedIqfwindows();
  CButton m_btnIQFWindows;
  afx_msg void OnBnClickedBtnsavesymbol();
  CEdit m_edtOriginalSymbolName;
  CEdit m_edtAlternateSymbolName;
  afx_msg void OnBnClickedBtnloadbasket();
  afx_msg void OnBnClickedBtnsavebasket();
  afx_msg void OnBnClickedBtnsafebsktdata();
  afx_msg void OnEnChangeEntry1();
  afx_msg void OnBnClickedDssimulation();
  afx_msg void OnBnClickedExecsimulation();
  afx_msg void OnBnClickedCbsimulator();
  CButton m_cbSimulatorOn;
  CButton m_btnSimulatorExecution;
  CButton m_btnSimulatorDataSource;
  afx_msg void OnBnClickedBtnsimuassigndir();
  afx_msg void OnBnClickedBtnrunsim();
  afx_msg void OnBnClickedBtnhdf5flush();
  afx_msg void OnBnClickedBtnstopsim();
  afx_msg void OnBnClickedBtnskunk();
};
