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

#include <LibCommon/FastDelegate.h>
using namespace fastdelegate;

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

class CZigZag {
public:
  CZigZag(double FilterWidth);
  virtual ~CZigZag(void);
  void Check( ptime dt, double val );

  enum EDirection { Init, Start, Down, Up };

  typedef FastDelegate4<CZigZag *, ptime, double, EDirection> OnPeakFoundHandler;
  void SetOnPeakFound( OnPeakFoundHandler function ) {
    OnPeakFound = function; 
  }

  typedef FastDelegate1<CZigZag *> OnDecisionPointFoundHandler;
  void SetUpDecisionPointFound( OnDecisionPointFoundHandler function ) {
    UpDecisionPointFound = function;
  }
  void SetDnDecisionPointFound( OnDecisionPointFoundHandler function ) {
    DnDecisionPointFound = function;
  }

protected:
  double m_dblFilterWidth; // pt1 becomes new anchor when abs(pt0-pt1)>delta
  int cntNewUp, cntNewDown, cntTurns;
  EDirection m_PatternState;

private:
  double m_dblPatternPt0,  // pattern end point, drags pt1 away from anchor, but can retrace at will
    m_dblPatternPt1; // pattern mid point, can only move away from anchor point
  ptime m_dtPatternPt1;   // when it was last encountered
  OnPeakFoundHandler OnPeakFound;
  OnDecisionPointFoundHandler UpDecisionPointFound, DnDecisionPointFound;
};
