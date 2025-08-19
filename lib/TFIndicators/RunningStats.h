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
namespace linear {
  struct Stats {
    double b1; // slope
    double b0; // offset
    double meanY;
    double rr;
    double r;
    double sd;
    Stats(): b1 {}, b0 {}, meanY {}, rr {}, r {}, sd {} {}
    Stats( const Stats& rhs )
    : b1( rhs.b1 ), b0( rhs.b0 ), meanY( rhs.meanY ), rr( rhs.rr ), r( rhs.r ), sd( rhs.sd ) {}
    void Reset() {
      b1 = b0 = meanY = rr = r = sd = 0.0;
    }
  };
}

class RunningStats {
public:

  RunningStats();
  RunningStats( double BBMultiplier );
  RunningStats( const RunningStats& );
  RunningStats( const RunningStats&& );
  virtual ~RunningStats();

  void SetBBMultiplier( double dbl ) { m_BBMultiplier = dbl; }
  double GetBBMultiplier() const { return m_BBMultiplier; }

  void Add( double x, double y );
  void Remove( double x, double y );
  virtual void CalcStats();
  void CalcStats( linear::Stats& );
  void Reset();

  const linear::Stats& Get() const { return m_stats; }

  double Slope() const { return m_stats.b1; } // slope  B1  termios.h has this as #define
  double Offset() const { return m_stats.b0; } // offset B0

  double SumY() const { return m_SumY; }
  double MeanY() const { return m_stats.meanY; }

  double RR() const { return m_stats.rr; }
  double R() const { return m_stats.r; }

  double SD() const { return m_stats.sd; }

  double BBOffset() const { return m_stats.sd * m_BBMultiplier; }
  double BBUpper() const { return m_stats.meanY + m_stats.sd * m_BBMultiplier; }
  double BBLower() const { return  m_stats.meanY - m_stats.sd * m_BBMultiplier; }

protected:
private:

  linear::Stats m_stats;

  unsigned int m_nX;
  double m_SumXX, m_SumX, m_SumXY, m_SumY, m_SumYY;
  double m_BBMultiplier;
};

} // namespace tf
} // namespace ou
