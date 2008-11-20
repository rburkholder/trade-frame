#pragma once

#include <string>
#include "ChartDirector\chartviewer.h"
#include "ChartDataView.h"

class CChartMaster :  public CChartViewer {
public:
  CChartMaster(void);
  CChartMaster( unsigned int width, unsigned int height );
  virtual ~CChartMaster(void);
  void SetChartDimensions( unsigned int width, unsigned int height);
  //void SetChartTitle( std::string sChartTitle ) { m_sChartTitle = sChartTitle; };
  void SetChartDataView( CChartDataView *pcdv ) { m_pCdv = pcdv; if ( NULL != pcdv ) m_pCdv->SetChanged(); };
  CChartDataView *GetChartDataView( void ) { return m_pCdv; };
  double GetXMin( void ) { return m_dblXMin; };
  double GetXMax( void ) { return m_dblXMax; };
  bool GetChartDataViewChanged( void ) { return ( NULL == m_pCdv ) ? false : m_pCdv->GetChanged(); };  // flag is reset during call
  void DrawChart( void );
protected:
  //std::string m_sChartTitle;  // data view has this information
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  CChartDataView *m_pCdv;

  double m_dblMinDuration;  // minimum viewport width in seconds
  double m_dblCurDuration;  // current viewport width in seconds

  double m_dblXMin;  // initial data time stamp
  double m_dblXMax;  // last data time stamp

  void Initialize( void );
private:
  virtual afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
  virtual afx_msg void OnDestroy( );

	DECLARE_MESSAGE_MAP()
};
