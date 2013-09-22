#pragma once
//#include "afxwin.h"
#include "Resource.h"
#include "TimeSeries.h"
#include "IQFeedRetrieveHistory.h"
#include "ChartArmsIntraDay.h"
#include "MergeDatedDatums.h"
#include "ChartDatedDatum.h"
#include "IQFeedProviderSingleton.h"

// CVuChartArmsIntraDay dialog

class CVuChartArmsIntraDay : public CDialog {
	DECLARE_DYNAMIC(CVuChartArmsIntraDay)

public:
	CVuChartArmsIntraDay(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVuChartArmsIntraDay();

// Dialog Data
	enum { IDD = IDD_VUCHARTARMSINTRADAY };

  void StartCharts( bool bLive, unsigned int nDaysAgo );

  CChartArmsIntraDay m_ChartArmsIntraDay;
  ChartDatedDatum m_ChartIndu;
  ChartDatedDatum m_ChartTrin;
  ChartDatedDatum m_ChartTick;

  void OnInduHistoryDone( IQFeedHistory* pHistory );
  void OnTickHistoryDone( IQFeedHistory* pHistory );
  void OnTrinHistoryDone( IQFeedHistory* pHistory );

  void ProcessHistory( void );

  afx_msg void OnStnClickedTick();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

  MergeDatedDatums merge;

  Trades *pTradesIndu, *pTradesTrin, *pTradesTick;
  Quotes *pQuotesIndu, *pQuotesTrin, *pQuotesTick;

  IQFeedHistoryHT *pHistoryIndu, *pHistoryTrin, *pHistoryTick;
  bool m_bInduHistoryDone, m_bTrinHistoryDone, m_bTickHistoryDone;

  void ProcessMergeIndu( const Trade &trade );
  void ProcessMergeTrin( const Trade &trade );
  void ProcessMergeTick( const Trade &trade );

  void HandleInduUpdate( IQFeedSymbol *pSym );
  void HandleTrinUpdate( IQFeedSymbol *pSym );
  void HandleTickUpdate( IQFeedSymbol *pSym );

  void HandleRealTime( void );

  CIQFeedProviderSingleton m_IQFeedProvider;

  afx_msg BOOL OnInitDialog();

private:

public:
};
