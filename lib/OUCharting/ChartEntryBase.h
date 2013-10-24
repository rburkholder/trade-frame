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

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/shared_ptr.hpp>

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
  ChartEntryBase( unsigned int nSize );
  virtual ~ChartEntryBase( void );

  ou::Colour::enumColour GetColour( void ) const { return m_eColour; };
  virtual void SetColour( ou::Colour::enumColour colour ) { m_eColour = colour; };
  void SetName( std::string name ) { m_sName = name; };
  const std::string &GetName( void ) const { return m_sName; };
  void Append( double price );
  virtual size_t Size( void ) const { return m_vPrice.size(); };

  virtual void AddEntryToChart( XYChart *pXY, structChartAttributes* pAttributes ) const {};

  virtual void Clear( void );

protected:

  size_type m_ixStart; // starting point into viewport
  size_type m_nElements;  // number of elements in the viewport

  ou::Colour::enumColour m_eColour;
  std::string m_sName;

  std::vector<double> m_vPrice;  // full vector of raw prices

  DoubleArray GetPrices( void ) const {  // prices which are visible in viewport
//    vdouble_t::const_iterator iter = m_vPrice.begin();
//    return DoubleArray( &(*iter), static_cast<int>( m_vPrice.size() ) );
    return DoubleArray( &m_vPrice[ m_ixStart ], m_nElements );
  }

  virtual void Reserve( unsigned int );

private:
};

class ChartEntryBaseWithTime : public ChartEntryBase { // maintains chart information for a set of price@datetime points
public:

  ChartEntryBaseWithTime( void );
  ChartEntryBaseWithTime( unsigned int nSize );
  virtual ~ChartEntryBaseWithTime( void );

  void SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd );

  void Append( const boost::posix_time::ptime &dt, double price );
  void Append( const boost::posix_time::ptime &dt );

  virtual void Clear( void );

protected:

  boost::posix_time::ptime m_dtViewPortBegin;
  boost::posix_time::ptime m_dtViewPortEnd;

  typedef std::vector<boost::posix_time::ptime> vDateTime_t;

  vDateTime_t m_vDateTime;
  vdouble_t m_vChartTime;  // used by ChartDir, double version of m_vDateTime

  // need to get to top of call hierarchy and only call when m_nElements is non-zero
  DoubleArray GetDateTimes( void ) const {
    return DoubleArray( &m_vChartTime[ m_ixStart ], m_nElements );
  }

  virtual void Reserve( unsigned int );

private:
};

} // namespace ou
