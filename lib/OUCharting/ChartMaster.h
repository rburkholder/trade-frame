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

#include <vector>
#include <string>
#include <functional>

#include <boost/date_time/posix_time/posix_time.hpp>

class MultiChart;
class XYChart;
class MemBlock;

namespace ou { // One Unified

class ChartDataView;

class ChartMaster  {
public:
  ChartMaster();
  ChartMaster( unsigned int width, unsigned int height );
  virtual ~ChartMaster();

  void SetChartDimensions( unsigned int width, unsigned int height);
  void SetChartDataView( ChartDataView* pcdv );
  ChartDataView* GetChartDataView( void ) const { return m_pCdv; };

  void SetBarWidth( boost::posix_time::time_duration tdBarWidth );

  bool GetChartDataViewChanged();
  void DrawChart( bool bViewPortChanged = false );  // recalc viewport zoom effects when true
  //bool isCreated( void ) const { return m_bCreated; };

  using fOnDrawChart_t = std::function<void( const MemBlock& )>;
  void SetOnDrawChart( fOnDrawChart_t&& function ) {
    m_fOnDrawChart = std::move( function );
  }
protected:

  unsigned int m_nChartWidth;
  unsigned int m_nChartHeight;

  boost::posix_time::time_duration m_tdBarWidth;

  fOnDrawChart_t m_fOnDrawChart;

private:

  ChartDataView* m_pCdv;
  std::unique_ptr<MultiChart> m_pChart;

  using pXYChart_t = std::unique_ptr<XYChart>;
  using vSubChart_t = std::vector<pXYChart_t>;

  vSubChart_t m_vSubCharts;

  XYChart* m_pXY0;

  bool m_bHasData;

  void Initialize();

  void ChartStructure();
  void ChartData( XYChart* );

};

} // namespace ou
