#pragma once

#include <vector>

#include "ChartControls.h"
#include "ChartRealTimeTreeView.h"
#include "ChartViewPort.h"

// CChartingContainer contains the view, tree, and controls for the mechanics of 
//  selecting and viewing a chart.

class CChartingContainer {
public:
  explicit CChartingContainer(void);
  ~CChartingContainer(void);
  CChartRealTimeTreeView *GetTreeView( void ) { return &m_TreeView; };
protected:
  void CreateNewViewPort( void );

  void HandleCreateNewViewPort( CChartControls *pControls );

  CChartControls m_ChartControls;  // not sure if I'll end up using this, meant to be used for mouse gestures on the chart and such
  CChartRealTimeTreeView m_TreeView; 

  size_t m_ixActiveViewPort;
  std::vector<CChartViewPort *> m_vViewPorts;
private:
};
