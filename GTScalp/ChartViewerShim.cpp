#include "StdAfx.h"
#include "ChartViewerShim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CChartViewerShim::CChartViewerShim(void) :CChartViewer() {
  m_nChartWidth = 600;
  m_nChartHeight = 180;
}

CChartViewerShim::~CChartViewerShim(void) {
}

void CChartViewerShim::SetChartDimensions(unsigned int x, unsigned int y) {
  m_nChartWidth = x;
  m_nChartHeight = y;
}
