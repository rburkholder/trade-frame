#pragma once

// 
// Tools are meant for drawing in world coordinates:  
//   at a price level, or at a price/time intersection
//

// inheriting classes need to be aware of how Size is calculated, as it 
//   will be used by the charting application for determinig if 
//   it will be calculating the DoubleArray parameter for the charting library

#ifdef RGB
#undef RGB
// windows COLORREF is backwards from what ChartDir is expecting
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#endif
#include "Colour.h"

#include <vector>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

//#include "ChartDirector\FinanceChart.h"
#include "ChartDirector\memblock.h"
#include "ChartDirector\chartviewer.h"

class CChartEntryBase {
public:
  CChartEntryBase( void );
  CChartEntryBase( unsigned int nSize );
  virtual ~CChartEntryBase( void );
  Colour::enumColour GetColour( void ) { return m_eColour; };
  virtual void SetColour( Colour::enumColour colour ) { m_eColour = colour; };
  void Name( std::string name ) { m_sName = name; };
  const std::string &Name( void ) { return m_sName; };
  void Add( double price );
  typedef std::vector<double> vdouble_t;
  virtual size_t Size( void ) { return m_vPrice.size(); };
  virtual void AddDataToChart( XYChart *pXY ) {};
protected:
  virtual void Reserve( unsigned int );
  std::vector<double> m_vPrice;
  Colour::enumColour m_eColour;
  std::string m_sName;
  DoubleArray GetPrice( void ) {
    vdouble_t::iterator iter = m_vPrice.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vPrice.size() ) );
  }
private:
};

class CChartEntryBaseWithTime : public CChartEntryBase {
public:
  CChartEntryBaseWithTime( void );
  CChartEntryBaseWithTime( unsigned int nSize );
  virtual ~CChartEntryBaseWithTime( void );
  void Add( const ptime &dt, double price );
  void Add( const ptime &dt );
protected:
  std::vector<ptime> m_vDateTime;
  std::vector<double> m_vChartTime;  // used by ChartDir
  virtual void Reserve( unsigned int );
  DoubleArray GetDateTime( void ) {
    vdouble_t::iterator iter = m_vChartTime.begin();
    return DoubleArray( &(*iter), static_cast<int>( m_vChartTime.size() ) );
  }
private:
};
