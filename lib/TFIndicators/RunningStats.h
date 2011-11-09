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

namespace ou { // One Unified
namespace tf { // TradeFrame

class CRunningStats {
public:
  CRunningStats(void);
  CRunningStats(double BBMultiplier);
  virtual ~CRunningStats(void);
  void SetBBMultiplier( double dbl ) { m_BBMultiplier = dbl; };
  double GetBBMultiplier( void ) const { return m_BBMultiplier; };

  void Add( double, double );
  void Remove( double, double );
  virtual void CalcStats( void );
  void Reset( void );

  double B2() const { return b2; }; // acceleration
  double B1() const { return b1; }; // slope
  double B0() const { return b0; }; // offset

  double MeanY() const { return meanY; };

  double RR() const { return rr; };
  double R() const { return r; };

  double SD() const { return sd; };

  double BBUpper() const { return bbUpper; };
  double BBLower() const { return bbLower; };

protected:

  double b2; // acceleration
  double b1; // slope
  double b0; // offset

  double meanY;

  double rr;
  double r;

  double sd;

  double bbUpper, bbLower;

  unsigned int nX, nY;
  double SumXX, SumX, SumXY, SumY, SumYY;
  double m_BBMultiplier;
private:
};

} // namespace tf
} // namespace ou
