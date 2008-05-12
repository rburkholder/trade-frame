#pragma once
#include "ChartViewerShim.h"
#include "SlidingWindow.h"
#include "DatedDatum.h"
#include "BarFactory.h"
#include "FinanceChart.h"

class CChartDatedDatum : public CChartViewerShim {
public:
  CChartDatedDatum(void);
  virtual ~CChartDatedDatum(void);
  void Add( const CBar &bar );
  void Add( const CTrade &trade );
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
  CSlidingWindowBars *m_pWindowBars;
  CBarFactory m_factory;
  void HandleOnNewBar( const CBar &bar );
  void HandleOnBarUpdated( const CBar &bar );
  double m_majorTickInc, m_minorTickInc; 
private:
};
