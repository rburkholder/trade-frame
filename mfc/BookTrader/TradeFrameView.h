#pragma once

// CTradeFrameView

#include <vector>

class CTradeFrameView : public CWnd {
	DECLARE_DYNAMIC(CTradeFrameView)

public:
	CTradeFrameView( unsigned int nRows, bool bUseAcct1, bool bUseAcct2 );
	virtual ~CTradeFrameView();

  enum enumColumnIndex {
    Acct1PL, Acct2PL, AcctPL, 
    BidPend1, BidPend2, BidPend,
    Bid, Price, Ask, 
    AskPend1, AskPend2, AskPend,
    Ticks, Volume, Histogram, 
    StaticIndicators, DynamicIndicators,
    _ColumnCount
  };

  void SetInsideBidRow( short nRow );  // -1 for no row
  void SetInsideAskRow( short nRow );  // -1 for no row
  void SetLastTradeRow( short nRow );  // -1 for no row
  void ArmedForTrading( bool bArmed ); // true to start sending through clicks

  void SetRowContentNoAccounts( );
  void SetRowContentOneAccount();
  void SetRowContentTwoAccounts();

protected:

  unsigned int m_nRows;
  bool m_bUseAcct1;
  bool m_bUseAcct2;

  unsigned int m_nColumns;

  static char *m_rColumnHeaderText[];

  //vector<enumColumnIndex> m_vColumnsUsed;
  //bool m_rbColumnsPresent[]; // quick look to see if column is available
  //struct struct
  short m_rnColumnTranslate[ _ColumnCount ]; // translate enum to screen column, -1 means no column

  enum enumViewStyle { NoAccounts, OneAccount, TwoAccounts } m_eViewStyle;

  int m_nCellWidth;   // Cell width in pixels
  int m_nCellHeight;  // Cell height in pixels
  int m_nRibbonWidth; // Ribbon width in pixels
  int m_nViewWidth;   // Workspace width in pixels
  int m_nViewHeight;  // Workspace height in pixels

  short m_nInsideBidRow;
  short m_nInsideAskRow;
  short m_nTradeRow;
  bool m_bArmedForTrading;

  afx_msg void OnPaint ();
  afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize (UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

private:
};

