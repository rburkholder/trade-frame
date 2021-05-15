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
class DrawArea;

namespace ou { // One Unified

class ChartDataView;

class ChartMaster  {
public:
  ChartMaster();
  ChartMaster( unsigned int width, unsigned int height );
  virtual ~ChartMaster();

  void SetChartDataView( ChartDataView* pcdv );
  ChartDataView* GetChartDataView( void ) const { return m_pCdv; };
  bool GetChartDataViewChanged();

  void SetBarWidth( boost::posix_time::time_duration tdBarWidth );

  void SetChartDimensions( unsigned int width, unsigned int height);
  void DrawChart();

  using fOnDrawChart_t = std::function<void( const MemBlock& )>;
  void SetOnDrawChart( fOnDrawChart_t&& function ) {
    m_fOnDrawChart = std::move( function );
  }

  void CrossHairPosition( int x, int y );
  void CrossHairDraw( bool );

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
  DrawArea* m_pDA;

  bool m_bHasData;

  int m_intCrossHairX, m_intCrossHairY;
  bool m_bCrossHair;

  void Initialize();

  void ChartStructure();
  void ChartData( XYChart* );

  void DrawDynamicLayer();

  void RenderChart();

  bool ResetDynamicLayer();

};

} // namespace ou
