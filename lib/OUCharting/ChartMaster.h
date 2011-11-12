/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <string>

#include "ChartDataView.h"

namespace ou { // One Unified

class ChartMaster  {
public:
  ChartMaster(void);
  ChartMaster( unsigned int width, unsigned int height );
  virtual ~ChartMaster(void);
  void SetChartDimensions( unsigned int width, unsigned int height);
  //void SetChartTitle( std::string sChartTitle ) { m_sChartTitle = sChartTitle; };
  void SetChartDataView( ChartDataView *pcdv ) { m_pCdv = pcdv; if ( NULL != pcdv ) m_pCdv->SetChanged(); };
  ChartDataView *GetChartDataView( void ) { return m_pCdv; };
  double GetXMin( void ) { return m_dblXMin; };
  double GetXMax( void ) { return m_dblXMax; };
  bool GetChartDataViewChanged( void ) { return ( NULL == m_pCdv ) ? false : m_pCdv->GetChanged(); };  // flag is reset during call
  void DrawChart( bool bViewPortChanged = false );  // recalc viewport zoom effects when true
  bool isCreated( void ) { return m_bCreated; };
protected:

  //std::string m_sChartTitle;  // data view has this information
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  ChartDataView *m_pCdv;

  double m_dblMinDuration;  // minimum viewport width in seconds
  double m_dblCurDuration;  // current viewport width in seconds

  double m_dblXMin;  // initial data time stamp
  double m_dblXMax;  // last data time stamp

  bool m_bCreated;

  void Initialize( void );

private:

};

} // namespace ou
