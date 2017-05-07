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

  typedef std::vector<double> vDouble_t;
  typedef vDouble_t::size_type size_type;
  
  ChartEntryPrice( void );
  virtual ~ChartEntryPrice( void );

  void Append( const ou::tf::Price& );
  void Append( const boost::posix_time::ptime &dt, double price );
  size_type Size( void ) const { return m_vDouble.size(); }
  virtual void Clear( void );
  virtual void Reserve( size_type );
  
  void ClearQueue( void );

  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes* pAttributes );

protected:
  
  void Pop( const ou::tf::Price& );
  
  DoubleArray GetPrices( void ) const {  // prices which are visible in viewport
    return DoubleArray( &m_vDouble[ m_ixStart ], m_nElements );
  }

private:
  
  vDouble_t m_vDouble;
  
  ou::tf::Queue<ou::tf::Price> m_queue;

};

} // namespace ou


#endif /* CHARTENTRYPRICE_H */

