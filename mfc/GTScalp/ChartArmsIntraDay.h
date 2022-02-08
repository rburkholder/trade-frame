#pragma once
//#include "chartviewer.h"
#include "ChartViewerShim.h"
#include "ChartDirector\FinanceChart.h"
#include "ZigZag.h"

class CChartArmsIntraDay : public CChartViewerShim {
public:
  CChartArmsIntraDay(void);
  virtual ~CChartArmsIntraDay(void);

  void ProcessIndu( const Trade &trade );
  void ProcessTrin( const Trade &trade );
  void ProcessTick( const Trade &trade );

protected:
  CZigZag *pzzIndu, *pzzTrin;

  XYChart *pChart;
  LineLayer *pLLIndu;
  LineLayer *pLLTrin;
  LineLayer *pLLTick;
  vector<double> m_vdblIndu, m_vdblTrin;
  double m_dblInduPt1, m_dblTrinPt1;
  double m_dblIndu, m_dblTrin;

  bool m_bInduChanged, m_bTrinChanged;

  void ZZInduChanged( CZigZag *, ptime, double, CZigZag::EDirection );
  void ZZTrinChanged( CZigZag *, ptime, double, CZigZag::EDirection );

  void DrawChart( void );

  bool m_bFirstInduFound;
  double m_dblFirstIndu;

private:
};
