/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

namespace ou { // One Unified
namespace tf { // TradeFrame

// RunningStats has no events
//   CalcStats could be inherited, and called, then event processed in higher levels

class RunningStats {
public:

  RunningStats();
  RunningStats( double BBMultiplier );
  RunningStats( const RunningStats& );
  RunningStats( const RunningStats&& );
  virtual ~RunningStats();

  void SetBBMultiplier( double dbl ) { m_BBMultiplier = dbl; };
  double GetBBMultiplier() const { return m_BBMultiplier; };

  void Add( double x, double y );
  void Remove( double x, double y );
  virtual void CalcStats();
  void Reset();

//  double B2() const { return b2; }; // acceleration
  double Slope() const { return b1; }; // slope  B1  termios.h has this as #define
  double Offset() const { return b0; }; // offset B0

  double MeanY() const { return meanY; };

  double RR() const { return rr; };
  double R() const { return r; };

  double SD() const { return sd; };

  double BBOffset() const { return sd * m_BBMultiplier; };
  double BBUpper() const { return meanY + sd * m_BBMultiplier; };
  double BBLower() const { return  meanY - sd * m_BBMultiplier; };

protected:
private:

//  double b2; // acceleration
  double b1; // slope
  double b0; // offset

  double meanY;

  double rr;
  double r;

  double sd;

//  double bbUpper, bbLower;

  unsigned int nX, nY;
  double SumXX, SumX, SumXY, SumY, SumYY;
  double m_BBMultiplier;
};

} // namespace tf
} // namespace ou
