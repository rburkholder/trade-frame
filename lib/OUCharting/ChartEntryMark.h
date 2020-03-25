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

#include <string>
#include <vector>

#include  <TFTimeSeries/DoubleBuffer.h>

#include "ChartEntryBase.h"

// level markers (horizontal lines at a price level)

// 2020/03/24 - seems to lock up data presentation

namespace ou { // One Unified

class ChartEntryMark: public ChartEntryBase {
public:

  struct Mark_t {
    double m_dblPrice;
    ou::Colour::enumColour m_colour;
    std::string m_sName;
    Mark_t( void ): m_dblPrice( 0.0 ), m_colour( ou::Colour::Black ) {};
    Mark_t( double price, ou::Colour::enumColour colour, const std::string &name )
      : m_dblPrice( price ), m_colour( colour ), m_sName( name ) {};
  };

  ChartEntryMark(void);
  virtual ~ChartEntryMark(void);
  void AddMark( double price, ou::Colour::enumColour colour, const std::string &name ); // bg thread
  void AddMark( const Mark_t& mark ); // bg thread
  virtual bool AddEntryToChart( XYChart *pXY, structChartAttributes *pAttributes );
  virtual void Clear( void );
protected:
  std::vector<double> m_vPrice;
  std::vector<ou::Colour::enumColour> m_vColour;
  std::vector<std::string> m_vName;
private:
  typedef ou::tf::Queue<Mark_t> queueMark_t;
  queueMark_t m_queue;
  void Pop( const Mark_t& );
//  boost::lockfree::spsc_queue<Mark_t, boost::lockfree::capacity<lockfreesize> > m_lfMark;
};

} // namespace ou
