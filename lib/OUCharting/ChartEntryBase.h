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

#include <boost/date_time/posix_time/posix_time_types.hpp>

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

  struct structChartAttributes {

    //double dblYMax;
    //double dblYMin;

    double dblXMax;
    double dblXMin;

    using fPostLayout_t = std::function<void()>;
    fPostLayout_t fPostLayout;

    structChartAttributes()
    : dblXMin{}, dblXMax{}
    //, dblYMin {}, dblYMax {}
    , fPostLayout( nullptr )
    {}
  };

  ChartEntryBase();
  ChartEntryBase( const ChartEntryBase& );
  ChartEntryBase( ChartEntryBase&& );
  virtual ~ChartEntryBase();

  ou::Colour::EColour GetColour() const { return m_eColour; };
  virtual void SetColour( ou::Colour::EColour colour ) { m_eColour = colour; };

  void SetName( const std::string& name ) { m_sName = name; };
  const std::string& GetName() const { return m_sName; };

  //void Sync( void ); // should call other registered distribution functions
  //void Clear( void );  // should call other registered distribution functions
  //void Reserve( size_type ); // should call other registered distribution functions

  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes& ) { return false; }

  virtual void Clear() {
    m_ixStart = 0;
    m_nElements = 0;
  }

protected:

  ou::Colour::EColour m_eColour;
  std::string m_sName;

  size_t IxStart() const { return m_ixStart; }
  int CntElements() const { return m_nElements; }

  void SetIxStart( size_t n ) { m_ixStart = n; }
  void SetCntElements( int n ) { m_nElements = n; }

  void IncCntElements() { m_nElements++; }

  //bool m_bUseThreadSafety;
  //static const unsigned int lockfreesize = 4096;

private:
  size_t m_ixStart; // starting point into viewport
  int m_nElements;  // number of elements in the viewport
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
    range_t( const range_t& rhs )
    : dtBegin( rhs.dtBegin ), dtEnd( rhs.dtEnd ) {}
    range_t( const range_t&& rhs )
    : dtBegin( rhs.dtBegin ), dtEnd( rhs.dtEnd ) {}
    const range_t& operator=( const range_t& rhs ) {
      if ( &rhs != this ) {
        dtBegin = rhs.dtBegin;
        dtEnd = rhs.dtEnd;
      }
      return *this;
    }
    inline bool HasBegin() const { return boost::posix_time::not_a_date_time != dtBegin; }
    inline bool HasEnd() const { return boost::posix_time::not_a_date_time != dtEnd; }
    inline bool HasBoth() const { return HasBegin() && HasEnd(); }
  };

  ChartEntryTime();
  ChartEntryTime( ChartEntryTime&& );
  virtual ~ChartEntryTime();

  void Append( boost::posix_time::ptime dt ); // background append

  virtual void Reserve( size_type );

  void SetViewPort( const range_t& );

  range_t GetExtents() const;
  boost::posix_time::ptime GetExtentBegin() const;
  boost::posix_time::ptime GetExtentEnd() const;

  static double Convert( boost::posix_time::ptime ); // convert to chart datetime format

  virtual void Clear() override;

protected:

  range_t m_rangeViewPort;

  void AppendFg( boost::posix_time::ptime dt ); // foreground append

  // need to get to top of call hierarchy and only call when m_nElements is non-zero
  DoubleArray GetDateTimes() const {
    // 2017/05/06 this should be cleaned up:
//    if ( ( 2 <= m_vChartTime.size() ) && ( 2 <= m_nElements ) ) {
//      double diff = m_vChartTime[ m_ixStart + m_nElements - 1 ] - m_vChartTime[ m_ixStart ];
//      if ( 610.0 < diff ) {
//        static double change( 0 );
//        change = diff;
//      }
//    }
    return DoubleArray( &m_vChartTime[ IxStart() ], CntElements() );
  }

  size_type Size() const { return m_vDateTime.size(); }

  virtual void ClearQueue();

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
