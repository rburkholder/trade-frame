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

// 
// Tools are meant for drawing in world coordinates:  
//   at a price level, or at a price/time intersection
//

// inheriting classes need to be aware of how Size is calculated, as it 
//   will be used by the charting application for determinig if 
//   it will be calculating the DoubleArray parameter for the charting library

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef RGB
#undef RGB
// windows COLORREF is backwards from what ChartDir is expecting
#endif
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#include <OUCommon/Colour.h>

#include "ChartDirector64/chartdir.h"

namespace ou { // One Unified

class ChartEntryBase {  // maintains chart information for a set of prices
public:

  typedef boost::shared_ptr<ChartEntryBase> pChartEntryBase_t;
  typedef std::vector<double> vdouble_t;
  typedef vdouble_t::size_type size_type;

  struct structChartAttributes {
    double dblXMin;
    double dblXMax;
    double dblYMin;
    double dblYMax;
    structChartAttributes( void ) : dblXMin( 0 ), dblXMax( 0 ), dblYMin( 0 ), dblYMax( 0 ) {};
  };

  ChartEntryBase( void );
  ChartEntryBase( size_type nSize );
  virtual ~ChartEntryBase( void );

  ou::Colour::enumColour GetColour( void ) const { return m_eColour; };
  virtual void SetColour( ou::Colour::enumColour colour ) { m_eColour = colour; };

  void SetName( const std::string& name ) { m_sName = name; };
  const std::string& GetName( void ) const { return m_sName; };

  void Append( double price );
  virtual size_type Size( void ) const { return m_vPrice.size(); };
  virtual void Clear( void );
  virtual void Reserve( size_type );

  virtual void AddEntryToChart( XYChart* pXY, structChartAttributes* pAttributes ) {}; // requires use of lockfree to add values

protected:

  size_type m_ixStart; // starting point into viewport
  int m_nElements;  // number of elements in the viewport

  ou::Colour::enumColour m_eColour;
  std::string m_sName;

  vdouble_t m_vPrice;  // full vector of raw prices

  DoubleArray GetPrices( void ) const {  // prices which are visible in viewport
    return DoubleArray( &m_vPrice[ m_ixStart ], m_nElements );
  }


private:
};

class ChartEntryBaseWithTime : public ChartEntryBase { // maintains chart information for a set of price@datetime points
public:

  typedef ChartEntryBase::size_type size_type;

  ChartEntryBaseWithTime( void );
  ChartEntryBaseWithTime( size_type nSize );
  virtual ~ChartEntryBaseWithTime( void );

  void Append( boost::posix_time::ptime dt, double price );  // protected with lockfree

  virtual void Clear( void );
  virtual void Reserve( size_type );

  void SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd );

protected:

  typedef std::vector<boost::posix_time::ptime> vDateTime_t;

  static const unsigned int lockfreesize = 4096;

  boost::posix_time::ptime m_dtViewPortBegin;
  boost::posix_time::ptime m_dtViewPortEnd;

  vDateTime_t m_vDateTime;
  vdouble_t m_vChartTime;  // used by ChartDir, double version of m_vDateTime

  void Append( boost::posix_time::ptime dt );  // needs to be protected with lockfree 
  void ClearQueue( void );  // used in the charting thread to clear lock free queue

  // need to get to top of call hierarchy and only call when m_nElements is non-zero
  DoubleArray GetDateTimes( void ) const {
    return DoubleArray( &m_vChartTime[ m_ixStart ], m_nElements );
  }

private:

  struct TimeDouble_t {
    boost::posix_time::ptime m_dt;
    double m_price;
    TimeDouble_t( void ): m_price( 0.0 ) {};
    TimeDouble_t( boost::posix_time::ptime dt, double price ): m_dt( dt ), m_price( price ) {};
  };

  boost::lockfree::spsc_queue<TimeDouble_t, boost::lockfree::capacity<lockfreesize> > m_lfTimeDouble;


};

} // namespace ou
