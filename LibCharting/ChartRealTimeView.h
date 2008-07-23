#pragma once
#include "ChartDirector\chartviewer.h"
#include "ChartDirector\FinanceChart.h"
#include "GeneratePeriodicRefresh.h"

#include "ChartRealTimeModel.h"

#include <string>

// code from here will morph for use by CChartViewPort

class CChartRealTimeView :
  public CChartViewer {
public:
  CChartRealTimeView( CChartRealTimeModel *pModel );
  virtual ~CChartRealTimeView();
  //void SetModel( CChartRealTimeModel *pModel ) { m_pModel = pModel; };
  void SetChartDimensions( unsigned int width, unsigned int height);
  void SetChartTitle( std::string sChartTitle ) { m_sChartTitle = sChartTitle; };
  void HandleBarCompleted(CChartRealTimeModel *model);
protected:
  CChartRealTimeModel *m_pModel;
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  bool m_bModelChanged;
  std::string m_sChartTitle;
  CGeneratePeriodicRefresh m_refresh;
  void HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg );

	DECLARE_MESSAGE_MAP()
private:
};
