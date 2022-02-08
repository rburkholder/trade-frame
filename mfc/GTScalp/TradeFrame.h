#pragma once

#include <vector>
#include "Colour.h"
#include "VisibleRow.h"
//#include "DataRow.h"
#include "DataRows.h"
#include "TradingLogic.h"
#include "GeneratePeriodicRefresh.h"

// CTradeFrame dialog

//DWORD WINAPI TriggerRefresh(LPVOID);

class CTradingLogic;

//class CTradeFrame : public CDialog
class CTradeFrame : public CFrameWnd {
	DECLARE_DYNAMIC(CTradeFrame)

public:
	CTradeFrame( CTradingLogic *pTradingLogic );   // standard constructor
	virtual ~CTradeFrame();
  //virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
  virtual BOOL Create(CWnd* pParentWnd = NULL);
  //void NewQuoteLevel1( int BidSize, double BidPrice, int AskSize, double AskPrice );
  void NewQuoteLevel1( const Quote &quote );
  //void NewPrint( double Price, DWORD Shares );
  void NewPrint( const Trade &trade );
  void NewLevel2Ask( int cntLevels, long *pShares, double *pPrices );
  void NewLevel2Bid( int cntLevels, long *pShares, double *pPrices );

  void SetAllowRedraw( bool b) { m_bAllowRedraw = b; };

  void HandleAcct1OnOpenPosition( double dblOpenPrice, char chOpenSide, int nOpenShares );
  void HandleAcct2OnOpenPosition( double dblOpenPrice, char chOpenSide, int nOpenShares );

  void AppendStaticIndicator( double price, const char *ind );
  void AddDynamicIndicator( double price, const char *ind );
  void RemoveDynamicIndicator( double price, const char *ind );

  //void SetTitleBarText( const char* pText ) { CFrameWnd::SetWindowTextA( pText ); };
  void SetTitleBarText( const std::string &sTitle ) { m_sChartTitle = sTitle; };

private:
  //bool bMouseIsDown;
  bool bSizing;
  CRect rectSize;
  bool bMoving;
  CRect rectLocation;
  //CRect rectLabel;
  long cntSharesPerOrder;
  bool m_bAllowRedraw;

  std::string m_sChartTitle;

  CTradingLogic *m_pTradingLogic;

  static const int FontHeight = 15;
  static const int RowHeight = 16;
  static const int BorderWidth = 5;
  static const int PriceMultiplier = 100;

  int m_RowCount;
  //int m_PriceRowCount;
  std::vector<CVisibleRow*> m_vpVisibleRows;
  //std::vector<CDataRow*> m_vpRowData;

  //COLORREF crAccount1;
  //COLORREF crAccount2;
  //COLORREF crPrice;
  //COLORREF crColumnHeader;

  void DeleteAllRows();
  void DrawRows();

  CDataRows m_DataRows;
  unsigned int ixFirstVisibleRow;
  unsigned int ixLastVisibleRow;
  unsigned int cntVisibleRows;
  unsigned int cntFrameRows;
  unsigned int ixHiRecenterFrame;
  unsigned int ixLoRecenterFrame;
  unsigned int cntCenterRows; // cnt of non frame rows

  unsigned int ixLastPricePrint;
  unsigned int ixLastAskPrint;
  unsigned int ixLastBidPrint;

  //CDataRow *MapPriceToDataRow( double );
  void RecenterVisible( unsigned int ixPrice );

  static const int nLevels = 10;
  unsigned int oldAskPrices[ nLevels ];
  long   oldAskShares[ nLevels ];
  unsigned int oldBidPrices[ nLevels ];
  long   oldBidShares[ nLevels ];

  int nAcct1OpenShares;
  double dblAcct1OpenPrice;
  char chAcct1OpenSide;
  double Acct1Value; // from HandleAcct1OnOpenPosition

  int nAcct2OpenShares;
  double dblAcct2OpenPrice;
  char chAcct2OpenSide;
  double Acct2Value; // from HandleAcct2OnOpenPosition

  double LastPrint;
  double AcctValue;  // from UpdateProfitLossIndications

  bool bShift;
  bool bControl;
  bool bAlt;

  Trade m_latestTrade;
  Quote m_latestQuote;
  double dblQuoteMidPoint;

  void HandleRowInsert( CDataRow* pDataRow );

  void HandleAcct1BidLClk( int );
  void HandleAcct2BidLClk( int );
  void HandleAcct1BidRClk( int );
  void HandleAcct2BidRClk( int );
  void HandleAcct1AskLClk( int );
  void HandleAcct2AskLClk( int );
  void HandleAcct1AskRClk( int );
  void HandleAcct2AskRClk( int );

  void UpdateProfitLossIndications();

protected:

  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
  afx_msg void OnMove(int x, int y);
  afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
  afx_msg void OnSize(UINT nType, int cx, int cy);
 	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg UINT OnNcHitTest( CPoint point );
  afx_msg void OnNcLButtonDown( UINT, CPoint );
  afx_msg void OnNcLButtonUp( UINT, CPoint );
  afx_msg void OnNcCalcSize( BOOL, NCCALCSIZE_PARAMS FAR* );
  afx_msg void OnNcMouseMove( UINT, CPoint );
  afx_msg HBRUSH OnCtlColor( CDC*, CWnd*, UINT );
  afx_msg BOOL OnEraseBkgnd( CDC * );
  afx_msg void OnDestroy( );
  afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
  afx_msg void OnSysChar( UINT nChar, UINT nRepCnt, UINT nFlags );
  afx_msg void OnKeyDown( UINT, UINT, UINT );
  afx_msg void OnKeyUp( UINT, UINT, UINT );
  afx_msg void OnSysKeyDown( UINT, UINT, UINT );
  afx_msg void OnSysKeyUp( UINT, UINT, UINT );

  //afx_msg LRESULT OnCustomRedraw( WPARAM, LPARAM );

  void OnPeriodicRefresh( CGeneratePeriodicRefresh* );

  CGeneratePeriodicRefresh m_refresh;

  //virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
  virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
//	enum { IDD = IDD_TRADEFRAME };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
