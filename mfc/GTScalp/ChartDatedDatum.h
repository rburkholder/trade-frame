#pragma once
#include "ChartViewerShim.h"
#include "SlidingWindow.h"
#include "DatedDatum.h"
#include "BarFactory.h"
#include "ChartDirector\FinanceChart.h"

class ChartDatedDatum : public CChartViewerShim {
public:
  ChartDatedDatum(void);
  virtual ~ChartDatedDatum(void);
  void Add( const CBar &bar );
  void Add( const Trade &trade );
  void AddTrade( const Trade &trade ) { Add( trade ); };
  void SetWindowWidthSeconds( long seconds );
  long GetWindowWidthSeconds( void ) { return m_pWindowBars -> GetSlidingWindowSeconds(); };
  void SetBarFactoryWidthSeconds( long seconds ) { m_factory.SetBarWidth( seconds ); };
  long GetBarFactoryWidthSeconds( void ) { return m_factory.GetBarWidth(); };
  void UpdateChart( void );
  void ClearChart( void );
  void setMajorTickInc( double inc ) { m_majorTickInc = inc; };
  void setMinorTickInc( double inc ) { m_minorTickInc = inc; };

protected:
  FinanceChart *chart;
  //long m_nWindowWidthSeconds;
  SlidingWindowBars *m_pWindowBars;  // this list of bars are the ones visible in the chart
  BarFactory m_factory;
  void HandleOnNewBar( const CBar &bar );
  void HandleOnBarUpdated( const CBar &bar );
  double m_majorTickInc, m_minorTickInc; 

	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
};
