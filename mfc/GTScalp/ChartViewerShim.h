#pragma once

#include <ChartDirector\ChartViewer.h>
#include <string>
using namespace std;


// used just above chartdi.h to provide access to common settings

class CChartViewerShim :
  public CChartViewer {
public:
  CChartViewerShim(void);
  virtual ~CChartViewerShim(void);

  void SetUpdateChart( bool b ) { m_bUpdateChart = b; };
  bool GetUpdateChart( void ) { return m_bUpdateChart; };
  void SetChartDimensions( unsigned int x, unsigned int y);
  void SetTitle( string sChartTitle ) { m_sChartTitle = sChartTitle; };

protected:
  bool m_bUpdateChart; 
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  string m_sChartTitle;

	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
};
