#pragma once

#include "ChartEntryBase.h"

#include "DatedDatum.h"

class CChartEntryVolume: public CChartEntryBaseWithTime {
public:
  CChartEntryVolume(void);
  CChartEntryVolume(unsigned int nSize);
  virtual ~CChartEntryVolume(void);
  virtual void Reserve( unsigned int );
  void Add( const ptime &dt, double volume );
  virtual void AddDataToChart( XYChart *pXY );
protected:
private:
};

class CChartEntryBars :
  public CChartEntryVolume {  
    // derived from CChartEntryVolume so 
    //   can do bar in one chart frame and volume in another
public:
  CChartEntryBars(void);
  CChartEntryBars(unsigned int nSize);
  virtual ~CChartEntryBars(void);
  virtual void Reserve( unsigned int );
  void AddBar( const CBar &bar );
  virtual void AddDataToChart( XYChart *pXY );
protected:
  std::vector<double> m_vOpen;
  std::vector<double> m_vHigh;
  std::vector<double> m_vLow;
  std::vector<double> m_vClose;
  //std::vector<double> m_vVolume;
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
  //DoubleArray GetVolume( void ) {
  //  vdouble_t::iterator iter = m_vVolume.begin();
  //  return DoubleArray( &(*iter), m_vVolume.size() );
  //}
private:
};
