#pragma once
#include "ChartDirector\chartviewer.h"
#include "ChartDirector\FinanceChart.h"
#include "GeneratePeriodicRefresh.h"

#include "ChartRealTimeModel.h"

#include <string>

class CChartRealTimeView :
  public CChartViewer {
public:
  CChartRealTimeView( CChartRealTimeModel *pModel );
  virtual ~CChartRealTimeView();
  //void SetModel( CChartRealTimeModel *pModel ) { m_pModel = pModel; };
  void SetUpdateChart( bool b ) { m_bUpdateChart = b; };
  bool GetUpdateChart( void ) { return m_bUpdateChart; };
  void SetChartDimensions( unsigned int width, unsigned int height);
  void SetChartTitle( std::string sChartTitle ) { m_sChartTitle = sChartTitle; };
  void HandleBarCompleted(CChartRealTimeModel *model);
protected:
  CChartRealTimeModel *m_pModel;
  bool m_bUpdateChart; 
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  bool m_bModelChanged;
  std::string m_sChartTitle;
  CGeneratePeriodicRefresh m_refresh;
  void HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg );

	DECLARE_MESSAGE_MAP()
private:
};
