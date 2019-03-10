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
//   will be used by the charting application for determining if
//   it will be calculating the DoubleArray parameter for the charting library

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
//#include <boost/lockfree/spsc_queue.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef RGB
#undef RGB
// windows COLORREF is backwards from what ChartDir is expecting
#endif
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#include <OUCommon/Colour.h>

#if defined(__x86_64__)
#include "ChartDirector64/chartdir.h"
#elif defined(_M_X64)
#include "ChartDirector64/chartdir.h"
#else
#include "ChartDirector/chartdir.h"
#endif

#include <TFTimeSeries/DoubleBuffer.h>

namespace ou { // One Unified

class ChartEntryBase {  // maintains chart information for a set of prices
public:

  typedef boost::shared_ptr<ChartEntryBase> pChartEntryBase_t;
  //typedef std::vector<double> vdouble_t;
  //typedef vdouble_t::size_type size_type;
  //typedef ou::tf::DoubleBuffer<double> bufferedPrice_t;
  //typedef bufferedPrice_t::size_type size_type;

  struct structChartAttributes {
    double dblXMin;
    double dblXMax;
    double dblYMin;
    double dblYMax;
    structChartAttributes( void ) : dblXMin( 0 ), dblXMax( 0 ), dblYMin( 0 ), dblYMax( 0 ) {};
  };

  ChartEntryBase( void );
  //ChartEntryBase( size_type nSize );
  virtual ~ChartEntryBase( void );

  ou::Colour::enumColour GetColour( void ) const { return m_eColour; };
  virtual void SetColour( ou::Colour::enumColour colour ) { m_eColour = colour; };

  void SetName( const std::string& name ) { m_sName = name; };
  const std::string& GetName( void ) const { return m_sName; };

  // now threadsafe by default
  void SetThreadSafe( bool bThreadSafe ) { m_bUseThreadSafety = bThreadSafe; };
  bool GetThreadSafe( void ) const  { return m_bUseThreadSafety; }

  //void Append( double price );
  //size_type Size( void ) { return m_bufferedPrice.Size(); } // an expensive call with sync/mutex
  //void Sync( void ); // should call other registered distribution functions
  //void Clear( void );  // should call other registered distribution functions
  //void Reserve( size_type ); // should call other registered distribution functions

  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes* pAttributes ) { return false; }

  virtual void Clear( void ) {}

protected:

  size_t m_ixStart; // starting point into viewport
  int m_nElements;  // number of elements in the viewport

  ou::Colour::enumColour m_eColour;
  std::string m_sName;

  bool m_bUseThreadSafety;
  //static const unsigned int lockfreesize = 4096;

  //vdouble_t m_vPrice;  // full vector of raw prices
  //bufferedPrice_t m_bufferedPrice;

  //DoubleArray GetPrices( void ) const {  // prices which are visible in viewport
    //return DoubleArray( &m_vPrice[ m_ixStart ], m_nElements );
  //  return DoubleArray( m_bufferedPrice[ m_ixStart ], m_nElements );
  //}

private:
};

// **********

class ChartEntryTime : public ChartEntryBase { // maintains chart information for a set of price@datetime points
public:

  typedef std::vector<boost::posix_time::ptime> vDateTime_t;
  typedef vDateTime_t::size_type size_type;

  ChartEntryTime( void );
  //ChartEntryTime( size_type nSize );
  virtual ~ChartEntryTime( void );

  void Append( boost::posix_time::ptime dt ); // background append

  virtual void Clear( void );
  virtual void Reserve( size_type );

  virtual void ClearQueue( void );

  void SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd );

protected:

  typedef std::vector<double> vChartTime_t;

  boost::posix_time::ptime m_dtViewPortBegin;
  boost::posix_time::ptime m_dtViewPortEnd;

  void AppendFg( boost::posix_time::ptime dt ); // foreground append

  // need to get to top of call hierarchy and only call when m_nElements is non-zero
  DoubleArray GetDateTimes( void ) const {
    // 2017/05/06 this should be cleaned up:
//    if ( ( 2 <= m_vChartTime.size() ) && ( 2 <= m_nElements ) ) {
//      double diff = m_vChartTime[ m_ixStart + m_nElements - 1 ] - m_vChartTime[ m_ixStart ];
//      if ( 610.0 < diff ) {
//        static double change( 0 );
//        change = diff;
//      }
//    }
    return DoubleArray( &m_vChartTime[ m_ixStart ], m_nElements );
  }

  size_type Size( void ) const { return m_vDateTime.size(); }

private:

  ou::tf::Queue<boost::posix_time::ptime> m_queue;

  struct TimeDouble_t {
    boost::posix_time::ptime m_dt;
    double m_price;
    TimeDouble_t( void ): m_price( 0.0 ) {};
    TimeDouble_t( boost::posix_time::ptime dt, double price ): m_dt( dt ), m_price( price ) {};
  };

  //bufferedDateTime_t m_bufferedDateTime;
  vDateTime_t m_vDateTime;
  vChartTime_t m_vChartTime;  // used by ChartDir, double version of m_vDateTime

};

} // namespace ou
