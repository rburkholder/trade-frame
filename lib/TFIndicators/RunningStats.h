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

class RunningStats {
public:
  RunningStats(void);
  RunningStats(double BBMultiplier);
  virtual ~RunningStats(void);
  void SetBBMultiplier( double dbl ) { m_BBMultiplier = dbl; };
  double GetBBMultiplier( void ) const { return m_BBMultiplier; };

  void Add( double, double );
  void Remove( double, double );
  virtual void CalcStats( void );
  void Reset( void );

//  double B2() const { return b2; }; // acceleration
  double Slope( void ) const { return b1; }; // slope  B1  termios.h has this as #define
  double Offset( void ) const { return b0; }; // offset B0

  double MeanY( void ) const { return meanY; };

  double RR( void ) const { return rr; };
  double R( void ) const { return r; };

  double SD( void ) const { return sd; };

  double BBOffset( void ) const { return sd * m_BBMultiplier; };
  double BBUpper( void ) const { return meanY + sd * m_BBMultiplier; };
  double BBLower( void ) const { return  meanY - sd * m_BBMultiplier; };

protected:

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
private:
};

} // namespace tf
} // namespace ou
