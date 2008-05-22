#pragma once
#include "ChartDirector\chartviewer.h"

#include "ChartRealTimeModel.h"
#include "GeneratePeriodicRefresh.h"

#include <string>

class CChartRealTimeView :
  public CChartViewer {
public:
  CChartRealTimeView();
  virtual ~CChartRealTimeView(void);
  void SetUpdateChart( bool b ) { m_bUpdateChart = b; };
  bool GetUpdateChart( void ) { return m_bUpdateChart; };
  void SetChartDimensions( unsigned int x, unsigned int y);
  void SetChartTitle( std::string sChartTitle ) { m_sChartTitle = sChartTitle; };
  void HandleModelChanged(const CChartRealTimeModel &model);
protected:
  bool m_bUpdateChart; 
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  std::string m_sChartTitle;
  CGeneratePeriodicRefresh m_refresh;
  void HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg );
	DECLARE_MESSAGE_MAP()
private:
};
