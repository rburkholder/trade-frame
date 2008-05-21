#pragma once

// 
// Tools are meant for drawing in world coordinates:  at a price level, or at a price/time intersection
//

#include "..\LibCommon\Color.h"

#include <vector>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
//#include "boost/date_time/gregorian/gregorian_types.hpp" 
using namespace boost::posix_time;
using namespace boost::gregorian;

class CChartEntryBase {
public:
  CChartEntryBase( void );
  CChartEntryBase( unsigned int nSize );
  virtual ~CChartEntryBase( void );
  void Color( EColor color ) { m_eColor = color; };
  EColor Color( void ) { return m_eColor; };
  void Name( std::string name ) { m_sName = name; };
  const std::string &Name( void ) { return m_sName; };
  void Add( double price );
protected:
  virtual void Reserve( unsigned int );
  std::vector<double> m_vPrice;
  EColor m_eColor;
  std::string m_sName;
private:
};

class CChartEntryBaseWithTime : public CChartEntryBase {
public:
  CChartEntryBaseWithTime( void );
  CChartEntryBaseWithTime( unsigned int nSize );
  virtual ~CChartEntryBaseWithTime( void );
  void Add( ptime dt, double price );
protected:
  std::vector<ptime> m_vDateTime;
  std::vector<double> m_vChartTime;  // used by ChartDir
  virtual void Reserve( unsigned int );
private:
};
