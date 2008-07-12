#pragma once

#include <vector>

#include "ChartControls.h"
#include "ChartRealTimeTreeView.h"
#include "ChartViewPort.h"

class CChartingContainer {
public:
  CChartingContainer(void);
  ~CChartingContainer(void);
protected:
  CChartControls m_ChartControls;  // not sure if I'll end up using this, meant to be used for mouse gestures on the chart and such
  CChartRealTimeTreeView *m_pTreeView; 
  std::vector<CChartViewPort *> m_vViewPorts;
  void HandleCreateNewViewPort( CChartControls *pControls );
  void CreateNewViewPort( void );
  size_t m_ixActiveViewPort;
private:
};
