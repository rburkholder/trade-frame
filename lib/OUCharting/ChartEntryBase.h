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
using namespace boost::posix_time;
using namespace boost::gregorian;

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

  ChartEntryBase( void );
  ChartEntryBase( unsigned int nSize );
  virtual ~ChartEntryBase( void );

  typedef boost::shared_ptr<ChartEntryBase> pChartEntryBase_t;

  ou::Colour::enumColour GetColour( void ) const { return m_eColour; };
  virtual void SetColour( ou::Colour::enumColour colour ) { m_eColour = colour; };
  void SetName( std::string name ) { m_sName = name; };
  const std::string &GetName( void ) const { return m_sName; };
  void Add( double price );
  typedef std::vector<double> vdouble_t;
  virtual size_t Size( void ) const { return m_vPrice.size(); };

  struct structChartAttributes {
    double dblXMin;
    double dblXMax;
    double dblYMin;
    double dblYMax;
    structChartAttributes( void ) : dblXMin( 0 ), dblXMax( 0 ), dblYMin( 0 ), dblYMax( 0 ) {};
  };

  virtual void AddDataToChart( XYChart *pXY, structChartAttributes* pAttributes ) const {};

protected:

  virtual void Reserve( unsigned int );
  std::vector<double> m_vPrice;
  ou::Colour::enumColour m_eColour;
  std::string m_sName;
  DoubleArray GetPrices( void ) const {
    vdouble_t::const_iterator iter = m_vPrice.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vPrice.size() ) );
  }

private:
};

class ChartEntryBaseWithTime : public ChartEntryBase { // maintains chart information for a set of price@datetime points
public:

  ChartEntryBaseWithTime( void );
  ChartEntryBaseWithTime( unsigned int nSize );
  virtual ~ChartEntryBaseWithTime( void );

  void Add( const ptime &dt, double price );
  void Add( const ptime &dt );

protected:

  std::vector<ptime> m_vDateTime;
  std::vector<double> m_vChartTime;  // used by ChartDir
  virtual void Reserve( unsigned int );
  DoubleArray GetDateTimes( void ) const {
    vdouble_t::const_iterator iter = m_vChartTime.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vChartTime.size() ) );
  }

private:
};

} // namespace ou
