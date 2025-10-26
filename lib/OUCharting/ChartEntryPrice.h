/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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

/*
 * File:   ChartEntryPrice.h
 * Author: rpb
 *
 * Created on May 6, 2017, 7:03 PM
 */

#ifndef CHARTENTRYPRICE_H
#define CHARTENTRYPRICE_H

#include <TFTimeSeries/DatedDatum.h>
#include <TFTimeSeries/DoubleBuffer.h>

#include "ChartEntryBase.h"

namespace ou { // One Unified

class ChartEntryPrice: public ChartEntryTime {  // maintains chart information for a set of prices
public:

  using vDouble_t = std::vector<double>;
  using size_type = vDouble_t::size_type;

  ChartEntryPrice();
  ChartEntryPrice( ChartEntryPrice&& );
  virtual ~ChartEntryPrice();

  void Append( const ou::tf::Price& );
  void Append( const boost::posix_time::ptime &dt, double price );
  size_type Size() const { return m_vDouble.size(); }

  virtual void Reserve( size_type ) override;
  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes& ) override;

  virtual void Clear() override;

protected:

  void Pop( const ou::tf::Price& ); // used by ChartEntryShape

  DoubleArray GetPrices() const {  // prices which are visible in viewport
    return DoubleArray( &m_vDouble[ IxStart() ], CntElements() );
  }

  virtual void ClearQueue() override;

private:

  vDouble_t m_vDouble;

  ou::tf::Queue<ou::tf::Price> m_queue;

};

} // namespace ou

#endif /* CHARTENTRYPRICE_H */
