/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <TFTimeSeries/DatedDatum.h>

#include "ChartEntryBase.h"

namespace ou { // One Unified

class ChartEntryBars :
  public ChartEntryTime {
public:
  ChartEntryBars();
  virtual ~ChartEntryBars();

  virtual void Reserve( size_type ) override;
  void AppendBar( const ou::tf::Bar& bar ); // uses thread crossing buffer
  virtual bool AddEntryToChart( XYChart *pXY, structChartAttributes& ) override;
  virtual void Clear() override;

protected:
  DoubleArray GetOpen() const {
    // vdouble_t::const_iterator iter = m_vOpen.begin();
    // return DoubleArray( &(*iter), static_cast<int>( m_vOpen.size() ) );
    return DoubleArray( &m_vOpen[ IxStart() ], CntElements() );
  }
  DoubleArray GetHigh() const {
    //vdouble_t::const_iterator iter = m_vHigh.begin();
    //return DoubleArray( &(*iter), static_cast<int>( m_vHigh.size() ) );
    return DoubleArray( &m_vHigh[ IxStart() ], CntElements() );
  }
  DoubleArray GetLow() const {
    //vdouble_t::const_iterator iter = m_vLow.begin();
    //return DoubleArray( &(*iter), static_cast<int>( m_vLow.size() ) );
    return DoubleArray( &m_vLow[ IxStart() ], CntElements() );
  }
  DoubleArray GetClose() const {
    //vdouble_t::const_iterator iter = m_vClose.begin();
    //return DoubleArray( &(*iter), static_cast<int>( m_vClose.size() ) );
    return DoubleArray( &m_vClose[ IxStart() ], CntElements() );
  }

  virtual void ClearQueue() override;

private:
  //boost::lockfree::spsc_queue<ou::tf::Bar, boost::lockfree::capacity<lockfreesize> > m_lfBar;

  std::vector<double> m_vOpen;
  std::vector<double> m_vHigh;
  std::vector<double> m_vLow;
  std::vector<double> m_vClose;

  ou::tf::Queue<ou::tf::Bar> m_queueBars;

  void Pop( const ou::tf::Bar& bar );
};

} // namespace ou
