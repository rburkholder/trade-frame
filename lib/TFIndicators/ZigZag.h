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

// Bollinger page 91 suggests 0.17 * sqrt( price ) is a good filter width
// could set this number on each new peak

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace tf { // TradeFrame

class ZigZag{
public:

  ZigZag( void );
  ZigZag(double FilterWidth);
  ~ZigZag(void);

  void SetFilterWidth( double width ) { m_dblFilterWidth = width; };
  double GetFilterWidth( void ) const { return m_dblFilterWidth; };

  void Check( ptime dt, double val );

  enum EDirection { Init, Start, Down, Up };

  typedef FastDelegate4<ZigZag*, ptime, double, EDirection> OnPeakFoundHandler;
  void SetOnPeakFound( OnPeakFoundHandler function ) {
    OnPeakFound = function; 
  }

  typedef FastDelegate1<ZigZag*> OnDecisionPointFoundHandler;
  void SetUpDecisionPointFound( OnDecisionPointFoundHandler function ) {
    UpDecisionPointFound = function;
  }
  void SetDnDecisionPointFound( OnDecisionPointFoundHandler function ) {
    DnDecisionPointFound = function;
  }

protected:
  double m_dblFilterWidth; // pt1 becomes new anchor when abs(pt0-pt1)>delta
  int m_cntNewUp, m_cntNewDown, m_cntTurns;
  EDirection m_PatternState;

private:
  double m_dblPatternPt0,  // pattern end point, drags pt1 away from anchor, but can retrace at will
    m_dblPatternPt1; // pattern mid point, can only move away from anchor point
  ptime m_dtPatternPt1;   // when it was last encountered
  OnPeakFoundHandler OnPeakFound;
  OnDecisionPointFoundHandler UpDecisionPointFound, DnDecisionPointFound;
};

} // namespace tf
} // namespace ou
