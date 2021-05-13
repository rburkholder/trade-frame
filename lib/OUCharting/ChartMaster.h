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
#include <functional>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "ChartDataView.h"

namespace ou { // One Unified

class ChartMaster  {
public:
  ChartMaster();
  ChartMaster( unsigned int width, unsigned int height );
  virtual ~ChartMaster(void);

  void SetChartDimensions( unsigned int width, unsigned int height);
  //void SetChartTitle( std::string sChartTitle ) { m_sChartTitle = sChartTitle; };
  void SetChartDataView( ChartDataView* pcdv );
  ChartDataView* GetChartDataView( void ) const { return m_pCdv; };

  // can use not_a_date_time for one, the other, or both
  //void SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ); // deprecated - use ChartDataView instead
  void SetBarWidth( boost::posix_time::time_duration tdBarWidth );

  bool GetChartDataViewChanged( void );
  void DrawChart( bool bViewPortChanged = false );  // recalc viewport zoom effects when true
  bool isCreated( void ) const { return m_bCreated; };

  using fOnDrawChart_t = std::function<void( const MemBlock& )>;
  void SetOnDrawChart( fOnDrawChart_t&& function ) {
    m_fOnDrawChart = std::move( function );
  }
protected:

  //std::string m_sChartTitle;  // data view has this information
  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;
  ChartDataView* m_pCdv;

  double m_dblViewPortXBegin;  // initial data time stamp - deprecated, to be removed
  double m_dblViewPortXEnd;  // last data time stamp - deprecated, to be remvoed

  boost::posix_time::time_duration m_tdBarWidth;

  fOnDrawChart_t m_fOnDrawChart;

  bool m_bCreated;


private:

  void Initialize( void );

};

} // namespace ou
