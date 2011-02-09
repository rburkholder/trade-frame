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

class CChartEntryVolume: public CChartEntryBaseWithTime {
public:
  CChartEntryVolume(void);
  CChartEntryVolume(unsigned int nSize);
  virtual ~CChartEntryVolume(void);
  virtual void Reserve( unsigned int );
  void Add( const ptime &dt, int volume );
  virtual void AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes );
protected:
private:
};

class CChartEntryBars :
  public CChartEntryBaseWithTime {  
public:
  CChartEntryBars(void);
  CChartEntryBars(unsigned int nSize);
  virtual ~CChartEntryBars(void);
  virtual void Reserve( unsigned int );
  void AddBar( const ou::tf::CBar &bar );
  virtual void AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes );
protected:
  std::vector<double> m_vOpen;
  std::vector<double> m_vHigh;
  std::vector<double> m_vLow;
  std::vector<double> m_vClose;
  DoubleArray GetOpen( void ) {
    vdouble_t::iterator iter = m_vOpen.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vOpen.size() ) );
  }
  DoubleArray GetHigh( void ) {
    vdouble_t::iterator iter = m_vHigh.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vHigh.size() ) );
  }
  DoubleArray GetLow( void ) {
    vdouble_t::iterator iter = m_vLow.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vLow.size() ) );
  }
  DoubleArray GetClose( void ) {
    vdouble_t::iterator iter = m_vClose.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vClose.size() ) );
  }
private:
};

} // namespace ou
