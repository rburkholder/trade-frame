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
#include <memory>

#ifdef RGB
#undef RGB
// windows COLORREF is backwards from what ChartDir is expecting
#endif
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#include <OUCommon/Colour.h>

#include <chartdir/chartdir.h>

#include <TFTimeSeries/DoubleBuffer.h>

namespace ou { // One Unified

class ChartEntryBase {  // maintains chart information for a set of prices
public:

  using pChartEntryBase_t = std::shared_ptr<ChartEntryBase>;
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

  //bool m_bUseThreadSafety;
  //static const unsigned int lockfreesize = 4096;

private:
};

// **********

class ChartEntryTime : public ChartEntryBase { // maintains chart information for a set of price@datetime points
public:

  using vDateTime_t = std::vector<boost::posix_time::ptime>;
  using size_type   =  vDateTime_t::size_type;

  struct range_t {
    boost::posix_time::ptime dtBegin;
    boost::posix_time::ptime dtEnd;
    range_t()
    : dtBegin( boost::posix_time::not_a_date_time ), dtEnd( boost::posix_time::not_a_date_time ) {}
    range_t( boost::posix_time::ptime dtBegin_, boost::posix_time::ptime dtEnd_ )
    : dtBegin( dtBegin_ ), dtEnd( dtEnd_ ) {}
    inline bool HasBegin() const { return boost::posix_time::not_a_date_time != dtBegin; }
    inline bool HasEnd() const { return boost::posix_time::not_a_date_time != dtEnd; }
    inline bool HasBoth() const { return HasBegin() && HasEnd(); }
  };

  ChartEntryTime( void );
  virtual ~ChartEntryTime( void );

  void Append( boost::posix_time::ptime dt ); // background append

  virtual void Clear( void );
  virtual void Reserve( size_type );

  virtual void ClearQueue( void );

  void SetViewPort( const range_t& );
  void SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd );

  range_t GetExtents() const;
  boost::posix_time::ptime GetExtentBegin() const;
  boost::posix_time::ptime GetExtentEnd() const;

protected:

  range_t m_rangeViewPort;

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

  using vChartTime_t = std::vector<double> ;

  ou::tf::Queue<boost::posix_time::ptime> m_queue;

//  struct TimeDouble_t {
//    boost::posix_time::ptime m_dt;
//    double m_price;
//    TimeDouble_t( void ): m_price( 0.0 ) {};
//    TimeDouble_t( boost::posix_time::ptime dt, double price ): m_dt( dt ), m_price( price ) {};
//  };

  //bufferedDateTime_t m_bufferedDateTime;
  vDateTime_t m_vDateTime;
  vChartTime_t m_vChartTime;  // used by ChartDir, double version of m_vDateTime

};

} // namespace ou
